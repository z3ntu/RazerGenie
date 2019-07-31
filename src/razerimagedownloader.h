/*
 * Copyright (C) 2016-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
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

#ifndef RAZERIMAGEDOWNLOADER_H
#define RAZERIMAGEDOWNLOADER_H

#include <QFile>
#include <QSettings>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

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
