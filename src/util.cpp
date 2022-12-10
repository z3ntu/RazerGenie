// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "util.h"

#include <QMessageBox>

void util::showError(QString error)
{
    QMessageBox messageBox;
    messageBox.critical(nullptr, QMessageBox::tr("Error!"), error);
    messageBox.setFixedSize(500, 200);
}

void util::showInfo(QString info)
{
    QMessageBox messageBox;
    messageBox.information(nullptr, QMessageBox::tr("Information!"), info);
    messageBox.setFixedSize(500, 200);
}
