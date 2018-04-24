/*
 * Copyright (C) 2016-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
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
#include <QFileInfo>
#include <QDBusArgument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QVariantHash>
#include <QtGui/qcolor.h>

#include <iostream>

#include "libopenrazer.h"

/*!
    \namespace libopenrazer
    \inmodule libopenrazer

    \brief Contains methods, enums and classes to interface with the OpenRazer daemon via the D-Bus interface.
*/
namespace libopenrazer
{

// ----- MISC METHODS FOR LIBOPENRAZER -----

/**
 * Returns a QDBusMessage object for the given device ("org/razer/serial").
 */
QDBusMessage Device::prepareDeviceQDBusMessage(const QString &interface, const QString &method)
{
    return QDBusMessage::createMethodCall("org.razer", "/org/razer/device/" + mSerial, interface, method);
}

/**
 * Returns a QDBusMessage object for general daemon use ("/org/razer").
 */
QDBusMessage prepareGeneralQDBusMessage(const QString &interface, const QString &method)
{
    return QDBusMessage::createMethodCall("org.razer", "/org/razer", interface, method);
}

/**
 * Prints out relevant error information about a failed DBus call.
 */
void printError(QDBusMessage& message, const char *functionname)
{
    qWarning() << "libopenrazer: There was an error in" << functionname << "!";
    qWarning() << "libopenrazer:" << message.errorName();
    qWarning() << "libopenrazer:" << message.errorMessage();
}

/**
 * Sends a QDBusMessage and returns the boolean value.
 */
bool QDBusMessageToBool(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].toBool();
    }
    // TODO: Handle error
    printError(msg, Q_FUNC_INFO);
    return false;
}

/**
 * Sends a QDBusMessage and returns the integer value.
 */
int QDBusMessageToInt(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].toInt();
    }
    // TODO: Handle error
    printError(msg, Q_FUNC_INFO);
    return false;
}

/**
 * Sends a QDBusMessage and returns the double value.
 */
double QDBusMessageToDouble(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].toDouble();
    }
    // TODO: Handle error
    printError(msg, Q_FUNC_INFO);
    return false;
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
    printError(msg, Q_FUNC_INFO);
    return "error";
}

/**
 * Sends a QDBusMessage and returns the string value.
 */
uchar QDBusMessageToByte(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].value<uchar>();
    }
    // TODO: Handle error
    printError(msg, Q_FUNC_INFO);
    return 0x00;
}

/**
 * Sends a QDBusMessage and returns the stringlist value.
 */
QStringList QDBusMessageToStringList(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        return msg.arguments()[0].toStringList();// VID / PID
    }
    // TODO: Handle error
    printError(msg, Q_FUNC_INFO);
    return msg.arguments()[0].toStringList();
}

/**
 * Sends a QDBusMessage and returns the int array value.
 */
QList<int> QDBusMessageToIntArray(const QDBusMessage &message)
{
    QList<int> *retList = new QList<int>();
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
//         qDebug() << "reply :" << msg; // sth like QDBusMessage(type=MethodReturn, service=":1.1482", signature="ai", contents=([Argument: ai {5426, 67}]) )
//         qDebug() << "reply arguments : " << msg.arguments();
//         qDebug() << "reply[0] :" << msg.arguments().at(0);
        // ugh
        const QDBusArgument myArg = msg.arguments().at(0).value<QDBusArgument>();
        myArg.beginArray();
        while (!myArg.atEnd()) {
            int myElement = qdbus_cast<int>(myArg);
            retList->append(myElement);
        }
        myArg.endArray();
        return *retList;
    }
    // TODO: Handle error
    printError(msg, Q_FUNC_INFO);
    return *retList;
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
        return doc;
    }
    // TODO: Handle error
    printError(msg, Q_FUNC_INFO);
    return doc;
}

/**
 * Sends a QDBusMessage and returns if the call was successful.
 */
bool QDBusMessageToVoid(const QDBusMessage &message)
{
    return QDBusConnection::sessionBus().send(message);
    // TODO: Handle error ?
}

/*!
 * \fn bool libopenrazer::isDaemonRunning()
 *
 * Returns if the daemon is running (and responding to the version call).
 */
bool isDaemonRunning()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.daemon", "version");
    QDBusMessage msg = QDBusConnection::sessionBus().call(m);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        return true;
    } else {
        return false;
    }
}

/*!
 * \fn QVariantHash libopenrazer::getSupportedDevices()
 *
 * Returns a list of supported devices in the format of \c {QHash<QString(DeviceName), QList<double(VID), double(PID)>>}.
 *
 * \sa Device::getVid(), Device::getPid()
 */
QVariantHash getSupportedDevices()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "supportedDevices");
    QString ret = QDBusMessageToString(m);
    return QJsonDocument::fromJson(ret.toUtf8()).object().toVariantHash();
}

/*!
 * \fn QStringList libopenrazer::getConnectedDevices()
 *
 * Returns a list of connected devices in form of their serial number (e.g. \c ['XX0000000001', 'PM1439131641838']).
 *
 * Can be used to create a libopenrazer::Device object and get further information about the device.
 */
QStringList getConnectedDevices()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getDevices");
    return QDBusMessageToStringList(m);
}

