/*
 * Copyright (C) 2017  Luca Weiss <luca (at) z3ntu (dot) xyz>
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

#ifndef DEVICELISTWIDGET_H
#define DEVICELISTWIDGET_H

#include <librazer.h>
#include <QWidget>
#include <QLabel>

class DeviceListWidget : public QWidget
{
    Q_OBJECT
public:
    DeviceListWidget(QWidget *parent, librazer::Device *device);
    librazer::Device *device();
    void setNoImage();
public slots:
    void imageDownloaded(QString &filename);
private:
    QPixmap createPixmapFromFile(QString &filename);
    librazer::Device *mDevice;
    QLabel *imageLabel;
};

#endif // DEVICELISTWIDGET_H
