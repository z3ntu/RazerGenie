// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QSettings>
#include <libopenrazer.h>

class Preferences : public QDialog
{
    Q_OBJECT
public:
    Preferences(libopenrazer::Manager *manager, QWidget *parent = nullptr);
    ~Preferences() override;

private:
    QSettings settings;

    libopenrazer::Manager *manager;
};

#endif // PREFERENCES_H
