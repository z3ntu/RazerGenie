// Copyright (C) 2017-2024  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIGHTINGWIDGET_H
#define LIGHTINGWIDGET_H

#include <QWidget>
#include <libopenrazer.h>

class LightingWidget : public QWidget
{
    Q_OBJECT
public:
    LightingWidget(libopenrazer::Device *device);
    ~LightingWidget() override;

    static bool isAvailable(libopenrazer::Device *device);

private:
    libopenrazer::Device *device;

    void openCustomEditor(bool forceFallback);
};

#endif // LIGHTINGWIDGET_H
