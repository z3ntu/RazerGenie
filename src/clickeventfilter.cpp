// Copyright (C) 2022  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "clickeventfilter.h"

#include <QMouseEvent>

ClickEventFilter::ClickEventFilter()
{
}

bool ClickEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->modifiers() == Qt::ShiftModifier) {
            emit shiftClicked();
            return true;
        }
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}
