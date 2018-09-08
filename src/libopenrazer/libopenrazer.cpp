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

#include <QDBusArgument>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDebug>
#include <QDomDocument>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QVariantHash>
#include <QDBusReply>
#include <QtGui/qcolor.h>

#include <iostream>

#include "libopenrazer.h"

#define OPENRAZER_SERVICE_NAME "io.github.openrazer1"

/*!
    \namespace libopenrazer
    \inmodule libopenrazer

    \brief Contains methods, enums and classes to interface with the OpenRazer daemon via the D-Bus interface.
*/
namespace libopenrazer
{

// ----- MISC METHODS FOR LIBOPENRAZER -----

QDBusInterface *Device::deviceIface()
{
    if(iface == nullptr) {
        iface = new QDBusInterface(OPENRAZER_SERVICE_NAME, mObjectPath.path(), "io.github.openrazer1.Device",
                                   QDBusConnection::sessionBus(), this);
    }
    if(!iface->isValid()) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::sessionBus().lastError().message()));
    }
    return iface;
}

QDBusInterface *Manager::managerIface()
{
    if(iface == nullptr) {
        iface = new QDBusInterface(OPENRAZER_SERVICE_NAME, "/io/github/openrazer1", "io.github.openrazer1.Manager",
                                   QDBusConnection::sessionBus(), this);
    }
    if(!iface->isValid()) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::sessionBus().lastError().message()));
    }
    return iface;
}

QDBusInterface *Led::ledIface()
{
    if(iface == nullptr) {
        iface = new QDBusInterface(OPENRAZER_SERVICE_NAME, mObjectPath.path(), "io.github.openrazer1.Led",
                                   QDBusConnection::sessionBus(), this);
    }
    if(!iface->isValid()) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::sessionBus().lastError().message()));
    }
    return iface;
}

/**
 * Prints out relevant error information about a failed DBus call.
 */
void printError(QDBusMessage reqMessage, QDBusMessage& message, const char *functionname)
{
    qWarning() << "libopenrazer: There was an error in" << functionname << "!";
    qWarning() << "libopenrazer:" << message.errorName();
    qWarning() << "libopenrazer:" << message.errorMessage();
    qWarning() << "libopenrazer:" << reqMessage.service() << reqMessage.path() << reqMessage.interface() << reqMessage.member();
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
    printError(message, msg, Q_FUNC_INFO);
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
    printError(message, msg, Q_FUNC_INFO);
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
    printError(message, msg, Q_FUNC_INFO);
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
    printError(message, msg, Q_FUNC_INFO);
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
    printError(message, msg, Q_FUNC_INFO);
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
    printError(message, msg, Q_FUNC_INFO);
    return msg.arguments()[0].toStringList();
}

/**
 * Sends a QDBusMessage and returns the QDBusObjectPath array value.
 */
QList<QDBusObjectPath> QDBusMessageToObjectPathArray(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        qDebug() << msg;
        qDebug() << "reply arguments : " << msg.arguments();
        qDebug() << "reply[0] :" << msg.arguments().at(0);
        return qdbus_cast<QList<QDBusObjectPath>>(msg.arguments()[0].value<QDBusArgument>());
    }
    // TODO: Handle error
    printError(message, msg, Q_FUNC_INFO);
    return QList<QDBusObjectPath>();
}

QList<QDBusObjectPath> QVariantToObjectPathArray(const QVariant &variant)
{
    return qdbus_cast<QList<QDBusObjectPath>>(variant);
}

/**
 * Sends a QDBusMessage and returns the int array value.
 */
QList<int> QDBusMessageToIntArray(const QDBusMessage &message)
{
    QList<int> retList;
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
//         qDebug() << "reply :" << msg; // sth like QDBusMessage(type=MethodReturn, service=":1.1482", signature="ai", contents=([Argument: ai {5426, 67}]) )
//         qDebug() << "reply arguments : " << msg.arguments();
//         qDebug() << "reply[0] :" << msg.arguments().at(0);

        const QDBusArgument myArg = msg.arguments().at(0).value<QDBusArgument>();
        myArg.beginArray();
        while (!myArg.atEnd()) {
            int myElement = qdbus_cast<int>(myArg);
            retList.append(myElement);
        }
        myArg.endArray();
        return retList;
    }
    // TODO: Handle error
    printError(message, msg, Q_FUNC_INFO);
    return retList;
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
    printError(message, msg, Q_FUNC_INFO);
    return doc;
}

