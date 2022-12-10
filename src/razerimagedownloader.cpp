// Copyright (C) 2016-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "razerimagedownloader.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

RazerImageDownloader::RazerImageDownloader(QUrl url, QObject *parent)
    : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    this->url = url;
}

void RazerImageDownloader::startDownload()
{
    QString path = getDownloadPath();
    // Create directory
    QDir dir(path);
    dir.mkpath(path);

    _filepath = path + QFileInfo(url.path()).fileName();
    _file = new QFile(_filepath);
    if (_file->exists()) {
        return;
    }
    if (!settings.value("downloadImages").toBool()) {
        emit downloadErrored(tr("Image download disabled"), tr("Image downloading is disabled. Visit the preferences to enable it."));
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

void RazerImageDownloader::finished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit downloadErrored(tr("Network Error"), QVariant::fromValue(reply->error()).toString());
        return;
    }

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
