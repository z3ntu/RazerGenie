// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DPICOMBOBOXWIDGET_H
#define DPICOMBOBOXWIDGET_H

#include <QWidget>
#include <libopenrazer.h>

class DpiComboBoxWidget : public QWidget
{
    Q_OBJECT
public:
    DpiComboBoxWidget(QWidget *parent, libopenrazer::Device *device);

public slots:
    void dpiChanged(int /* value */);

private:
    libopenrazer::Device *device;
};

#endif // DPICOMBOBOXWIDGET_H
