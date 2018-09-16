/*
 * Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
