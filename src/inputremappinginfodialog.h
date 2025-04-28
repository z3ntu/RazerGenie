// Copyright (C) 2018-2024  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INPUTREMAPPINGINFODIALOG_H
#define INPUTREMAPPINGINFODIALOG_H

#include <QDialog>

class InputRemappingInfoDialog : public QDialog
{
    Q_OBJECT
public:
    InputRemappingInfoDialog(QWidget *parent = nullptr);
    ~InputRemappingInfoDialog() override;
};

#endif // INPUTREMAPPINGINFODIALOG_H
