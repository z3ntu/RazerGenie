/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  Luca Weiss <luca@z3ntu.xyz>
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
 */

#include "matrixpushbutton.h"
#include <QDebug>

MatrixPushButton::MatrixPushButton(QString label) : QPushButton(label)
{
    //TODO: Get rid of light blue "selected" color - can get rid of with setFlat(true) or using a qlineargradient in the stylesheet
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
    double yiq = ((color.red()*299)+(color.green()*587)+(color.blue()*114))/1000;
    QString fontcolor = (yiq >= 128) ? "black" : "white";

    // Construct QPalette with the wanted color
    QPalette p = QPalette(color);
    // Set the font color to the previously calculated value
    p.setColor(QPalette::ButtonText, fontcolor);
    this->setPalette(p);
}
