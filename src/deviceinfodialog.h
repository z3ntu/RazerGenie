// Copyright (C) 2018-2024  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEVICEINFODIALOG_H
#define DEVICEINFODIALOG_H

#include <QDialog>
#include <libopenrazer.h>

class DeviceInfoDialog : public QDialog
{
    Q_OBJECT
public:
    DeviceInfoDialog(libopenrazer::Device *device, QWidget *parent = nullptr);
    ~DeviceInfoDialog() override;
};

#endif // DEVICEINFODIALOG_H
