// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CUSTOMEDITOR_H
#define CUSTOMEDITOR_H

#include "matrixpushbutton.h"

#include <QDialog>
#include <QJsonObject>
#include <libopenrazer.h>

enum DrawStatus {
    set,
    clear
};

class CustomEditor : public QDialog
{
    Q_OBJECT
public:
    CustomEditor(libopenrazer::Device *device, bool launchMatrixDiscovery = false, QWidget *parent = nullptr);
    ~CustomEditor() override;

private:
    void closeWindow();
    QLayout *buildMainControls();
    QLayout *buildKeyboard();
    QLayout *buildMousemat();
    QLayout *buildFallback();
    QLayout *buildLayoutFromJson(QJsonObject layout);

    QJsonDocument loadMatrixLayoutJson(QString jsonname);
    bool updateKeyrow(int row);
    void clearAll();

    QVector<MatrixPushButton *> matrixPushButtons;
    libopenrazer::Device *device;
    openrazer::MatrixDimensions dimens;

    QVector<QVector<QColor>> colors;
    QColor selectedColor;
    DrawStatus drawStatus;
private slots:
    void colorButtonClicked();
    void onMatrixPushButtonClicked();
};

#endif // CUSTOMEDITOR_H
