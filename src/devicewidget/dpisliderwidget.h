// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DPISLIDERWIDGET_H
#define DPISLIDERWIDGET_H

#include "dpistagewidget.h"

#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>
#include <libopenrazer.h>

class DpiSliderWidget : public QWidget
{
    Q_OBJECT
public:
    DpiSliderWidget(QWidget *parent, libopenrazer::Device *device);

private:
    libopenrazer::Device *device;

    bool singleStage;

    uchar activeStage;
    QVector<openrazer::DPI> dpiStages;

    QVector<DpiStageWidget *> dpiStageWidgets;

    void handleStageUpdates();
};

#endif // DPISLIDERWIDGET_H
