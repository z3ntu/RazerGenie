/*
 * Copyright (C) 2016-2017  Luca Weiss <luca (at) z3ntu (dot) xyz>
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

#include <QDomDocument>
#include <QDBusMessage>
#include "razercapability.h"

namespace razermethods
{
QStringList getConnectedDevices();
QString getDaemonVersion();
bool syncDevices(bool yes);
bool stopDaemon();
/* Helper methods */
QString QDBusMessageToString(const QDBusMessage &message);
QStringList QDBusMessageToStringList(const QDBusMessage &message);
QDomDocument QDBusMessageToXML(const QDBusMessage &message);
bool QDBusMessageToVoid(const QDBusMessage& message);
QDBusMessage prepareGeneralQDBusMessage(const QString &interface, const QString &method);

// razer-drivers name to picture url on http://developer.razerzone.com/chroma/compatible-devices/
const static QHash<QString, QString> urlLookup { {"Razer Kraken 7.1 (Rainie)", "krakenchroma"}, {"Razer DeathAdder Chroma", "dachroma"} };
//const static QHash<QString, RazerCapability> capabilityLookup { {"lighting_breath_single", RazerCapability(1)} };
const static QList<RazerCapability> capabilites {
    RazerCapability("lighting_breath_single", "Breath Single", 1),
    RazerCapability("lighting_breath_dual", "Breath Dual", 2),
    RazerCapability("lighting_breath_triple", "Breath Triple", 3),
    RazerCapability("lighting_breath_random", "Breath Random", 0),
    RazerCapability("lighting_wave", "Wave", 0), //TODO Check
    RazerCapability("lighting_reactive", "Reactive", 0), //TODO Check
    RazerCapability("lighting_none", "None", 0),
    RazerCapability("lighting_spectrum", "Spectrum", 0),
    RazerCapability("lighting_static", "Static", 1)
};

class Device
{
private:
    QString serial;
    QStringList introspection;
    QHash<QString, bool> capabilites;

    QDBusMessage prepareDeviceQDBusMessage(const QString &interface, const QString &method);
    void Introspect();
    void setupCapabilities();

    bool hasCapabilityInternal(const QString &interface, const QString &method = QString());
public:
    Device(QString serial);
    ~Device();

    QString getDeviceName();
    QString getDeviceType();
    QString getFirmwareVersion();
    QString getPngFilename();
    bool hasCapability(const QString &name);
    QHash<QString, bool> getAllCapabilities();
    bool setLogoStatic(int r, int g, int b);

    enum lightingLocations {
        lighting, lighting_logo, lighting_scroll
    };
};

}

#endif // RAZERMETHODS_H
