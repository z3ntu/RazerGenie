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

namespace razermethods
{
QStringList getConnectedDevices();
QString getDriverVersion();
bool syncDevices(bool yes);
/* Helper methods */
QString QDBusMessageToString(const QDBusMessage &message);
QStringList QDBusMessageToStringList(const QDBusMessage &message);
QDomDocument QDBusMessageToXML(const QDBusMessage &message);
void QDBusMessageToVoid(const QDBusMessage &message);
QDBusMessage prepareGeneralQDBusMessage(const QString &interface, const QString &method);

// razer-drivers name to picture url on http://developer.razerzone.com/chroma/compatible-devices/
const static QHash<QString, QString> urlLookup { {"Razer Kraken 7.1 (Rainie)", "krakenchroma"}, {"Razer DeathAdder Chroma", "dachroma"} };

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
    QString getPngFilename();
    bool hasCapability(const QString &name);
    QHash<QString, bool> getAllCapabilities();
    void setLogoStatic(int r, int g, int b);

    enum lightingLocations {
        lighting, lighting_logo, lighting_scroll
    };
};

}

#endif // RAZERMETHODS_H
