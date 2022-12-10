// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "devicelistwidget.h"

#include "razerimagedownloader.h"

#include <QFileInfo>
#include <QLabel>
#include <QVBoxLayout>

DeviceListWidget::DeviceListWidget(QWidget *parent, libopenrazer::Device *device)
    : QWidget(parent)
{
    this->mDevice = device;

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);

    // Add icon
    QString path = RazerImageDownloader::getDownloadPath() + device->getDeviceImageUrl().split("/").takeLast();
    if (QFile(path).exists() && QFileInfo(path).isFile()) {
        QPixmap scaled = createPixmapFromFile(path);
        imageLabel = new QLabel(this);
        imageLabel->setPixmap(scaled);
    } else {
        imageLabel = new QLabel(tr("Downloading image..."), this);
    }
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setWordWrap(true);
    layout->addWidget(imageLabel);

    QLabel *deviceName = new QLabel(device->getDeviceName(), this);
    deviceName->setWordWrap(true);
    deviceName->setAlignment(Qt::AlignCenter);
    layout->addWidget(deviceName);
}

QPixmap DeviceListWidget::createPixmapFromFile(QString &filename)
{
    QPixmap icon(filename);
    return icon.scaled(150, 75, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void DeviceListWidget::imageDownloaded(QString &filename)
{
    qDebug() << "DeviceListWidget: Received signal!" << filename;
    QPixmap scaled = createPixmapFromFile(filename);
    imageLabel->setPixmap(scaled);
}

void DeviceListWidget::imageDownloadErrored(QString reason, QString longReason)
{
    qDebug() << "DeviceListWidget: Received errored signal!";
    qDebug() << "DeviceListWidget: Reason:" << reason;
    qDebug() << "DeviceListWidget: Long reason:" << longReason;
    imageLabel->setText(reason);
}

libopenrazer::Device *DeviceListWidget::device()
{
    return mDevice;
}

void DeviceListWidget::setNoImage()
{
    imageLabel->setText(tr("No image"));
}
