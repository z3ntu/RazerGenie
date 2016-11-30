/*
 * Copyright (C) 2016  Luca Weiss <luca (at) z3ntu (dot) xyz>
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
 *
 */

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDebug>
#include <iostream>

#include "razermethods.h"

namespace razermethods
{
/* General methods */
QStringList getConnectedDevices() {
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getDevices");
    return QDBusMessageToStringList(m);
}

bool syncDevices(bool yes) {
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "syncEffects");

    // Set arguments
    QList<QVariant> args;
    yes ? args.append("True") : args.append("False"); // maybe bool works here
    m.setArguments(args);

    bool queued = QDBusConnection::sessionBus().send(m);
    std::cout << "Queued: " << queued << std::endl;
    return queued;
}

QString getDriverVersion() {
    QDBusMessage m = prepareGeneralQDBusMessage("razer.daemon", "version");
    return QDBusMessageToString(m);
}

/* Device class methods */
Device::Device(QString s) {
    serial = s;
}

QString Device::getDeviceName() {
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceName");
    return QDBusMessageToString(m);
}

QString Device::getDeviceType() {
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceType");
    return QDBusMessageToString(m);
}

Device::~Device() {

}

/* Helper methods */
QDBusMessage Device::prepareDeviceQDBusMessage(const QString &interface, const QString &method)
{
    return QDBusMessage::createMethodCall("org.razer", "/org/razer/device/" + serial, interface, method);
}

QDBusMessage prepareGeneralQDBusMessage(const QString &interface, const QString &method)
{
    return QDBusMessage::createMethodCall("org.razer", "/org/razer", interface, method);
}

QString QDBusMessageToString(const QDBusMessage &message)
{
    return QDBusConnection::sessionBus().call(message).arguments()[0].toString();
}

QStringList QDBusMessageToStringList(const QDBusMessage &message)
{
    return QDBusConnection::sessionBus().call(message).arguments()[0].toStringList();
}
}

enum devicetype {
    mouse,
    keyboard,
    tartarus
};

int main() {
    std::cout << "Driver version: " << razermethods::getDriverVersion().toStdString() << std::endl;
    QStringList serialnrs = razermethods::getConnectedDevices();
    foreach (const QString &str, serialnrs) {
        std::cout << "-----------------" << std::endl;
        std::cout << "Serial: " << str.toStdString() << std::endl;
        razermethods::Device device = razermethods::Device(str);
        std::cout << "Name:   " << device.getDeviceName().toStdString() << std::endl;
        std::cout << "Type:   " << device.getDeviceType().toStdString() << std::endl;
    }
}

