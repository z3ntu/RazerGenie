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

#ifndef KCM_RAZERDRIVERS_H
#define KCM_RAZERDRIVERS_H

#include <kcmodule.h>
#include "ui_kcm_razerdrivers.h"
#include "razerimagedownloader.h"
#include "razermethods.h"

class kcm_razerdrivers : public KCModule
{
    Q_OBJECT
public:
    kcm_razerdrivers(QWidget* parent, const QVariantList& args);
    ~kcm_razerdrivers();
public slots:
    void toggleSync(bool);
    void imageDownloaded(QString &serial, QString &filename);

    void standardColorButton();
    void logoColorButton();
    void scrollColorButton();
    void standardCombo(const QString &text);
    void scrollCombo(const QString &text);
    void logoCombo(const QString &text);
    void brightnessChanged(int value);
    void scrollBrightnessChanged(int value);
    void logoBrightnessChanged(int value);
private:
    Ui::RazerDriversKcmUi ui;
    void getRazerDevices(void);
    void fillList();
    void showError(QString error);
    void showInfo(QString info);

    QHash<QString, razermethods::Device*> devices;
};


#endif
