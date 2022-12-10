// Copyright (C) 2016-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RAZERGENIE_H
#define RAZERGENIE_H

#include "ui_razergenie.h"

#include <QSettings>
#include <libopenrazer.h>

class RazerGenie : public QWidget
{
    Q_OBJECT
public:
    RazerGenie(QWidget *parent = nullptr);
    ~RazerGenie() override;
public slots:
    // General checkboxes
    void toggleSync(bool);
    void toggleOffOnScreesaver(bool on);

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

    QHash<QDBusObjectPath, libopenrazer::Device *> devices;
    libopenrazer::Manager *manager;

    QSettings settings;
};

#endif
