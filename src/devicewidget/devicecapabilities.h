// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DEVICECAPABILITIES_H
#define DEVICECAPABILITIES_H
#include <QDBusObjectPath>
#include <QSet>
#include <QString>
QSet<QString> deviceMethods(const QDBusObjectPath &path, const QString &interface);
#endif
