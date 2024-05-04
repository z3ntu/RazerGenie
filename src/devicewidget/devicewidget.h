// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QDBusObjectPath>
#include <QWidget>
#include <libopenrazer.h>

class DeviceWidget : public QWidget
{
    Q_OBJECT
public:
    DeviceWidget(libopenrazer::Device *device);
    ~DeviceWidget() override;
};

#endif // DEVICEWIDGET_H