/*!
 * \fn bool libopenrazer::syncEffects(bool yes)
 *
 * If devices should sync effects, as specified by \a yes.
 *
 * Example: Set it to \c 'on', set the lighting on one device to something, other devices connected will automatically get set to the same effect.
 *
 * Returns if the D-Bus call was successful.
 *
 * \sa getSyncEffects()
 */
bool syncEffects(bool yes)
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "syncEffects");
    QList<QVariant> args;
    args.append(yes);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::getSyncEffects()
 *
 * Returns if devices should sync effect.
 *
 * \sa syncEffects()
 */
bool getSyncEffects()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getSyncEffects");
    return QDBusMessageToBool(m);
}

/*!
 * \fn QString libopenrazer::getDaemonVersion()
 *
 * Returns the daemon version currently running (e.g. \c '2.3.0').
 */
QString getDaemonVersion()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.daemon", "version");
    return QDBusMessageToString(m);
}

/*!
 * \fn bool libopenrazer::stopDaemon()
 *
 * Stops the OpenRazer daemon. \b WARNING: FURTHER COMMUNICATION WILL NOT BE POSSIBLE.
 *
 * Returns if the D-Bus call was successful.
 */
bool stopDaemon()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.daemon", "stop");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::setTurnOffOnScreensaver(bool turnOffOnScreensaver)
 *
 * Sets if the LEDs should turn off if the screensaver is turned on, as specified by \a turnOffOnScreensaver.
 *
 * Returns if the D-Bus call was successful.
 *
 * \sa getTurnOffOnScreensaver()
 */
bool setTurnOffOnScreensaver(bool turnOffOnScreensaver)
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "enableTurnOffOnScreensaver");
    QList<QVariant> args;
    args.append(turnOffOnScreensaver);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::getTurnOffOnScreensaver()
 *
 * Returns if the LEDs should turn off if the screensaver is turned on.
 *
 * \sa setTurnOffOnScreensaver()
 */
bool getTurnOffOnScreensaver()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getOffOnScreensaver");
    return QDBusMessageToBool(m);
}

/*!
 * \fn DaemonStatus libopenrazer::getDaemonStatus()
 *
 * Returns status of the daemon, see DaemonStatus.
 */
DaemonStatus getDaemonStatus()
{
    // Scenarios to handle:
    // - Command systemctl doesn't exist (e.g. Alpine or Gentoo) - exit code 255
    // - Unit wasn't found (i.e. daemon is not installed - or only an old version) - exit code 1
    // Daemon can be not installed but enabled -.-
    QProcess process;
    process.start("systemctl", QStringList() << "--user" << "is-enabled" << "openrazer-daemon.service");
    process.waitForFinished();
    QString output(process.readAllStandardOutput());
    QString error(process.readAllStandardError());
    if(output == "enabled\n") return DaemonStatus::Enabled;
    else if(output == "disabled\n") return DaemonStatus::Disabled;
    else if(error == "Failed to get unit file state for openrazer-daemon.service: No such file or directory\n") return DaemonStatus::NotInstalled;
    else if(process.error() == QProcess::FailedToStart) { // check if systemctl could be started - fails on non-systemd distros and flatpak
        QFileInfo daemonFile("/usr/bin/openrazer-daemon");
        // if the daemon executable does not exist, show the not_installed message - probably flatpak
        if(!daemonFile.exists()) return DaemonStatus::NotInstalled;
        // otherwise show the NoSystemd message - probably a non-systemd distro
        return DaemonStatus::NoSystemd;
    } else {
        qWarning() << "libopenrazer: There was an error checking if the daemon is enabled. Unit state is: " << output << ". Error message:" << error;
        return DaemonStatus::Unknown;
    }
}

/*!
 * \fn QString libopenrazer::getDaemonStatusOutput()
 *
 * Returns the multiline output of \c {"systemctl --user status openrazer-daemon.service"}.
 */
QString getDaemonStatusOutput()
{
    QProcess process;
    process.start("systemctl", QStringList() << "--user" << "status" << "openrazer-daemon.service");
    process.waitForFinished();
    QString output(process.readAllStandardOutput());
    QString error(process.readAllStandardError());
    // TODO Handle systemctl not found
    // TODO Check if output and error and only display what's filled (to get rid of stray newline)
    return output + "\n" + error;
}

/*!
 * \fn bool libopenrazer::enableDaemon()
 *
 * Enables the systemd unit for the OpenRazer daemon to auto-start when the user logs in. Runs \c {"systemctl --user enable openrazer-daemon.service"}
 *
 * Returns if the call was successful.
 */
bool enableDaemon()
{
    QProcess process;
    process.start("systemctl", QStringList() << "--user" << "enable" << "openrazer-daemon.service");
    process.waitForFinished();
    return process.exitCode() == 0;
}

// ====== DEVICE CLASS ======

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
 * Fill "capabilities" list with the capabilities of the device. Names are from the pylib, parsed with the script ./scripts/capabilities_to_cpp.sh in the root of this repo.
 */
