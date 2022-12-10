// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UTIL_H
#define UTIL_H

#include <QString>

#define QCOLOR_TO_RGB(c)                       \
    openrazer::RGB                             \
    {                                          \
        static_cast<uchar>(c.red()),           \
                static_cast<uchar>(c.green()), \
                static_cast<uchar>(c.blue())   \
    }

namespace util {
void showError(QString error);
void showInfo(QString info);
}

#endif // UTIL_H
