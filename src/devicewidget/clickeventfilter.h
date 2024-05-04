// Copyright (C) 2022  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CLICKEVENTFILTER_H
#define CLICKEVENTFILTER_H

#include <QObject>

class ClickEventFilter : public QObject
{
    Q_OBJECT
public:
    ClickEventFilter();

signals:
    void shiftClicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // CLICKEVENTFILTER_H