void Device::setupCapabilities()
{
    capabilities.insert("name", true);
    capabilities.insert("type", true);
    capabilities.insert("firmware_version", true);
    capabilities.insert("serial", true);
    capabilities.insert("dpi", hasCapabilityInternal("razer.device.dpi", "setDPI"));
    capabilities.insert("brightness", hasCapabilityInternal("razer.device.lighting.brightness"));
    capabilities.insert("get_brightness", hasCapabilityInternal("razer.device.lighting.brightness", "setBrightness"));
    capabilities.insert("battery", hasCapabilityInternal("razer.device.power"));
    capabilities.insert("poll_rate", hasCapabilityInternal("razer.device.misc", "setPollRate"));
    capabilities.insert("mug", hasCapabilityInternal("razer.device.misc.mug", "isMugPresent"));
    capabilities.insert("backlight", hasCapabilityInternal("razer.device.lighting.backlight", "getBacklightActive"));
    capabilities.insert("kbd_layout", hasCapabilityInternal("razer.device.misc", "getKeyboardLayout"));

    capabilities.insert("macro_logic", hasCapabilityInternal("razer.device.macro"));

    // Default device is a chroma so lighting capabilities
    capabilities.insert("lighting", hasCapabilityInternal("razer.device.lighting.chroma"));
    capabilities.insert("lighting_breath_single", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathSingle"));
    capabilities.insert("lighting_breath_dual", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathDual"));
    capabilities.insert("lighting_breath_triple", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathTriple"));
    capabilities.insert("lighting_breath_random", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathRandom"));
    capabilities.insert("lighting_charging", hasCapabilityInternal("razer.device.lighting.power"));
    capabilities.insert("lighting_wave", hasCapabilityInternal("razer.device.lighting.chroma", "setWave"));
    capabilities.insert("lighting_reactive", hasCapabilityInternal("razer.device.lighting.chroma", "setReactive"));
    capabilities.insert("lighting_none", hasCapabilityInternal("razer.device.lighting.chroma", "setNone"));
    capabilities.insert("lighting_spectrum", hasCapabilityInternal("razer.device.lighting.chroma", "setSpectrum"));
    capabilities.insert("lighting_static", hasCapabilityInternal("razer.device.lighting.chroma", "setStatic"));

    capabilities.insert("lighting_starlight_single", hasCapabilityInternal("razer.device.lighting.chroma", "setStarlightSingle"));
    capabilities.insert("lighting_starlight_dual", hasCapabilityInternal("razer.device.lighting.chroma", "setStarlightDual"));
    capabilities.insert("lighting_starlight_random", hasCapabilityInternal("razer.device.lighting.chroma", "setStarlightRandom"));

    capabilities.insert("lighting_ripple", hasCapabilityInternal("razer.device.lighting.custom", "setRipple"));
    capabilities.insert("lighting_ripple_random", hasCapabilityInternal("razer.device.lighting.custom", "setRippleRandomColour"));

    capabilities.insert("lighting_bw2013", hasCapabilityInternal("razer.device.lighting.bw2013"));
    capabilities.insert("lighting_static_bw2013", hasCapabilityInternal("razer.device.lighting.bw2013", "setStatic"));
    capabilities.insert("lighting_pulsate", hasCapabilityInternal("razer.device.lighting.bw2013", "setPulsate"));

    capabilities.insert("lighting_profile_leds", hasCapabilityInternal("razer.device.lighting.profile_led"));

    capabilities.insert("lighting_led_matrix", hasMatrix());
    capabilities.insert("lighting_led_single", hasCapabilityInternal("razer.device.lighting.chroma", "setKey"));

    // Mouse lighting attrs
    capabilities.insert("lighting_logo", hasCapabilityInternal("razer.device.lighting.logo"));
    capabilities.insert("lighting_logo_active", hasCapabilityInternal("razer.device.lighting.logo", "setLogoActive"));
    capabilities.insert("lighting_logo_blinking", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBlinking"));
    capabilities.insert("lighting_logo_brightness", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBrightness"));
    capabilities.insert("get_lighting_logo_brightness", hasCapabilityInternal("razer.device.lighting.logo", "getLogoBrightness"));
    capabilities.insert("lighting_logo_pulsate", hasCapabilityInternal("razer.device.lighting.logo", "setLogoPulsate"));
    capabilities.insert("lighting_logo_spectrum", hasCapabilityInternal("razer.device.lighting.logo", "setLogoSpectrum"));
    capabilities.insert("lighting_logo_static", hasCapabilityInternal("razer.device.lighting.logo", "setLogoStatic"));
    capabilities.insert("lighting_logo_none", hasCapabilityInternal("razer.device.lighting.logo", "setLogoNone"));
    capabilities.insert("lighting_logo_reactive", hasCapabilityInternal("razer.device.lighting.logo", "setLogoReactive"));
    capabilities.insert("lighting_logo_breath_single", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathSingle"));
    capabilities.insert("lighting_logo_breath_dual", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathDual"));
    capabilities.insert("lighting_logo_breath_random", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathRandom"));

    capabilities.insert("lighting_scroll", hasCapabilityInternal("razer.device.lighting.scroll"));
    capabilities.insert("lighting_scroll_active", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollActive"));
    capabilities.insert("lighting_scroll_blinking", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBlinking"));
    capabilities.insert("lighting_scroll_brightness", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBrightness"));
    capabilities.insert("get_lighting_scroll_brightness", hasCapabilityInternal("razer.device.lighting.scroll", "getScrollBrightness"));
    capabilities.insert("lighting_scroll_pulsate", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollPulsate"));
    capabilities.insert("lighting_scroll_spectrum", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollSpectrum"));
    capabilities.insert("lighting_scroll_static", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollStatic"));
    capabilities.insert("lighting_scroll_none", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollNone"));
    capabilities.insert("lighting_scroll_reactive", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollReactive"));
    capabilities.insert("lighting_scroll_breath_single", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathSingle"));
    capabilities.insert("lighting_scroll_breath_dual", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathDual"));
    capabilities.insert("lighting_scroll_breath_random", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathRandom"));

    capabilities.insert("lighting_backlight", hasCapabilityInternal("razer.device.lighting.backlight"));
    capabilities.insert("lighting_backlight_active", hasCapabilityInternal("razer.device.lighting.backlight", "setBacklightActive"));
    capabilities.insert("get_lighting_backlight_effect", hasCapabilityInternal("razer.device.lighting.backlight", "getBacklightEffect"));
    capabilities.insert("lighting_backlight_brightness", hasCapabilityInternal("razer.device.lighting.backlight", "setBacklightBrightness"));
    capabilities.insert("get_lighting_backlight_brightness", hasCapabilityInternal("razer.device.lighting.backlight", "getBacklightBrightness"));
    capabilities.insert("lighting_backlight_spectrum", hasCapabilityInternal("razer.device.lighting.backlight", "setBacklightSpectrum"));
    capabilities.insert("lighting_backlight_static", hasCapabilityInternal("razer.device.lighting.backlight", "setBacklightStatic"));
}

/*!
 * \fn libopenrazer::Device::Device(QString serial)
 *
 * Constructs a new device object with the given \a serial.
 */
Device::Device(QString s)
{
    mSerial = s;
    Introspect();
    setupCapabilities();
}

/*
 * Destructor
 */
Device::~Device()
{
}

/*!
 * \fn bool libopenrazer::connectDeviceAdded(QObject *receiver, const char *slot)
 *
 * Connects the \c device_added signal of the daemon to the specified method using the \a receiver and \a slot.
 *
 * Can be used in the Qt4-style Signal&Slot syntax, e.g.:
 * \code
 * libopenrazer::connectDeviceAdded(this, SLOT(deviceAdded()));
 * \endcode
 *
 * Returns if the connection was successful.
 *
 * \sa connectDeviceRemoved()
 */
// TODO New Qt5 connect style syntax - maybe https://stackoverflow.com/a/35501065/3527128
bool connectDeviceAdded(QObject *receiver, const char *slot)
{
    return QDBusConnection::sessionBus().connect("org.razer", "/org/razer", "razer.devices", "device_added", receiver, slot);
}

/*!
 * \fn bool libopenrazer::connectDeviceRemoved(QObject *receiver, const char *slot)
 *
 * Connects the \c device_removed signal of the daemon to the specified method using the \a receiver and \a slot.
 *
 * Can be used in the Qt4-style Signal&Slot syntax, e.g.:
 * \code
 * libopenrazer::connectDeviceRemoved(this, SLOT(deviceRemoved()));
 * \endcode
 *
 * Returns if the connection was successful.
 *
 * \sa connectDeviceAdded()
 */
// TODO New Qt5 connect style syntax - maybe https://stackoverflow.com/a/35501065/3527128
bool connectDeviceRemoved(QObject *receiver, const char *slot)
{
    return QDBusConnection::sessionBus().connect("org.razer", "/org/razer", "razer.devices", "device_removed", receiver, slot);
}

// ---- MISC METHODS ----
/*!
 * \fn QString libopenrazer::Device::serial()
 *
 * Returns the device serial.
 */
QString Device::serial()
{
    return mSerial;
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

/*!
 * \fn bool libopenrazer::Device::hasCapability(const QString &name)
 *
 * Returns if a device has a given capability with the given \a name. Capability strings can be listed with getAllCapabilities() or viewed in \c libopenrazer.cpp.
 *
 * \sa getAllCapabilities()
 */
bool Device::hasCapability(const QString &name)
{
    return capabilities.value(name);
}

/*!
 * \fn QHash<QString, bool> libopenrazer::Device::getAllCapabilities()
 *
 * Returns a \c QHash object with all capabilities, \c QString is the name of the capability and \c bool if the device has that capability.
 *
 * \sa hasCapability()
 */
QHash<QString, bool> Device::getAllCapabilities()
{
    return capabilities;
}

/*!
 * \fn QString libopenrazer::Device::getPngFilename()
 *
 * Returns the filename for the \c top_img picture (e.g. \c razer-naga-hex-gallery-12.png).
 *
 * Could return an empty string (qstring.isEmpty()) if no picture was found.
 *
 * \sa getRazerUrls(), getPngUrl()
 */
QString Device::getPngFilename()
{
    return getRazerUrls().value("top_img").toString().split("/").takeLast();
}

/*!
 * \fn QString libopenrazer::Device::getPngUrl()
 *
 * Returns the URL for the \c top_img picture.
 *
 * Could return an empty string (\c qstring.isEmpty()) if no picture was found.
 *
 * \sa getRazerUrls(), getPngFilename()
 */
QString Device::getPngUrl()
{
    return getRazerUrls().value("top_img").toString();
}


// ----- DBUS METHODS -----

/*!
 * \fn QString libopenrazer::Device::getDeviceMode()
 *
 * Returns the device mode of the device, like '0:0' or '3:0' for normal mode and driver mode respetively.
 *
 * \sa setDeviceMode()
 */
QString Device::getDeviceMode()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceMode");
    return QDBusMessageToString(m);
}

/*!
 * \fn bool libopenrazer::Device::setDeviceMode(uchar mode_id, uchar param)
 *
 * Sets the device mode to the specified \a mode_id and \a param.
 * Allowed values are 0x00 (normal mode) and 0x03 (driver mode) for \a mode_id and 0x00 for param.
 *
 * When the device is set to Driver Mode, tilting the mouse wheel stops working, and the 'Up DPI' and 'Down DPI' buttons don't actually do anything. Razer Synapse on Windows is supposed to change the DPI instead of the mouse.
 *
 * Returns if the D-Bus call was successful.
 *
 * \sa getDeviceMode()
 */
bool Device::setDeviceMode(uchar mode_id, uchar param)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "setDeviceMode");
    QList<QVariant> args;
    args.append(mode_id);
    args.append(param);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn QString libopenrazer::Device::getDeviceName()
 *
 * Returns a human readable device name like \c {"Razer DeathAdder Chroma"} or \c {"Razer Kraken 7.1"}.
 */
QString Device::getDeviceName()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceName");
    return QDBusMessageToString(m);
}

/*!
 * \fn QString libopenrazer::Device::getDeviceType()
 *
 * Returns the type of the device. Could be one of \c 'keyboard', \c 'mouse', \c 'mousemat', \c 'core', \c 'keypad', \c 'headset', \c 'mug' or another type, if added to the daemon.
 */
QString Device::getDeviceType()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceType");
    QString devicetype = QDBusMessageToString(m);
    // Fix up devicetype for old versions of the daemon (PR #445 in openrazer/openrazer).
    // TODO: Remove once the new daemon version was released (and was out for a while).
    if(devicetype == "firefly") {
        devicetype = "mousemat";
    } else if(devicetype == "orbweaver" || devicetype == "tartarus") {
        devicetype = "keypad";
    }
    return devicetype;
}

/*!
 * \fn QString libopenrazer::Device::getDriverVersion()
 *
 * Returns the kernel driver version used by the device (e.g. \c '2.3.0').
 */
QString Device::getDriverVersion()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDriverVersion");
    return QDBusMessageToString(m);
}

/*!
 * \fn QString libopenrazer::Device::getFirmwareVersion()
 *
 * Returns the firmware version of the device (e.g. \c 'v1.0').
 */
QString Device::getFirmwareVersion()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getFirmware");
    return QDBusMessageToString(m);
}

/*!
 * \fn QString libopenrazer::Device::getKeyboardLayout()
 *
 * Returns the physical layout of the keyboard (e.g. \c 'de_DE', \c 'en_US', \c 'en_GB' or \c 'unknown')
 */
QString Device::getKeyboardLayout()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getKeyboardLayout");
    return QDBusMessageToString(m);
}

/*!
 * \fn QVariantHash libopenrazer::Device::getRazerUrls()
 *
 * Returns a QVariantHash (\c {QHash<QString, QVariant>}).
 * Most likely contains keys \c "top_img", \c "side_img", \c "store" and \c "perspective_img".
 * Values are \c QVariant<QString> with a full URL as value.
 */
QVariantHash Device::getRazerUrls()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getRazerUrls");
    QString ret = QDBusMessageToString(m);
    return QJsonDocument::fromJson(ret.toUtf8()).object().toVariantHash();
}

/*!
 * \fn int libopenrazer::Device::getVid()
 *
 * Returns the USB vendor ID as integer in decimal notation. Should always be \c 5426 (-> Hex \c 1532)
 */
int Device::getVid()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getVidPid");
    return QDBusMessageToIntArray(m)[0];
}

/*!
 * \fn int libopenrazer::Device::getPid()
 *
 * Returns USB product ID as integer in decimal notation.
 */
int Device::getPid()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getVidPid");
    return QDBusMessageToIntArray(m)[1];
}

/*!
 * \fn bool libopenrazer::Device::hasDedicatedMacroKeys()
 *
 * Returns if the device has dedicated macro keys.
 */
bool Device::hasDedicatedMacroKeys()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "hasDedicatedMacroKeys");
    return QDBusMessageToBool(m);
}

/*!
 * \fn bool libopenrazer::Device::hasMatrix()
 *
 * Returns if the device has a matrix. Dimensions can be gotten with getMatrixDimensions()
 */
bool Device::hasMatrix()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "hasMatrix");
    return QDBusMessageToBool(m);
}

/*!
 * \fn QList<int> libopenrazer::Device::getMatrixDimensions()
 *
 * Returns the matrix dimensions in the format of \c [6, 22]. If the device has no matrix, it will return \c -1 for both numbers.
 */
QList<int> Device::getMatrixDimensions()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getMatrixDimensions");
    return QDBusMessageToIntArray(m);
}

/*!
 * \fn int libopenrazer::Device::getPollRate()
 *
 * Returns the current poll rate.
 */
int Device::getPollRate()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getPollRate");
    return QDBusMessageToInt(m);
}

/*!
 * \fn bool libopenrazer::Device::setPollRate(PollRate pollrate)
 *
 * Sets the poll rate of the mouse to the specified \a pollrate.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setPollRate(PollRate pollrate)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "setPollRate");
    QList<QVariant> args;
    args.append(pollrate);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setDPI(int dpi_x, int dpi_y)
 *
 * Sets the DPI of the mouse to the specified \a dpi_x for the x-Axis and \a dpi_y for the y-Axis. Maximum value is what is returned by maxDPI().
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setDPI(int dpi_x, int dpi_y)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.dpi", "setDPI");
    QList<QVariant> args;
    args.append(dpi_x);
    args.append(dpi_y);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn QList<int> libopenrazer::Device::getDPI()
 *
 * Returns the DPI of the mouse (e.g. \c [800, 800]).
 */
QList<int> Device::getDPI()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.dpi", "getDPI");
    return QDBusMessageToIntArray(m);
}

/*!
 * \fn int libopenrazer::Device::maxDPI()
 *
 * Returns the maximum DPI possible for the device.
 */
int Device::maxDPI()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.dpi", "maxDPI");
    return QDBusMessageToInt(m);
}

// BATTERY
/*!
 * \fn bool libopenrazer::Device::isCharging()
 *
 * Returns if the device is charging.
 */
bool Device::isCharging()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "isCharging");
    return QDBusMessageToBool(m);
}

/*!
 * \fn double libopenrazer::Device::getBatteryLevel()
 *
 * Returns the battery level between \c 0 and \c 100. Could potentially be \c -1 ???
 */
double Device::getBatteryLevel()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "getBattery");
    return QDBusMessageToDouble(m);
}

