/*
 * Copyright (C) 2017  Luca Weiss <luca (at) z3ntu (dot) xyz>
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

#ifndef CUSTOMEDITOR_H
#define CUSTOMEDITOR_H

#include <QWidget>
#include <QJsonObject>
#include <librazer.h>

class CustomEditor : public QWidget
{
    Q_OBJECT
public:
    CustomEditor(librazer::Device* device, QWidget* parent = 0);
    ~CustomEditor();
public slots:
    void colorButtonClicked();
private:
    void closeWindow();
    QLayout* generateMainControls();
    QLayout* generateKeyboard();
    QLayout* generateMousemat();
    QLayout* generateMouse();
    bool parseKeyboardJSON();

    QJsonObject keyboardKeys;
    librazer::Device *device;

    QVector<QVector<QColor>> colors;
    QColor selectedColor;
};

#endif // CUSTOMEDITOR_H
