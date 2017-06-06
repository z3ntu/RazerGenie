/*
 * Copyright (C) 2016-2017  Luca Weiss <luca (at) z3ntu (dot) xyz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>

#include <QString>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QDataStream>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>

#include "razerimagedownloader.h"


RazerImageDownloader::RazerImageDownloader(QUrl url, QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);

    QString path = getDownloadPath();
    // Create directory
    QDir dir(path);
    dir.mkpath(path);

    _filepath = path + QFileInfo(url.path()).fileName();
    _file = new QFile(_filepath);
    if(_file->exists()) {
        return;
    }

    // TODO: What happens when two devices of the same model are connected?

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "Mozilla Firefox");

    connect(manager, &QNetworkAccessManager::finished, this, &RazerImageDownloader::finished);

    manager->get(request);
}

RazerImageDownloader::~RazerImageDownloader()
{
    // TODO: Complete destructor (no idea how they work / what you have to do)
    delete _file;
    delete manager;
}

void RazerImageDownloader::finished(QNetworkReply* reply)
{
    qDebug() << "Finished downloading the picture.";

    _file->open(QFile::WriteOnly);
    _file->write(reply->readAll());
    _file->flush();
    _file->close();

    reply->deleteLater();

    emit downloadFinished(_filepath);
}

QString RazerImageDownloader::getDownloadPath()
{
    // Should be ~/.local/share/razergenie/devicepictures/
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/razergenie/devicepictures/";
}

#include "razerimagedownloader.moc"