/*!
 * \fn bool libopenrazer::Device::setIdleTime(ushort idle_time)
 *
 * Sets the idle time of the device, the time before the mouse goes idle.
 * \a idle_time is the time in seconds, with a maximum of 15 minutes (900 seconds).
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setIdleTime(ushort idle_time)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "setIdleTime");
    QList<QVariant> args;
    args.append(idle_time);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setLowBatteryThreshold(uchar threshold)
 *
 * Sets the low battery threshold to the specified \a threshold in percent which lets the mouse flash due to low battery.
 * The \c threshold is capped at 25% (so 25), recommended value is 10.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLowBatteryThreshold(uchar threshold)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "setLowBatteryThreshold");
    QList<QVariant> args;
    args.append(threshold);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::isMugPresent()
 *
 * Returns if the mug is on the mug holder.
 */
bool Device::isMugPresent()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc.mug", "isMugPresent");
    return QDBusMessageToBool(m);
}

// ------ LIGHTING EFFECTS ------

/*!
 * \fn bool libopenrazer::Device::setStatic(QColor color)
 *
 * Sets the lighting to static lighting in the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setStatic(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setStatic");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setBreathSingle(QColor color)
 *
 * Sets the lighting to the single breath effect with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setBreathSingle(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathSingle");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setBreathDual(QColor color, QColor color2)
 *
 * Sets the lighting to the dual breath effect with the specified \a color and \a color2.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setBreathDual(QColor color, QColor color2)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathDual");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    args.append(color2.red());
    args.append(color2.green());
    args.append(color2.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setBreathTriple(QColor color, QColor color2, QColor color3)
 *
 * Sets the lighting to the triple breath effect with the specified \a color, \a color2 and \a color3.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setBreathTriple(QColor color, QColor color2, QColor color3)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathTriple");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    args.append(color2.red());
    args.append(color2.green());
    args.append(color2.blue());
    args.append(color3.red());
    args.append(color3.green());
    args.append(color3.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setBreathRandom()
 *
 * Sets the lighting wheel to the random breath effect.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setBreathRandom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathRandom");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setReactive(QColor color, ReactiveSpeed speed)
 *
 * Sets the lighting to reactive mode with the specified \a color and \a speed.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setReactive(QColor color, ReactiveSpeed speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setReactive");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setSpectrum()
 *
 * Sets the lighting to spectrum mode.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setSpectrum");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setWave(WaveDirection direction)
 *
 * Sets the lighting effect to wave, in the direction \a direction.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setWave(WaveDirection direction)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setWave");
    QList<QVariant> args;
    args.append(direction);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setNone()
 *
 * Sets the LED to none / off.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setNone()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setNone");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setStarlightSingle(QColor color, StarlightSpeed speed)
 *
 * Sets the lighting to starlight effect with the specified \a color and \a speed.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setStarlightSingle(QColor color, StarlightSpeed speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setStarlightSingle");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setStarlightDual(QColor color, QColor color2, StarlightSpeed speed)
 *
 * Sets the lighting to starlight effect with the specified \a color, \a color2 and \a speed.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setStarlightDual(QColor color, QColor color2, StarlightSpeed speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setStarlightDual");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    args.append(color2.red());
    args.append(color2.green());
    args.append(color2.blue());
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setStarlightRandom(StarlightSpeed speed)
 *
 * Sets the lighting to starlight effect with the specified \a speed.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setStarlightRandom(StarlightSpeed speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setStarlightRandom");
    QList<QVariant> args;
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setStatic_bw2013()
 *
 * Sets the lighting to static (without color - bw2013 version).
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setStatic_bw2013()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.bw2013", "setStatic");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setPulsate()
 *
 * Sets the lighting to pulsate mode.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setPulsate()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.bw2013", "setPulsate");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::getBacklightActive()
 *
 * Returns if the backlight LED is active.
 */
