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

/**
 * Returns if the daemon is running (and responding to the version call)
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

/**
 * Returns a list of supported devices in the format of QHash<QString(DeviceName), QList<double(VID), double(PID)>>
 */
QVariantHash getSupportedDevices()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "supportedDevices");
    QString ret = QDBusMessageToString(m);
    return QJsonDocument::fromJson(ret.toUtf8()).object().toVariantHash();
}

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
bool syncEffects(bool yes)
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "syncEffects");

    // Set arguments
    QList<QVariant> args;
    //yes ? args.append("True") : args.append("False"); // maybe bool works here
    args.append(yes);
    m.setArguments(args);

    bool queued = QDBusConnection::sessionBus().send(m);
    qDebug() << "Queued: " << queued;
    return queued;
}

/**
 * Returns the daemon version currently running.
 */
bool getSyncEffects()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getSyncEffects");
    return QDBusMessageToBool(m);
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
bool setTurnOffOnScreensaver(bool turnOffOnScreensaver)
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "enableTurnOffOnScreensaver");
    QList<QVariant> args;
    args.append(turnOffOnScreensaver);
    m.setArguments(args);

    return QDBusMessageToVoid(m);
}

/**
 * Gets if the Chroma lighting should turn off if the screensaver is turned on.
 */
bool getTurnOffOnScreensaver()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getOffOnScreensaver");
    return QDBusMessageToBool(m);
}

/**
 * Returns if the systemd user unit for the daemon is enabled.
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

/**
 * Returns the multiline output of "systemctl --user status openrazer-daemon.service"
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

/**
 * Enables the daemon to auto-start when the user logs in. Runs "systemctl --user enable openrazer-daemon.service"
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

    capabilities.insert("lighting_profile_leds", hasCapabilityInternal("razer.device.lighting.profile_led", "getRedLED"));

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

/**
 * Constructs a new device object with the given serial.
 */
Device::Device(QString s)
{
    mSerial = s;
    Introspect();
    setupCapabilities();
}

/**
 * Destructor
 */
Device::~Device()
{
    //TODO Write
}

/**
 * Connects the device_added signal of the daemon to the specified method.
 * TODO New Qt5 connect style syntax - maybe https://stackoverflow.com/a/35501065/3527128
 */
bool connectDeviceAdded(QObject *receiver, const char *slot)
{
    return QDBusConnection::sessionBus().connect("org.razer", "/org/razer", "razer.devices", "device_added", receiver, slot);
}

/**
 * Connects the device_removed signal of the daemon to the specified method.
 * TODO New Qt5 connect style syntax - maybe https://stackoverflow.com/a/35501065/3527128
 */
bool connectDeviceRemoved(QObject *receiver, const char *slot)
{
    return QDBusConnection::sessionBus().connect("org.razer", "/org/razer", "razer.devices", "device_removed", receiver, slot);
}

// ---- MISC METHODS ----
/**
 * Returns the serial
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

/**
 * Returns if a device has a given capability.
 */
bool Device::hasCapability(const QString &name)
{
    return capabilities.value(name);
}

/**
 * Returns a QHash object with all capabilities.
 */
QHash<QString, bool> Device::getAllCapabilities()
{
    return capabilities;
}

/**
 * Returns the .png filename for pictures.
 * Could return an empty string (qstring.isEmpty()) if no picture was found.
 */
QString Device::getPngFilename()
{
    return getRazerUrls().value("top_img").toString().split("/").takeLast();
}

/**
 * Returns the download url for pictures.
 * Could return an empty string (qstring.isEmpty()) if no picture was found.
 */
QString Device::getPngUrl()
{
    return getRazerUrls().value("top_img").toString();
}


// ----- DBUS METHODS -----

/**
 * Returns a human readable device name like "Razer DeathAdder Chroma" or "Razer Kraken 7.1 (Rainie)".
 */
QString Device::getDeviceName()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceName");
    return QDBusMessageToString(m);
}

