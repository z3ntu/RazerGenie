// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