/**
 * Sends a QDBusMessage and returns if the call was successful.
 */
bool QDBusMessageToVoid(const QDBusMessage &message)
{
    qDebug() << message;
    return QDBusConnection::sessionBus().send(message);
    // TODO: Handle error ?
}

/*!
 * \fn bool libopenrazer::isDaemonRunning()
 *
 * Returns if the daemon is running (and responding to the version call).
 */
bool Manager::isDaemonRunning()
{
    QVariant reply = managerIface()->property("Version");
    return !reply.isNull();
}

/*!
 * \fn QVariantHash libopenrazer::getSupportedDevices()
 *
 * Returns a list of supported devices in the format of \c {QHash<QString(DeviceName), QList<double(VID), double(PID)>>}.
 *
 * \sa Device::getVid(), Device::getPid()
 */
QVariantHash Manager::getSupportedDevices()
{
    return QVariantHash();
//     QDBusMessage m = prepareManagerQDBusMessage("razer.devices", "supportedDevices");
//     QString ret = QDBusMessageToString(m);
//     return QJsonDocument::fromJson(ret.toUtf8()).object().toVariantHash();
}

/*!
 * \fn QStringList libopenrazer::getConnectedDevices()
 *
 * Returns a list of connected devices in form of their serial number (e.g. \c ['XX0000000001', 'PM1439131641838']).
 *
 * Can be used to create a libopenrazer::Device object and get further information about the device.
 */
