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

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDebug>
#include <QDomDocument>

#include <iostream>

#include "razermethods.h"

namespace razermethods
{
/**
 * Gets a list of connected devices in form of their serial number.
 * Can be used to create a 'Device' object and get further information about the devices.
 */
QStringList getConnectedDevices()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getDevices");
    return QDBusMessageToStringList(m);
}

/**
 * If devices should sync effects.
 * Example: Set it to 'on', set the lighting on one device to something, other devices connected will automatically get set to the same effect.
 */
bool syncDevices(bool yes)
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "syncEffects");

    // Set arguments
    QList<QVariant> args;
    //yes ? args.append("True") : args.append("False"); // maybe bool works here
    args.append(yes);
    m.setArguments(args);

    bool queued = QDBusConnection::sessionBus().send(m);
    std::cout << "Queued: " << queued << std::endl;
    return queued;
}

/**
 * Returns the daemon version currently running.
 */
QString getDaemonVersion()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.daemon", "version");
    return QDBusMessageToString(m);
}

/**
 * Stops the daemon. WARNING: FURTHER COMMUNICATION WILL NOT BE POSSIBLE.
 */
bool stopDaemon()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.daemon", "stop");
    return QDBusMessageToVoid(m);
}

/**
 * Sets if the Chroma lighting should turn off if the screensaver is turned on.
 */
bool setTurnOnScreensaver(bool turnOffOnScreensaver)
{
    QDBusMessage m;
    m = prepareGeneralQDBusMessage("razer.devices", "enableTurnOffOnScreensaver");
    QList<QVariant> args;
    //yes ? args.append("True") : args.append("False"); // maybe bool works here
    args.append(turnOffOnScreensaver);
    m.setArguments(args);

    return QDBusMessageToVoid(m);
}

/* Device class methods */
/**
 * Constrcuts a new device object with the given serial.
 */
Device::Device(QString s)
{
    serial = s;
    Introspect();
    setupCapabilities();
}

/**
 * Returns a human readable device name like "Razer DeathAdder Chroma" or "Razer Kraken 7.1 (Rainie)".
 */
QString Device::getDeviceName()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceName");
    return QDBusMessageToString(m);
}

/**
 * Returns the type of the device. Could be one of 'headset', 'mouse', 'mug', 'keyboard', 'tartarus' or another type, if added to the daemon.
 */
QString Device::getDeviceType()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceType");
    return QDBusMessageToString(m);
}

/**
 * Returns the firmware version of the device.
 */
QString Device::getFirmwareVersion()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getFirmware");
    return QDBusMessageToString(m);
}

/**
 * Returns the .png filename for pictures from http://developer.razerzone.com/chroma/compatible-devices/.
 * Could return an empty string (qstring.isEmpty()) if no picture was found.
 */
QString Device::getPngFilename()
{
    return urlLookup.value(getDeviceName()) + ".png";
}

/**
 * Sets the logo to static lighting.
 */
