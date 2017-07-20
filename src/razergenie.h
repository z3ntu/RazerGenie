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

#ifndef RAZERGENIE_H
#define RAZERGENIE_H

#include "ui_razergenie.h"
#include "razerimagedownloader.h"
#include "librazer/librazer.h"
#include <QComboBox>

class RazerGenie : public QWidget
{
    Q_OBJECT
public:
    RazerGenie(QWidget* parent = 0);
    ~RazerGenie();
public slots:
    // General checkboxes
    void toggleSync(bool);
    void toggleOffOnScreesaver(bool on);

    // Color buttons
    void colorButtonClicked();

    // Effect comboboxes
    void standardCombo(int index);
    void scrollCombo(int index);
    void logoCombo(int index);

    // Brightness sliders
    void brightnessChanged(int value);
    void scrollBrightnessChanged(int value);
    void logoBrightnessChanged(int value);

    void waveRadioButtonStandard(bool enabled);
    void waveRadioButtonLogo(bool enabled);
    void waveRadioButtonScroll(bool enabled);

    // DPI checkbox & slider
    void dpiChanged(int value);
    void dpiSyncCheckbox(bool checked);

    // Poll combobox
    void pollCombo(int /* index */);

    // 'Set active' checkbox
    void logoActiveCheckbox(bool checked);
    void scrollActiveCheckbox(bool checked);

    void openCustomEditor();

    void dbusServiceRegistered(const QString &serviceName);
    void dbusServiceUnregistered(const QString &serviceName);

    // device signals
    void deviceAdded();
    void deviceRemoved();

    void openIssueUrl();
private:
    Ui::RazerGenieUi ui_main;
    void setupUi();

    QWidget *noDevicePlaceholder = NULL;

    QList<QPair<int, int>> getConnectedDevices_lsusb();

    void fillDeviceList();
    void refreshDeviceList();
    void clearDeviceList();

    void addDeviceToGui(const QString &serial);
    bool removeDeviceFromGui(const QString &serial);
    QWidget *getNoDevicePlaceholder();
    void showError(QString error);
    void showInfo(QString info);

    QPair<librazer::Device*, QString> commonCombo(int index);

    void getRazerDevices(void);
    QColor getColorForButton(int num, librazer::Device::lightingLocation location);
    int getWaveDirection(librazer::Device::lightingLocation location);

    void applyEffect(librazer::Device::lightingLocation location);
    void applyEffectStandardLoc(QString identifier, librazer::Device *device);
    void applyEffectLogoLoc(QString identifier, librazer::Device *device);
    void applyEffectScrollLoc(QString identifier, librazer::Device *device);

    bool syncDpi = true;

    QHash<QString, librazer::Device*> devices;
};


#endif
