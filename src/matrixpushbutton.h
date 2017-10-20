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

#ifndef MATRIXPUSHBUTTON_H
#define MATRIXPUSHBUTTON_H

#include <QPushButton>

class MatrixPushButton : public QPushButton
{
public:
    MatrixPushButton(QString label);
    void setMatrixPos(int matrixX, int matrixY);
    QPair<int, int> matrixPos();
    void setButtonColor(QColor color);
private:
    QString mLabel;
    QPair<int, int> mMatrixPos;
};

#endif // MATRIXPUSHBUTTON_H
