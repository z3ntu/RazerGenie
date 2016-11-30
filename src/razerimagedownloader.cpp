/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  Luca Weiss <luca@z3ntu.xyz>
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


RazerImageDownloader::RazerImageDownloader(QString serial, QUrl url, QNetworkAccessManager *manager, QObject *parent) : QObject(parent)
{
    this->serial = serial;

    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/kcm_razerdrivers/devicepictures/";
    QDir dir(path);
    dir.mkpath(path);

    _filepath = path + QFileInfo(url.path()).fileName();
    std::cout << _filepath.toStdString() << std::endl;
    _file = new QFile(_filepath);
    if(_file->exists()) {
        // return image, can't emit here as the signal isn't connected yet, see http://stackoverflow.com/a/11641871/3527128
        _timerid = startTimer(0);
        //std::cout << "file already exists" << std::endl;
        return;
    } else {
        // TODO: Solve better
        // That the contructor creates the file, that the same file doesn't get downloaded simultanously.
        _file->open(QIODevice::WriteOnly);
        _file->close();
    }

    QNetworkRequest request;
    request.setUrl(url);
    //std::cout << "Starting download." << std::endl;
    connect(manager, &QNetworkAccessManager::finished, this, &RazerImageDownloader::finished);
    manager->get(request);
}

void RazerImageDownloader::timerEvent(QTimerEvent *event)
{
    killTimer(_timerid);
    emit downloadFinished(serial, _filepath);
}

RazerImageDownloader::~RazerImageDownloader()
{
    // TODO: Complete destructor
    delete _file;
}

void RazerImageDownloader::finished(QNetworkReply* reply)
{
    std::cout << "Finished downloading the picture." << std::endl;
    // Save the image here
    QByteArray b = reply->readAll();
    _file->open(QIODevice::WriteOnly);
    QDataStream out(_file);
    out << b;
    reply->deleteLater();
    // done

    emit downloadFinished(serial, _filepath);
}

#include "razerimagedownloader.moc"
