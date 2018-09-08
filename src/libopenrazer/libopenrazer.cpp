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

void printDBusError(QDBusError error, const char *functionname)
{
    qWarning("libopenrazer: There was an error in %s", functionname);
    qWarning("libopenrazer: %s", qUtf8Printable(error.name()));
    qWarning("libopenrazer: %s", qUtf8Printable(error.message()));
}

bool handleBoolReply (QDBusReply<bool> reply, const char *functionname)
{
    if(reply.isValid()) {
        return true;
    }
    printDBusError(reply.error(), functionname);
    return false;
}

QString handleStringReply(QDBusReply<QString> reply, const char *functionname)
{
    if(reply.isValid()) {
        return reply.value();
    }
    printDBusError(reply.error(), functionname);
    return "error";
}

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
    return {}; // FIXME
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
        return qdbus_cast<QList<QDBusObjectPath>>(reply);
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
    return "error"; // FIXME
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
    return false; // FIXME
}

QString Device::getSerial()
{
    QDBusReply<QString> reply = deviceIface()->call("getSerial");
    return handleStringReply(reply, Q_FUNC_INFO);
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
    return handleStringReply(reply, Q_FUNC_INFO);
}

/*!
 * \fn QString libopenrazer::Device::getKeyboardLayout()
 *
 * Returns the physical layout of the keyboard (e.g. \c 'de_DE', \c 'en_US', \c 'en_GB' or \c 'unknown')
 */
QString Device::getKeyboardLayout()
{
    QDBusReply<QString> reply = deviceIface()->call("getKeyboardLayout");
    return handleStringReply(reply, Q_FUNC_INFO);
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
    return {}; // FIXME
}

/*!
 * \fn int libopenrazer::Device::getPollRate()
 *
 * Returns the current poll rate.
 */
ushort Device::getPollRate()
{
    QDBusReply<ushort> reply = deviceIface()->call("getPollRate");
    if (reply.isValid()) {
        return reply.value();
    } else {
        printDBusError(reply.error(), Q_FUNC_INFO);
        return 0;
    }
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
    QDBusReply<bool> reply = deviceIface()->call("setPollRate", static_cast<ushort>(pollrate));
    return handleBoolReply (reply, Q_FUNC_INFO);
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
    QDBusReply<bool> reply = deviceIface()->call("setDPI", QVariant::fromValue(dpi));
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn QList<int> libopenrazer::Device::getDPI()
 *
 * Returns the DPI of the mouse (e.g. \c [800, 800]).
 */
razer_test::RazerDPI Device::getDPI()
{
    QDBusReply<razer_test::RazerDPI> reply = deviceIface()->call("getDPI");
    if (reply.isValid()) {
        return reply.value();
    } else {
        printDBusError(reply.error(), Q_FUNC_INFO);
        return {0, 0};
    }
}

/*!
 * \fn int libopenrazer::Device::maxDPI()
 *
 * Returns the maximum DPI possible for the device.
 */
int Device::maxDPI()
{
    return 10000; // FIXME
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
    return false; // FIXME
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
    return false; // FIXME
}

}
