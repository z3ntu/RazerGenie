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

#ifndef DPISLIDERWIDGET_H
#define DPISLIDERWIDGET_H

#include <QWidget>
#include <libopenrazer.h>

class DpiSliderWidget : public QWidget
{
public:
    DpiSliderWidget(QWidget *parent, libopenrazer::Device *device);

public slots:
    // DPI checkbox & slider
    void dpiChanged(int value);

private:
    bool syncDpi = true;
    libopenrazer::Device *device;
};

#endif // DPISLIDERWIDGET_H