bool Device::getBacklightActive()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "getBacklightActive");
    return QDBusMessageToBool(m);
}

/*!
 * \fn bool libopenrazer::Device::setBacklightActive(bool active)
 *
 * Sets the backlight active i.e. lights on, specified by \a active.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setBacklightActive(bool active)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "setBacklightActive");
    QList<QVariant> args;
    args.append(active);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn uchar libopenrazer::Device::getBacklightEffect()
 *
 * Returns the current effect on the backlight LED. Values are defined in LEDEffect.
 */
uchar Device::getBacklightEffect()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "getBacklightEffect");
    return QDBusMessageToByte(m);
}

/*!
 * \fn bool libopenrazer::Device::setBacklightBrightness(double brightness)
 *
 * Sets the backlight \a brightness (0-100).
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setBacklightBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "setBacklightBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn double libopenrazer::Device::getBacklightBrightness()
 *
 * Returns the current backlight brightness (0-100).
 */
double Device::getBacklightBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "getBacklightBrightness");
    return QDBusMessageToDouble(m);
}

/*!
 * \fn bool libopenrazer::Device::setBacklightStatic(QColor color)
 *
 * Sets the backlight to static lighting with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setBacklightStatic(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "setBacklightStatic");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setBacklightSpectrum()
 *
 * Sets the backlight to the spectrum effect.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setBacklightSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "setBacklightSpectrum");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setCustom()
 *
 * Sets the lighting to custom mode (applies effects set from setKeyRow()).
 *
 * Returns if the D-Bus call was successful.
 *
 * \sa setKeyRow()
 */
