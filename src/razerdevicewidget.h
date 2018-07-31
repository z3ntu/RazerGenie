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

#ifndef RAZERDEVICEWIDGET_H
#define RAZERDEVICEWIDGET_H

#include <QWidget>
#include <QDBusObjectPath>

class RazerDeviceWidget : public QWidget
{
public:
    RazerDeviceWidget(const QString& name, const QDBusObjectPath& devicePath);
    ~RazerDeviceWidget();

    QDBusObjectPath getDevicePath();
private:
    QString name;
    QDBusObjectPath devicePath;
};

#endif // RAZERDEVICEWIDGET_H