/**
 * Returns the type of the device. Could be one of 'keyboard', 'mouse', 'mousemat', 'core', 'keypad', 'headset', 'mug' or another type, if added to the daemon.
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

/**
 * Returns the firmware version of the device.
 */
QString Device::getFirmwareVersion()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getFirmware");
    return QDBusMessageToString(m);
}

/**
 * Returns the physical layout of the keyboard (e.g. 'de_DE', 'en_US', 'en_GB' or 'unknown')
 */
QString Device::getKeyboardLayout()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getKeyboardLayout");
    return QDBusMessageToString(m);
}

/**
 * Returns a QVariantHash (QHash\<QString, QVariant>).
 * Most likely contains keys "top_img", "side_img", "store", "perspective_img".
 * Values are QVariant\<QString> with a full url as value.
 */
QVariantHash Device::getRazerUrls()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getRazerUrls");
    QString ret = QDBusMessageToString(m);
    return QJsonDocument::fromJson(ret.toUtf8()).object().toVariantHash();
}

/**
 * Return usb vendor id as integer in decimal notation. Should always be 5426 (-> Hex 1532)
 */
int Device::getVid()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getVidPid");
    return QDBusMessageToIntArray(m)[0];
}

/**
 * Return usb product id as integer in decimal notation.
 */
int Device::getPid()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getVidPid");
    return QDBusMessageToIntArray(m)[1];
}

/**
 * Returns if the device has dedicated macro keys.
 */
bool Device::hasDedicatedMacroKeys()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "hasDedicatedMacroKeys");
    return QDBusMessageToBool(m);
}

/**
 * Returns if the device has a matrix. Dimensions can be gotten with getMatrixDimensions()
 */
bool Device::hasMatrix()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "hasMatrix");
    return QDBusMessageToBool(m);
}

/**
 * Returns the matrix dimensions. If the device has no matrix, it will return -1 for both numbers.
 */
QList<int> Device::getMatrixDimensions()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getMatrixDimensions");
    return QDBusMessageToIntArray(m);
}

/**
 * Returns the poll rate.
 */
int Device::getPollRate()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getPollRate");
    return QDBusMessageToInt(m);
}

/**
 * Sets the poll rate. Use one of libopenrazer::POLL_125HZ, libopenrazer::POLL_500HZ or libopenrazer::POLL_1000HZ.
 */
bool Device::setPollRate(ushort pollrate)
{
    if(pollrate != POLL_125HZ && pollrate != POLL_500HZ && pollrate != POLL_1000HZ) {
        qWarning() << "libopenrazer: setPollRate(): Has to be one of libopenrazer::POLL_125HZ, libopenrazer::POLL_500HZ or libopenrazer::POLL_1000HZ";
        return false;
    }
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "setPollRate");
    QList<QVariant> args;
    args.append(pollrate);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the DPI.
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

/**
 * Returns the DPI.
 */
QList<int> Device::getDPI()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.dpi", "getDPI");
    return QDBusMessageToIntArray(m);
}

/**
 * Returns the maximum DPI possible for the device.
 */
int Device::maxDPI()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.dpi", "maxDPI");
    return QDBusMessageToInt(m);
}

// BATTERY
/**
 * Returns if the device is charging.
 */
bool Device::isCharging()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "isCharging");
    return QDBusMessageToBool(m);
}

/**
 * Returns the battery level between 0 and 100. Could maybe be -1 ???
 */
double Device::getBatteryLevel()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "getBattery");
    return QDBusMessageToDouble(m);
}

/**
 * Sets the idle time of the device.
 */
bool Device::setIdleTime(ushort idle_time)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "setIdleTime");
    QList<QVariant> args;
    args.append(idle_time);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the low battery threshold.
 */
bool Device::setLowBatteryThreshold(uchar threshold)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "setLowBatteryThreshold");
    QList<QVariant> args;
    args.append(threshold);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Returns if the mug is on the mug holder.
 */
