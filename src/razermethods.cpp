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
    QStringList getConnectedDevices() {
        QDBusMessage m = QDBusMessage::createMethodCall("org.razer",
                                                    "/org/razer",
                                                    "razer.devices",
                                                    "getDevices");
        QDBusMessage response = QDBusConnection::sessionBus().call(m);
        QStringList serialnrs = response.arguments()[0].toStringList();
        return serialnrs;
    }
    
    bool syncDevices(bool yes) {
        QDBusMessage m = QDBusMessage::createMethodCall("org.razer", "/org/razer", "razer.devices", "syncEffects");
        
        // Set arguments
        QList<QVariant> args;
        yes ? args.append("True") : args.append("False");
        m.setArguments(args);
        
        bool queued = QDBusConnection::sessionBus().send(m);
        std::cout << "Queued: " << queued << std::endl;
        return queued;
    }
    
    QString getDriverVersion() {
        QDBusMessage m = QDBusMessage::createMethodCall("org.razer",
                                                    "/org/razer",
                                                    "razer.daemon",
                                                    "version");
        QDBusMessage response = QDBusConnection::sessionBus().call(m);
        return response.arguments()[0].toString();
    }
    
    Device::Device(QString s) {
        serial = s;
    }
    
    QString Device::getDeviceName() {
        QDBusMessage m = QDBusMessage::createMethodCall("org.razer", "/org/razer/device/" + serial, "razer.device.misc", "getDeviceName");
        QDBusMessage response = QDBusConnection::sessionBus().call(m);
        return response.arguments()[0].toString();
    }
        
    Device::~Device() {
            
    }
    
}

int main() {
    //std::cout << "Hello, world!" << std::endl;
    //qDebug() << "hello";
    QStringList serialnrs = razermethods::getConnectedDevices();
    foreach (const QString &str, serialnrs) {
        std::cout << str.toStdString() << std::endl;
        razermethods::Device device = razermethods::Device(str);
        std::cout << device.getDeviceName().toStdString() << std::endl;
    }
    
    std::cout << "Driver version: " << razermethods::getDriverVersion().toStdString() << std::endl;
}

