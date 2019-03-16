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

#ifndef RAZERGENIE_H
#define RAZERGENIE_H

#include "ui_razergenie.h"
#include "razerimagedownloader.h"

#include <libopenrazer.h>

#include <QComboBox>

class RazerGenie : public QWidget
{
    Q_OBJECT
public:
    RazerGenie(QWidget* parent = nullptr);
    ~RazerGenie() override;
public slots:
    // General checkboxes
    void toggleSync(bool);
    void toggleOffOnScreesaver(bool on);

    // DPI checkbox & slider
    void dpiChanged(int value);
    void dpiSyncCheckbox(bool checked);

    // Poll combobox
    void pollCombo(int /* index */);

    void openCustomEditor(bool openMatrixDiscovery = false);
    void openPreferences();

    void dbusServiceRegistered(const QString &serviceName);
    void dbusServiceUnregistered(const QString &serviceName);

    // device signals
    void devicesChanged();

    void openIssueUrl();
    void openSupportedDevicesUrl();
    void openTroubleshootingUrl();
    void openWebsiteUrl();
private:
    Ui::RazerGenieUi ui_main;
    void setupUi();

    QWidget *noDevicePlaceholder = nullptr;

    QList<QPair<int, int>> getConnectedDevices_lsusb();

    void fillDeviceList();
    void refreshDeviceList();
    void clearDeviceList();

    void addDeviceToGui(const QDBusObjectPath &devicePath);
    bool removeDeviceFromGui(const QDBusObjectPath &devicePath);
    QWidget *getNoDevicePlaceholder();

    void getRazerDevices();

    bool syncDpi = true;

    QHash<QDBusObjectPath, libopenrazer::Device*> devices;
    libopenrazer::Manager *manager;
};


#endif
