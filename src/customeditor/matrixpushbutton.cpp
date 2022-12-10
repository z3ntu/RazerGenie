// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "matrixpushbutton.h"

#include <QDebug>
#include <QStyle>

MatrixPushButton::MatrixPushButton(QString label)
    : QPushButton(label)
{
    // TODO: Get rid of light blue "selected" color - can get rid of with setFlat(true) or using a qlineargradient in the stylesheet
    mLabel = label;
}

void MatrixPushButton::setMatrixPos(int matrixX, int matrixY)
{
    mMatrixPos = qMakePair(matrixX, matrixY);
}

QPair<int, int> MatrixPushButton::matrixPos()
{
    return mMatrixPos;
}

void MatrixPushButton::setButtonColor(QColor color)
{
    // Calculate "the perfect font color" - from https://24ways.org/2010/calculating-color-contrast/
    double yiq = ((color.red() * 299) + (color.green() * 587) + (color.blue() * 114)) / 1000;
    QString fontcolor = (yiq >= 128) ? "black" : "white";

    // Construct QPalette with the wanted color
    QPalette p = QPalette(color);
    // Set the font color to the previously calculated value
    p.setColor(QPalette::ButtonText, fontcolor);
    this->setPalette(p);
}

void MatrixPushButton::resetButtonColor()
{
    this->setPalette(this->style()->standardPalette());
}