QList<QDBusObjectPath> Manager::getDevices()
{
    QVariant reply = managerIface()->property("Devices");
    if (!reply.isNull())
        return QVariantToObjectPathArray(reply);
    else
        return {};
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
bool Manager::syncEffects(bool yes)
{
    return false;
//     QDBusMessage m = prepareManagerQDBusMessage("razer.devices", "syncEffects");
//     QList<QVariant> args;
//     args.append(yes);
//     m.setArguments(args);
//     return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::getSyncEffects()
 *
 * Returns if devices should sync effect.
 *
 * \sa syncEffects()
 */
bool Manager::getSyncEffects()
{
    return false;
//     QDBusMessage m = prepareManagerQDBusMessage("razer.devices", "getSyncEffects");
//     return QDBusMessageToBool(m);
}

/*!
 * \fn QString libopenrazer::getDaemonVersion()
 *
 * Returns the daemon version currently running (e.g. \c '2.3.0').
 */
QString Manager::getDaemonVersion()
{
    QVariant reply = managerIface()->property("Version");
    if(!reply.isNull())
        return reply.toString();
    else
        return "error";
}

/*!
 * \fn bool libopenrazer::stopDaemon()
 *
 * Stops the OpenRazer daemon. \b WARNING: FURTHER COMMUNICATION WILL NOT BE POSSIBLE.
 *
 * Returns if the D-Bus call was successful.
 */
bool Manager::stopDaemon()
{
    return false;
//     QDBusMessage m = prepareManagerQDBusMessage("razer.daemon", "stop");
//     return QDBusMessageToVoid(m);
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
bool Manager::setTurnOffOnScreensaver(bool turnOffOnScreensaver)
{
    return false;
//     QDBusMessage m = prepareManagerQDBusMessage("razer.devices", "enableTurnOffOnScreensaver");
//     QList<QVariant> args;
//     args.append(turnOffOnScreensaver);
//     m.setArguments(args);
//     return QDBusMessageToVoid(m);
}

/*!
 * \fn bool libopenrazer::getTurnOffOnScreensaver()
 *
 * Returns if the LEDs should turn off if the screensaver is turned on.
 *
 * \sa setTurnOffOnScreensaver()
 */
bool Manager::getTurnOffOnScreensaver()
{
    return false;
//     QDBusMessage m = prepareManagerQDBusMessage("razer.devices", "getOffOnScreensaver");
//     return QDBusMessageToBool(m);
}

/*!
 * \fn DaemonStatus libopenrazer::getDaemonStatus()
 *
 * Returns status of the daemon, see DaemonStatus.
 */
DaemonStatus Manager::getDaemonStatus()
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
QString Manager::getDaemonStatusOutput()
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
bool Manager::enableDaemon()
{
    QProcess process;
    process.start("systemctl", QStringList() << "--user" << "enable" << "openrazer-daemon.service");
    process.waitForFinished();
    return process.exitCode() == 0;
}

Manager::Manager()
{
    // Register types
    qDBusRegisterMetaType<razer_test::RazerDPI>();
}

// ====== DEVICE CLASS ======

/*!
 * \class libopenrazer::Device
 * \inmodule libopenrazer
 *
 * \brief The libopenrazer::Device class provides an abstraction for the OpenRazer daemon D-Bus interface for easy interaction from C++ applications.
 */

/*!
 * \fn libopenrazer::Device::Device(QString serial)
 *
 * Constructs a new device object with the given \a serial.
 */
Device::Device(QDBusObjectPath objectPath)
{
    mObjectPath = objectPath;
    // FIXME Initialize LEDs for device
    supportedFx = this->getSupportedFx();
    supportedFeatures = this->getSupportedFeatures();
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
    return QDBusConnection::sessionBus().connect(OPENRAZER_SERVICE_NAME, "/org/razer", "razer.devices", "device_added", receiver, slot);
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
    return QDBusConnection::sessionBus().connect(OPENRAZER_SERVICE_NAME, "/org/razer", "razer.devices", "device_removed", receiver, slot);
}

// ---- MISC METHODS ----
/*!
 * \fn QString libopenrazer::Device::serial()
 *
 * Returns the device serial.
 */
QDBusObjectPath Device::objectPath()
{
    return mObjectPath;
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
    return supportedFx.contains(name) || supportedFeatures.contains(name);
}

bool Device::hasFx(const QString &fxStr)
{
    return supportedFx.contains(fxStr);
}

bool Device::hasFeature(const QString &featureStr)
{
    return supportedFeatures.contains(featureStr);
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
    // FIXME
    return {};
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

QList<QDBusObjectPath> Device::getLeds()
{
    QVariant reply = deviceIface()->property("Leds");
    if (!reply.isNull())
        return QVariantToObjectPathArray(reply);
    else
        return {};
}

QStringList Device::getSupportedFx()
{
    QVariant reply = deviceIface()->property("SupportedFx");
    if (!reply.isNull())
        return reply.toStringList();
    else
        return {};
}

QStringList Device::getSupportedFeatures()
{
    QVariant reply = deviceIface()->property("SupportedFeatures");
    if (!reply.isNull())
        return reply.toStringList();
    else
        return {};
}

/*!
 * \fn QString libopenrazer::Device::getDeviceMode()
 *
 * Returns the device mode of the device, like '0:0' or '3:0' for normal mode and driver mode respetively.
 *
 * \sa setDeviceMode()
 */
QString Device::getDeviceMode()
{
    return "error";
//     QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceMode");
//     return QDBusMessageToString(m);
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
    return false;
//     QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "setDeviceMode");
//     QList<QVariant> args;
//     args.append(mode_id);
//     args.append(param);
//     m.setArguments(args);
//     return QDBusMessageToVoid(m);
}

QString Device::getSerial()
{
    QDBusReply<QString> reply = deviceIface()->call("getSerial");
    if (reply.isValid())
        return reply.value();
    else
        return "error";
}

/*!
 * \fn QString libopenrazer::Device::getDeviceName()
 *
 * Returns a human readable device name like \c {"Razer DeathAdder Chroma"} or \c {"Razer Kraken 7.1"}.
 */
QString Device::getDeviceName()
{
    QVariant reply = deviceIface()->property("Name");
    if(!reply.isNull())
        return reply.toString();
    else
        return "error";
}

/*!
 * \fn QString libopenrazer::Device::getDeviceType()
 *
 * Returns the type of the device. Could be one of \c 'keyboard', \c 'mouse', \c 'mousemat', \c 'core', \c 'keypad', \c 'headset', \c 'mug' or another type, if added to the daemon.
 */
QString Device::getDeviceType()
{
    QVariant reply = deviceIface()->property("Type");
    if(!reply.isNull())
        return reply.toString();
    else
        return "error";
}

/*!
 * \fn QString libopenrazer::Device::getFirmwareVersion()
 *
 * Returns the firmware version of the device (e.g. \c 'v1.0').
 */
QString Device::getFirmwareVersion()
{
    QDBusReply<QString> reply = deviceIface()->call("getFirmwareVersion");
    if (reply.isValid())
        return reply.value();
    else
        return "error";
}

/*!
 * \fn QString libopenrazer::Device::getKeyboardLayout()
 *
 * Returns the physical layout of the keyboard (e.g. \c 'de_DE', \c 'en_US', \c 'en_GB' or \c 'unknown')
 */
QString Device::getKeyboardLayout()
{
    QDBusReply<QString> reply = deviceIface()->call("getKeyboardLayout");
    if (reply.isValid())
        return reply.value();
    else
        return "error";
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
    return {};
//     QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getRazerUrls");
//     QString ret = QDBusMessageToString(m);
//     return QJsonDocument::fromJson(ret.toUtf8()).object().toVariantHash();
}

/*!
 * \fn int libopenrazer::Device::getPollRate()
 *
 * Returns the current poll rate.
 */
ushort Device::getPollRate()
{
    QDBusReply<ushort> reply = deviceIface()->call("getPollRate");
    if (reply.isValid())
        return reply.value();
    else
        return 0;
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
    QDBusReply<QDBusVariant> reply = deviceIface()->call("setPollRate", static_cast<ushort>(pollrate));
    return reply.isValid();
}

/*!
 * \fn bool libopenrazer::Device::setDPI(int dpi_x, int dpi_y)
 *
 * Sets the DPI of the mouse to the specified \a dpi_x for the x-Axis and \a dpi_y for the y-Axis. Maximum value is what is returned by maxDPI().
 *
 * Returns if the D-Bus call was successful.
 */
bool Device::setDPI(razer_test::RazerDPI dpi)
{
    QDBusReply<QDBusVariant> reply = deviceIface()->call("setDPI", QVariant::fromValue(dpi));
    return reply.isValid();
}

/*!
 * \fn QList<int> libopenrazer::Device::getDPI()
 *
 * Returns the DPI of the mouse (e.g. \c [800, 800]).
 */
razer_test::RazerDPI Device::getDPI()
{
    QDBusReply<razer_test::RazerDPI> reply = deviceIface()->call("getDPI");
    if (reply.isValid())
        return reply.value();
    else
        return {0, 0};
}

/*!
 * \fn int libopenrazer::Device::maxDPI()
 *
 * Returns the maximum DPI possible for the device.
 */
int Device::maxDPI()
{
    return 10000; // FIXME
//     QDBusMessage m = prepareDeviceQDBusMessage("razer.device.dpi", "maxDPI");
//     return QDBusMessageToInt(m);
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
    return false;
//     QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setCustom");
//     return QDBusMessageToVoid(m);
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
    return false;
//     if(colors.count() != (endcol+1)-startcol) {
//         qWarning() << "Invalid 'colors' length. startcol:" << startcol << " - endcol:" << endcol << " needs " << (endcol+1)-startcol << " entries in colors!";
//         return false;
//     }
//
//     QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setKeyRow");
//
//     QByteArray parameters;
//     parameters[0] = row;
//     parameters[1] = startcol;
//     parameters[2] = endcol;
//     int counter = 3;
//     foreach(QColor c, colors) {
//         // set the rgb to the parameters[i]
//         parameters[counter++] = c.red();
//         parameters[counter++] = c.green();
//         parameters[counter++] = c.blue();
//     }
//
//     QList<QVariant> args;
//     args.append(parameters);
//     m.setArguments(args);
//     return QDBusMessageToVoid(m);
}

}