bool Device::isMugPresent()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc.mug", "isMugPresent");
    return QDBusMessageToBool(m);
}

// ------ LIGHTING EFFECTS ------

/**
 * Sets the lighting to static lighting.
 */
bool Device::setStatic(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setStatic");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to random breath lighting.
 */
bool Device::setBreathSingle(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathSingle");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to dual breath lighting.
 */
bool Device::setBreathDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathDual");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(r2);
    args.append(g2);
    args.append(b2);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to triple breath lighting.
 */
bool Device::setBreathTriple(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2, uchar r3, uchar g3, uchar b3)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathTriple");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(r2);
    args.append(g2);
    args.append(b2);
    args.append(r3);
    args.append(g3);
    args.append(b3);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to random breath lighting.
 */
bool Device::setBreathRandom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathRandom");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to reactive mode.
 * Use one of libopenrazer::REACTIVE_* for speed.
 */
bool Device::setReactive(uchar r, uchar g, uchar b, uchar speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setReactive");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to spectrum mode.
 */
bool Device::setSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setSpectrum");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to wave.
 * Use libopenrazer::WAVE_RIGHT or libopenrazer::WAVE_LEFT.
 */
bool Device::setWave(WaveDirection direction)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setWave");
    QList<QVariant> args;
    args.append(direction);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to "off".
 */
bool Device::setNone()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setNone");
    return QDBusMessageToVoid(m);
}

bool Device::setStarlightSingle(uchar r, uchar g, uchar b, uchar speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setStarlightSingle");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setStarlightDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2, uchar speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setStarlightDual");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(r2);
    args.append(g2);
    args.append(b2);
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setStarlightRandom(uchar speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setStarlightRandom");
    QList<QVariant> args;
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to static (without color - bw2013 version).
 */
bool Device::setStatic_bw2013()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.bw2013", "setStatic");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to pulsate.
 */
bool Device::setPulsate()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.bw2013", "setPulsate");
    return QDBusMessageToVoid(m);
}

/**
 * Returns if the backlight is active.
 */
bool Device::getBacklightActive()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "getBacklightActive");
    return QDBusMessageToBool(m);
}

/**
 * Sets the backlight to active.
 */
bool Device::setBacklightActive(bool active)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "setBacklightActive");
    QList<QVariant> args;
    args.append(active);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Returns the current backlight effect.
 */
uchar Device::getBacklightEffect()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "getBacklightEffect");
    return QDBusMessageToByte(m);
}

/**
 * Sets the backlight brightness (0-100).
 */
bool Device::setBacklightBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "setBacklightBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Gets the current backlight brightness (0-100).
 */
double Device::getBacklightBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "getBacklightBrightness");
    return QDBusMessageToDouble(m);
}

/**
 * Sets the backlight to static lighting.
 */
bool Device::setBacklightStatic(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "setBacklightStatic");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the backlight to spectrum.
 */
bool Device::setBacklightSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.backlight", "setBacklightSpectrum");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to custom mode (applies effects set from setKeyRow()).
 */
bool Device::setCustom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setCustom");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting of a key row to the specified colors.
 * Note, that you have to call setCustom() after setting otherwise the effect won't be displayed (even if you have already called setCustom() before).
 * Currently the driver only accepts whole rows that are sent.
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

/**
 * Sets the lighting to ripple.
 */
bool Device::setRipple(uchar r, uchar g, uchar b, double refresh_rate)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.custom", "setRipple");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(refresh_rate);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to random ripple.
 */
bool Device::setRippleRandomColor(double refresh_rate)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.custom", "setRippleRandomColour");
    QList<QVariant> args;
    args.append(refresh_rate);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the brightness (0-100).
 */
bool Device::setBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.brightness", "setBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Gets the current brightness (0-100).
 */
double Device::getBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.brightness", "getBrightness");
    return QDBusMessageToDouble(m);
}

/**
 * Sets the logo to static lighting.
 */
bool Device::setLogoStatic(uchar r, uchar g, uchar b)
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
 * Sets the logo active.
 */
