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

#ifndef RAZERMETHODS_H
#define RAZERMETHODS_H

namespace razermethods
{
    QStringList getConnectedDevices();
    QString getDriverVersion();
    bool syncDevices(bool yes);
    /* Helper methods */
    QString QDBusMessageToString(const QDBusMessage &message);
    QStringList QDBusMessageToStringList(const QDBusMessage &message);
    QDBusMessage prepareGeneralQDBusMessage(const QString &interface, const QString &method);
    
    class Device {
    private:
        QString serial;
        QDBusMessage prepareDeviceQDBusMessage(const QString &interface, const QString &method);
    public:
        Device(QString serial);
        ~Device();
        QString getDeviceName();
        QString getDeviceType();
    };
    
}

#endif // RAZERMETHODS_H
