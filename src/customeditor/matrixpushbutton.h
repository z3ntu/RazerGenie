// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
    void resetButtonColor();

private:
    QString mLabel;
    QPair<int, int> mMatrixPos;
};

#endif // MATRIXPUSHBUTTON_H
