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
#include <QStandardPaths>
#include <QFileInfo>

#include "razerimagedownloader.h"


RazerImageDownloader::RazerImageDownloader(QUrl url, QNetworkAccessManager *manager, QObject *parent) : QObject(parent)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/kcm_razerdrivers/devicepictures/" + QFileInfo(url.path()).fileName();
    std::cout << path.toStdString() << std::endl;
    _file = new QFile(path);
    if(_file->exists()) {
        // return image or something
    }

    QNetworkRequest request;
    request.setUrl(url);
    std::cout << "Starting download." << std::endl;
    connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(finished(QNetworkReply*)));
    QNetworkReply *rpl = manager->get(request);
    std::cout << "Starting download2." << std::endl;
    
    
    qDebug()<<rpl->errorString();
    qDebug()<<rpl->error();
    qDebug()<<"translateText ends";
    std::cout << "output" << std::endl;
}

RazerImageDownloader::~RazerImageDownloader()
{
    
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
}

#include "razerimagedownloader.moc"
