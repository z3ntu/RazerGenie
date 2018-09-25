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

#include <QProcess>
#include <QDBusMetaType>
#include <QFileInfo>

#include "libopenrazer.h"

namespace libopenrazer
{

Manager::Manager()
{
    // Register the enums with the Qt system
    razer_test::registerMetaTypes();
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
    return QVariantHash(); // FIXME
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
        return qdbus_cast<QList<QDBusObjectPath>>(reply);
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
    return false; // FIXME
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
    return false; // FIXME
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
    return false; // FIXME
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
    return false; // FIXME
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
    return false; // FIXME
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
        qWarning("libopenrazer: There was an error checking if the daemon is enabled. Unit state is: %s. Error message: %s", qUtf8Printable(output), qUtf8Printable(error));
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
bool Manager::connectDeviceAdded(QObject *receiver, const char *slot)
{
    return QDBusConnection::sessionBus().connect("io.github.openrazer1", "/org/razer", "razer.devices", "device_added", receiver, slot); // FIXME
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
bool Manager::connectDeviceRemoved(QObject *receiver, const char *slot)
{
    return QDBusConnection::sessionBus().connect("io.github.openrazer1", "/org/razer", "razer.devices", "device_removed", receiver, slot); // FIXME
}

}