bool Device::setLogoActive(bool active)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoActive");
    QList<QVariant> args;
    args.append(active);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Returns if the logo is active.
 */
bool Device::getLogoActive()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "getLogoActive");
    return QDBusMessageToBool(m);
}

/**
 * Returns the current logo effect.
 */
uchar Device::getLogoEffect()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "getLogoEffect");
    return QDBusMessageToByte(m);
}

/**
 * Sets the logo to blinking.
 */
bool Device::setLogoBlinking(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBlinking");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the logo to pulsate.
 */
bool Device::setLogoPulsate(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoPulsate");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the logo to spectrum.
 */
bool Device::setLogoSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoSpectrum");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the logo to none.
 */
bool Device::setLogoNone()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoNone");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the logo to reactive.
 */
bool Device::setLogoReactive(uchar r, uchar g, uchar b, uchar speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoReactive");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the logo to single breath.
 */
bool Device::setLogoBreathSingle(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBreathSingle");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the logo to dual breath.
 */
bool Device::setLogoBreathDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBreathDual");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(r2);
    args.append(g2);
    args.append(b2);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the logo to random breath.
 */
bool Device::setLogoBreathRandom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBreathRandom");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the logo brightness (0-100).
 */
bool Device::setLogoBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Gets the current logo brightness (0-100).
 */
double Device::getLogoBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "getLogoBrightness");
    return QDBusMessageToDouble(m);
}

/**
 * Sets the scrollwheel to static lighting.
 */
bool Device::setScrollStatic(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollStatic");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the scroll active.
 */
bool Device::setScrollActive(bool active)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollActive");
    QList<QVariant> args;
    args.append(active);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Returns if the scroll if active.
 */
bool Device::getScrollActive()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "getScrollActive");
    return QDBusMessageToBool(m);
}

/**
 * Returns the current scroll effect.
 */
uchar Device::getScrollEffect()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "getScrollEffect");
    return QDBusMessageToByte(m);
}

/**
 * Sets scroll to blinking.
 */
bool Device::setScrollBlinking(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBlinking");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the scroll to pulsate.
 */
bool Device::setScrollPulsate(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollPulsate");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the scroll to spectrum.
 */
bool Device::setScrollSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollSpectrum");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the scroll to none.
 */
bool Device::setScrollNone()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollNone");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the scroll to reactive.
 */
bool Device::setScrollReactive(uchar r, uchar g, uchar b, uchar speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollReactive");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the scroll to single breath.
 */
bool Device::setScrollBreathSingle(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBreathSingle");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the scroll to dual breath.
 */
bool Device::setScrollBreathDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBreathSingle");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(r2);
    args.append(g2);
    args.append(b2);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the scroll to random breath.
 */
bool Device::setScrollBreathRandom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBreathRandom");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the scroll wheel brightness (0-100).
 */
bool Device::setScrollBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Gets the current scroll brightness (0-100).
 */
double Device::getScrollBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "getScrollBrightness");
    return QDBusMessageToDouble(m);
}

/**
 * Gets if the blue profile LED is on/off.
 */
bool Device::getBlueLED()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "getBlueLED");
    return QDBusMessageToBool(m);
}

/**
 * Sets the blue profile LED to on/off.
 */
bool Device::setBlueLED(bool on)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "setBlueLED");
    QList<QVariant> args;
    args.append(on);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Gets if the green profile LED is on/off.
 */
bool Device::getGreenLED()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "getGreenLED");
    return QDBusMessageToBool(m);
}

/**
 * Sets the green profile LED to on/off.
 */
bool Device::setGreenLED(bool on)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "setGreenLED");
    QList<QVariant> args;
    args.append(on);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Gets if the red profile LED is on/off.
 */
bool Device::getRedLED()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.profile_led", "getRedLED");
    return QDBusMessageToBool(m);
}

/**
 * Sets the red profile LED to on/off.
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
