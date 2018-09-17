/*
 * Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
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

#include "devicelistwidget.h"
#include "razerimagedownloader.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QIcon>
#include <QFileInfo>

DeviceListWidget::DeviceListWidget(QWidget *parent, libopenrazer::Device *device) : QWidget(parent)
{
    this->mDevice = device;

    auto *layout = new QVBoxLayout(this);
    layout->setMargin(2);

    // Add icon
    QString path = RazerImageDownloader::getDownloadPath() + device->getPngFilename();
    if(QFile(path).exists() && QFileInfo(path).isFile()) {
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
