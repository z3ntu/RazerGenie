/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Luca Weiss <luca@z3ntu.xyz>
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
 */

#ifndef LEDWIDGET_H
#define LEDWIDGET_H

#include <QWidget>

#include <libopenrazer.h>

/**
 * @todo write docs
 */
class LedWidget : public QWidget
{
    Q_OBJECT
public:
    LedWidget(QWidget *parent, libopenrazer::Device *device, libopenrazer::Led *led);
    libopenrazer::Led *mLed;
    libopenrazer::Led *led();

    // Color buttons
    void colorButtonClicked();
    // Effect comboboxes
    void fxComboboxChanged(int index);
    // Brightness sliders
    void brightnessSliderChanged(int value);
    // Wave buttons
    void waveRadioButtonChanged(bool enabled);

    QColor getColorForButton(int num);
    razer_test::WaveDirection getWaveDirection();

    void applyEffect();
    void applyEffectStandardLoc(QString identifier);
};

#endif // LEDWIDGET_H
