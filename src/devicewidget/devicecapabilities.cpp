// SPDX-License-Identifier: GPL-3.0-or-later
#include "devicecapabilities.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QXmlStreamReader>
QSet<QString> deviceMethods(const QDBusObjectPath &path, const QString &interface)
{
    auto request = QDBusMessage::createMethodCall("org.razer", path.path(), "org.freedesktop.DBus.Introspectable", "Introspect");
    QDBusReply<QString> reply = QDBusConnection::sessionBus().call(request);
    if (!reply.isValid())
        return {};
    QSet<QString> methods;
    QXmlStreamReader xml(reply.value());
    bool matchingInterface = false;
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == QStringLiteral("interface"))
            matchingInterface = xml.attributes().value("name") == interface;
        else if (xml.isEndElement() && xml.name() == QStringLiteral("interface"))
            matchingInterface = false;
        else if (matchingInterface && xml.isStartElement() && xml.name() == QStringLiteral("method"))
            methods.insert(xml.attributes().value("name").toString());
    }
    return xml.hasError() ? QSet<QString>() : methods;
}
