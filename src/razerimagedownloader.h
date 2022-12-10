// Copyright (C) 2016-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RAZERIMAGEDOWNLOADER_H
#define RAZERIMAGEDOWNLOADER_H

#include <QFile>
#include <QNetworkReply>
#include <QSettings>

class RazerImageDownloader : public QObject
{
    Q_OBJECT
public:
    RazerImageDownloader(QUrl url, QObject *parent);
    ~RazerImageDownloader() override;
    void startDownload();
    static QString getDownloadPath();
signals:
    void downloadFinished(QString &filename);
    void downloadErrored(QString reason, QString longReason);

private:
    QFile *_file;
    QString _filepath;
    QUrl url;
    QNetworkAccessManager *manager;
    QSettings settings;
private slots:
    void finished(QNetworkReply *reply);
};

#endif // RAZERIMAGEDOWNLOADER_H
