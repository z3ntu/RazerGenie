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

#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QDBusObjectPath>
#include <QWidget>
#include <libopenrazer.h>

class DeviceWidget : public QWidget
{
public:
    DeviceWidget(const QString &name, const QDBusObjectPath &devicePath, libopenrazer::Device *device);
    ~DeviceWidget() override;

    QDBusObjectPath getDevicePath();

public slots:
    // DPI checkbox & slider
    void dpiChanged(int value);
    void dpiSyncCheckbox(bool checked);

    // Poll combobox
    void pollCombo(int /* index */);

    void openCustomEditor(bool openMatrixDiscovery = false);

private:
    QString name;
    QDBusObjectPath devicePath;
    libopenrazer::Device *device;

    bool syncDpi = true;
};

#endif // DEVICEWIDGET_H
