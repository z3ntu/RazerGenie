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
