// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEVICELISTWIDGET_H
#define DEVICELISTWIDGET_H

#include <QLabel>
#include <QWidget>
#include <libopenrazer.h>

class DeviceListWidget : public QWidget
{
    Q_OBJECT
public:
    DeviceListWidget(QWidget *parent, libopenrazer::Device *device);
    libopenrazer::Device *device();
    void setNoImage();
public slots:
    void imageDownloaded(QString &filename);
    void imageDownloadErrored(QString reason, QString longReason);

private:
    QPixmap createPixmapFromFile(QString &filename);
    libopenrazer::Device *mDevice;
    QLabel *imageLabel;
};

#endif // DEVICELISTWIDGET_H