bool Device::setCustom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setCustom");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setKeyRow(uchar row, uchar startcol, uchar endcol, QVector<QColor> colors)
 *
 * Sets the lighting of a key row to the specified \a colors.
 * \a row is the row in the matrix, \a startcol the column the \a colors list starts and \a endcol where the list ends.
 * Note, that you have to call setCustom() after setting otherwise the effect won't be displayed (even if you have already called setCustom() before).
 * Currently the driver only accepts whole rows that are sent.
 *
 * Returns if the D-Bus call was successful.
 *
 * \sa setCustom()
 */
bool Device::setKeyRow(uchar row, uchar startcol, uchar endcol, QVector<QColor> colors)
{
    if(colors.count() != (endcol+1)-startcol) {
        qWarning() << "Invalid 'colors' length. startcol:" << startcol << " - endcol:" << endcol << " needs " << (endcol+1)-startcol << " entries in colors!";
        return false;
    }

    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setKeyRow");

    QByteArray parameters;
    parameters[0] = row;
    parameters[1] = startcol;
    parameters[2] = endcol;
    int counter = 3;
    foreach(QColor c, colors) {
        // set the rgb to the parameters[i]
        parameters[counter++] = c.red();
        parameters[counter++] = c.green();
        parameters[counter++] = c.blue();
    }

    QList<QVariant> args;
    args.append(parameters);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setRipple(QColor color, double refresh_rate)
 *
 * Sets the lighting to the ripple effect with the specified \a color and \a refresh_rate.
 * Default refresh rate is \c libopenrazer::RIPPLE_REFRESH_RATE (0.05). 0.01 is faster but uses more CPU.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setRipple(QColor color, double refresh_rate)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.custom", "setRipple");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    args.append(refresh_rate);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setRippleRandomColor(double refresh_rate)
 *
 * Sets the lighting to the random ripple effect with the specified \a refresh_rate.
 * Default refresh rate is \c libopenrazer::RIPPLE_REFRESH_RATE (0.05). 0.01 is faster but uses more CPU.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setRippleRandomColor(double refresh_rate)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.custom", "setRippleRandomColour");
    QList<QVariant> args;
    args.append(refresh_rate);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setBrightness(double brightness)
 *
 * Sets the \a brightness (0-100).
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.brightness", "setBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn double libopenrazer::Device::getBrightness()
 *
 * Returns the current brightness (0-100).
 */
double Device::getBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.brightness", "getBrightness");
    return QDBusMessageToDouble(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoStatic(QColor color)
 *
 * Sets the logo to static lighting in the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoStatic(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoStatic");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoActive(bool active)
 *
 * Sets the logo active i.e. lights on, specified by \a active.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoActive(bool active)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoActive");
    QList<QVariant> args;
    args.append(active);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::getLogoActive()
 *
 * Returns if the logo LED is active.
 */
bool Device::getLogoActive()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "getLogoActive");
    return QDBusMessageToBool(m);
}

/*!
 * \fn uchar libopenrazer::Device::getLogoEffect()
 *
 * Returns the current effect on the logo LED. Values are defined in LEDEffect.
 */
uchar Device::getLogoEffect()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "getLogoEffect");
    return QDBusMessageToByte(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoBlinking(QColor color)
 *
 * Sets the logo to the blinking effect with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoBlinking(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBlinking");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoPulsate(QColor color)
 *
 * Sets the logo to the pulsate effect with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoPulsate(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoPulsate");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoSpectrum()
 *
 * Sets the logo to the spectrum effect.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoSpectrum");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoNone()
 *
 * Sets the logo LED to none / off.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoNone()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoNone");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoReactive(QColor color, ReactiveSpeed speed)
 *
 * Sets the logo to the reactive effect with the specified \a color and \a speed.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoReactive(QColor color, ReactiveSpeed speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoReactive");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoBreathSingle(QColor color)
 *
 * Sets the logo to the single breath effect with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoBreathSingle(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBreathSingle");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoBreathDual(QColor color, QColor color2)
 *
 * Sets the logo to the dual breath effect with the specified \a color and \a color2.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoBreathDual(QColor color, QColor color2)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBreathDual");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    args.append(color2.red());
    args.append(color2.green());
    args.append(color2.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoBreathRandom()
 *
 * Sets the logo to the random breath effect.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoBreathRandom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBreathRandom");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setLogoBrightness(double brightness)
 *
 * Sets the logo \a brightness (0-100).
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setLogoBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn double libopenrazer::Device::getLogoBrightness()
 *
 * Returns the current logo brightness (0-100).
 */
double Device::getLogoBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "getLogoBrightness");
    return QDBusMessageToDouble(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollStatic(QColor color)
 *
 * Sets the scrollwheel to static lighting with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollStatic(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollStatic");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollActive(bool active)
 *
 * Sets the scrollwheel active i.e. lights on, specified by \a active.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollActive(bool active)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollActive");
    QList<QVariant> args;
    args.append(active);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::getScrollActive()
 *
 * Returns if the scroll wheel LED is active.
 */
bool Device::getScrollActive()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "getScrollActive");
    return QDBusMessageToBool(m);
}

/*!
 * \fn uchar libopenrazer::Device::getScrollEffect()
 *
 * Returns the current effect on the scroll wheel LED. Values are defined in LEDEffect.
 */
uchar Device::getScrollEffect()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "getScrollEffect");
    return QDBusMessageToByte(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollBlinking(QColor color)
 *
 * Sets the scroll wheel to the blinking effect with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollBlinking(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBlinking");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollPulsate(QColor color)
 *
 * Sets the scroll wheel to the pulsate effect with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollPulsate(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollPulsate");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollSpectrum()
 *
 * Sets the scroll wheel to the spectrum effect.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollSpectrum");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollNone()
 *
 * Sets the scroll wheel LED to none / off.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollNone()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollNone");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollReactive(QColor color, ReactiveSpeed speed)
 *
 * Sets the scroll to the reactive effect with the specified \a color and \a speed.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollReactive(QColor color, ReactiveSpeed speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollReactive");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollBreathSingle(QColor color)
 *
 * Sets the scroll wheel to the single breath effect with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollBreathSingle(QColor color)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBreathSingle");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollBreathDual(QColor color, QColor color2)
 *
 * Sets the scroll wheel to the dual breath effect with the specified \a color and \a color2.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollBreathDual(QColor color, QColor color2)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBreathSingle");
    QList<QVariant> args;
    args.append(color.red());
    args.append(color.green());
    args.append(color.blue());
    args.append(color2.red());
    args.append(color2.green());
    args.append(color2.blue());
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollBreathRandom()
 *
 * Sets the scroll wheel to the random breath effect.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollBreathRandom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBreathRandom");
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::setScrollBrightness(double brightness)
 *
 * Sets the scroll wheel \a brightness (0-100).
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setScrollBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn double libopenrazer::Device::getScrollBrightness()
 *
 * Returns the current scroll wheel brightness (0-100).
 */
double Device::getScrollBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "getScrollBrightness");
    return QDBusMessageToDouble(m);
}

/*!
 * \fn bool libopenrazer::Device::getBlueLED()
 *
 * Returns if the blue profile LED is on/off.
 */
bool Device::getBlueLED()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "getBlueLED");
    return QDBusMessageToBool(m);
}

/*!
 * \fn bool libopenrazer::Device::setBlueLED(bool on)
 *
 * Sets the blue profile LED to on/off, specified by \a on.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setBlueLED(bool on)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "setBlueLED");
    QList<QVariant> args;
    args.append(on);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::getGreenLED()
 *
 * Returns if the green profile LED is on/off.
 */
bool Device::getGreenLED()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "getGreenLED");
    return QDBusMessageToBool(m);
}

/*!
 * \fn bool libopenrazer::Device::setGreenLED(bool on)
 *
 * Sets the green profile LED to on/off, specified by \a on.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setGreenLED(bool on)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "setGreenLED");
    QList<QVariant> args;
    args.append(on);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::Device::getRedLED()
 *
 * Returns if the red profile LED is on/off.
 */
bool Device::getRedLED()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "getRedLED");
    return QDBusMessageToBool(m);
}

/*!
 * \fn bool libopenrazer::Device::setRedLED(bool on)
 *
 * Sets the red profile LED to on/off, specified by \a on.
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setRedLED(bool on)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "setRedLED");
    QList<QVariant> args;
    args.append(on);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}
}
