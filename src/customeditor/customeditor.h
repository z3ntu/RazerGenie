/*
 * Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
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

#include <QDialog>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <libopenrazer.h>
#include "matrixpushbutton.h"
#include "kbdlayout.h"

enum DrawStatus {
    set, clear
};

class CustomEditor : public QDialog, public KbdLayout
{
    Q_OBJECT
public:
    CustomEditor(libopenrazer::Device* device, bool launchMatrixDiscovery=false, QWidget* parent = 0);
    ~CustomEditor();
private:
    void closeWindow();
    QLayout* generateMainControls();
    QLayout* generateKeyboard();
    QLayout* generateMousemat();
    QLayout* generateMouse();
    QLayout* generateMatrixDiscovery();


    bool parseKeyboardJSON(QString jsonname);
    bool updateKeyrow(int row, bool fromfile);
    void clearAll();
    
    void loadLayout();
    void saveLayout();
    QJsonDocument keyboardLayout;
    KbdLayout *klay = new KbdLayout;
    QString KbdFileFilter = tr("JSON File (*.json)");

    QJsonObject keyboardKeys;
    QJsonObject keyboardKeysColors;
    QVector<MatrixPushButton*> matrixPushButtons;
    libopenrazer::Device *device;
    QList<int> dimens;

    QVector<QVector<QColor>> colors;
    QColor selectedColor;
    DrawStatus drawStatus;
private slots:
    void colorButtonClicked();
    void onMatrixPushButtonClicked();
    void setDrawStatusSet();
    void setDrawStatusClear();
};

#endif // CUSTOMEDITOR_H
