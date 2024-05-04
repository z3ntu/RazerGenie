// Copyright (C) 2017-2024  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PERFORMANCEWIDGET_H
#define PERFORMANCEWIDGET_H

#include <QWidget>
#include <libopenrazer.h>

class PerformanceWidget : public QWidget
{
    Q_OBJECT
public:
    PerformanceWidget(libopenrazer::Device *device);
    ~PerformanceWidget() override;

    static bool isAvailable(libopenrazer::Device *device);

private:
    libopenrazer::Device *device;
};

#endif // PERFORMANCEWIDGET_H
