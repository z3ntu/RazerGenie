// Copyright (C) 2017-2024  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef POWERWIDGET_H
#define POWERWIDGET_H

#include <QWidget>
#include <libopenrazer.h>

class PowerWidget : public QWidget
{
    Q_OBJECT
public:
    PowerWidget(libopenrazer::Device *device);
    ~PowerWidget() override;

    static bool isAvailable(libopenrazer::Device *device);

private:
    libopenrazer::Device *device;
};

#endif // POWERWIDGET_H
