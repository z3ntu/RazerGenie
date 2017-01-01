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

#ifndef RAZERIMAGEDOWNLOADER_H
#define RAZERIMAGEDOWNLOADER_H

#include <QFile>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class RazerImageDownloader : public QObject
{
    Q_OBJECT
public:
    RazerImageDownloader(QString serial, QUrl url, QObject *parent);
    ~RazerImageDownloader();
    static QString getDownloadPath();
signals:
    void downloadFinished(QString &serial, QString &filename);
private:
    QFile *_file;
    QString _filepath;
    QString serial;
    QNetworkAccessManager *manager;
    int _timerid;
    void timerEvent(QTimerEvent *event);
private slots:
    void finished(QNetworkReply* reply);
};

#endif // RAZERIMAGEDOWNLOADER_H