bool Device::setLogoStatic(int r, int g, int b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoStatic");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Fill "introspection" variable with data from the dbus introspection xml
 */
void Device::Introspect()
{
    QStringList intr;

    QDBusMessage m = prepareDeviceQDBusMessage("org.freedesktop.DBus.Introspectable", "Introspect");
    QDomDocument doc = QDBusMessageToXML(m);

    QDomNodeList nodes = doc.documentElement().childNodes();
    for(int i = 0; i<nodes.count(); i++) {
        // Check if "interface" and also not org.freedesktop.DBus.Introspectable
        QDomElement element = nodes.at(i).toElement();
        QString interfacename = element.attributeNode("name").value();

        QDomNodeList methodnodes = element.childNodes();
        for(int ii = 0; ii<methodnodes.count(); ii++) {
            QDomElement methodelement = methodnodes.at(ii).toElement();
            intr.append(interfacename + ";" + methodelement.attributeNode("name").value());
        }
        intr.append(interfacename);
    }
    introspection = intr;
}

/**
 * Fill "capabilities" list with the capabilites of the device. Names are from the pylib, parsed with the script ./capabilities_to_cpp.sh in the root of this repo.
 */
void Device::setupCapabilities()
{
    capabilites.insert("name", true);
    capabilites.insert("type", true);
    capabilites.insert("firmware_version", true);
    capabilites.insert("serial", true);
    capabilites.insert("brightness", hasCapabilityInternal("razer.device.lighting.brightness"));

    capabilites.insert("macro_logic", hasCapabilityInternal("razer.device.macro"));

    // Default device is a chroma so lighting capabilities
    capabilites.insert("lighting", hasCapabilityInternal("razer.device.lighting.chroma"));
    capabilites.insert("lighting_breath_single", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathSingle"));
    capabilites.insert("lighting_breath_dual", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathDual"));
    capabilites.insert("lighting_breath_triple", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathTriple"));
    capabilites.insert("lighting_breath_random", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathRandom"));
    capabilites.insert("lighting_wave", hasCapabilityInternal("razer.device.lighting.chroma", "setWave"));
    capabilites.insert("lighting_reactive", hasCapabilityInternal("razer.device.lighting.chroma", "setReactive"));
    capabilites.insert("lighting_none", hasCapabilityInternal("razer.device.lighting.chroma", "setNone"));
    capabilites.insert("lighting_spectrum", hasCapabilityInternal("razer.device.lighting.chroma", "setSpectrum"));
    capabilites.insert("lighting_static", hasCapabilityInternal("razer.device.lighting.chroma", "setStatic"));
    capabilites.insert("lighting_ripple", hasCapabilityInternal("razer.device.lighting.custom", "setRipple")); // Thinking of extending custom to do more hence the key check
    capabilites.insert("lighting_ripple_random", hasCapabilityInternal("razer.device.lighting.custom", "setRippleRandomColour"));

    capabilites.insert("lighting_pulsate", hasCapabilityInternal("razer.device.lighting.chroma", "setPulsate"));

    // Get if the device has an LED Matrix, == True as its a DBus boolean otherwise, so for consistency sake we coerce it into a native bool
    // FIXME: 'lighting_led_matrix': self._dbus_interfaces['device'].hasMatrix() == True,
    capabilites.insert("lighting_led_single", hasCapabilityInternal("razer.device.lighting.chroma", "setKey"));

    // Mouse lighting attrs
    capabilites.insert("lighting_logo", hasCapabilityInternal("razer.device.lighting.logo"));
    capabilites.insert("lighting_logo_blinking", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBlinking"));
    capabilites.insert("lighting_logo_brightness", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBrightness"));
    capabilites.insert("lighting_logo_pulsate", hasCapabilityInternal("razer.device.lighting.logo", "setLogoPulsate"));
    capabilites.insert("lighting_logo_spectrum", hasCapabilityInternal("razer.device.lighting.logo", "setLogoSpectrum"));
    capabilites.insert("lighting_logo_static", hasCapabilityInternal("razer.device.lighting.logo", "setLogoStatic"));
    capabilites.insert("lighting_logo_none", hasCapabilityInternal("razer.device.lighting.logo", "setLogoNone"));
    capabilites.insert("lighting_logo_reactive", hasCapabilityInternal("razer.device.lighting.logo", "setLogoReactive"));
    capabilites.insert("lighting_logo_breath_single", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathSingle"));
    capabilites.insert("lighting_logo_breath_dual", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathDual"));
    capabilites.insert("lighting_logo_breath_random", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathRandom"));

    capabilites.insert("lighting_scroll", hasCapabilityInternal("razer.device.lighting.scroll"));
    capabilites.insert("lighting_scroll_blinking", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBlinking"));
    capabilites.insert("lighting_scroll_brightness", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBrightness"));
    capabilites.insert("lighting_scroll_pulsate", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollPulsate"));
    capabilites.insert("lighting_scroll_spectrum", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollSpectrum"));
    capabilites.insert("lighting_scroll_static", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollStatic"));
    capabilites.insert("lighting_scroll_none", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollNone"));
    capabilites.insert("lighting_scroll_reactive", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollReactive"));
    capabilites.insert("lighting_scroll_breath_single", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathSingle"));
    capabilites.insert("lighting_scroll_breath_dual", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathDual"));
    capabilites.insert("lighting_scroll_breath_random", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathRandom"));
}

/**
 * Returns if a device has a given capability.
 */
bool Device::hasCapability(const QString &name)
{
    return capabilites.value(name);
}

/**
 * Internal method to determine whether a device has a given capability based on interface and method names.
 */
bool Device::hasCapabilityInternal(const QString &interface, const QString &method)
{
    if(method.isNull()) {
        return introspection.contains(interface);
    }
    return introspection.contains(interface + ";" + method);
}

/**
 * Returns a QHash object with all capabilities.
 */
QHash<QString, bool> Device::getAllCapabilities()
{
    return capabilites;
}

//TODO Signal for deviceAdded and deviceRemoved
// QDBusConnection::sessionBus().connect("org.gnome.SessionManager", "/org/gnome/SessionManager/Presence", "org.gnome.SessionManager.Presence" ,"StatusChanged", this, SLOT(MySlot(uint)));

/**
 * Destructor
 */
Device::~Device()
{
}

/**
 * Returns a QDBusMessage object for the given device ("org/razer/serial").
 */
QDBusMessage Device::prepareDeviceQDBusMessage(const QString &interface, const QString &method)
{
    return QDBusMessage::createMethodCall("org.razer", "/org/razer/device/" + serial, interface, method);
}

/**
 * Returns a QDBusMessage object for general daemon use ("/org/razer").
 */
QDBusMessage prepareGeneralQDBusMessage(const QString &interface, const QString &method)
{
    return QDBusMessage::createMethodCall("org.razer", "/org/razer", interface, method);
}

/**
 * Sends a QDBusMessage and returns the string value.
 */
QString QDBusMessageToString(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].toString();
    }
    // TODO: Handle error
    return "error";
}

/**
 * Sends a QDBusMessage and returns the stringlist value.
 */
QStringList QDBusMessageToStringList(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        return msg.arguments()[0].toStringList();
    }
    // TODO: Handle errror
    return msg.arguments()[0].toStringList();
}

/**
 * Sends a QDBusMessage and returns the xml value.
 */
QDomDocument QDBusMessageToXML(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    QDomDocument doc;
    if(msg.type() == QDBusMessage::ReplyMessage) {
        doc.setContent(msg.arguments()[0].toString());
    }
    // TODO: Handle errror
    return doc;
}

/**
 * Sends a QDBusMessage and returns if the call was successful.
 */
bool QDBusMessageToVoid(const QDBusMessage &message)
{
    return QDBusConnection::sessionBus().send(message);
    // TODO: Handle errror
}
}

// Main method for testing / playing.
int main()
{
    std::cout << "Daemon version: " << razermethods::getDaemonVersion().toStdString() << std::endl;
    QStringList serialnrs = razermethods::getConnectedDevices();
    razermethods::syncDevices(false);
    foreach (const QString &str, serialnrs) {
        std::cout << "-----------------" << std::endl;
//         std::cout << "Serial: " << str.toStdString() << std::endl;
        razermethods::Device device = razermethods::Device(str);
        device.setLogoStatic(0, 255, 0);
//         std::cout << "Name:   " << device.getDeviceName().toStdString() << std::endl;
//         std::cout << "Type:   " << device.getDeviceType().toStdString() << std::endl;
//         //bool logostatic = device.hasCapabilityInternal("razer.device.lighting.logo", "setLogoStatic");
//         //std::cout << "Can logo static: " << logostatic << std::endl;
//         QHash<QString, bool> hash = device.getAllCapabilities();
//         for (QHash<QString, bool>::iterator i = hash.begin(); i != hash.end(); ++i)
//             std::cout << i.key().toStdString() << ": " << i.value() << std::endl;
    }
}

