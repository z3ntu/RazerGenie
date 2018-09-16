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

#ifndef LIBRAZER_H
#define LIBRAZER_H

#include <QDomDocument>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusInterface>
#include <QDBusReply>

#include <razer_test.h>

#include "razercapability.h"

// NOTE: DBus types -> Qt/C++ types: http://doc.qt.io/qt-5/qdbustypesystem.html#primitive-types

namespace libopenrazer
{
// --- UPDATE FROM https://github.com/openrazer/openrazer/blob/master/pylib/openrazer/client/constants.py ---
// Macro LED effect ID's
enum MacroLEDEffect { MACRO_LED_STATIC = 0x00, MACRO_LED_BLINK = 0x01 };

// Wave Directions
enum WaveDirection { WAVE_RIGHT = 0x01, WAVE_LEFT = 0x02 };

// Reactive
enum ReactiveSpeed { REACTIVE_500MS = 0x01, REACTIVE_1000MS = 0x02, REACTIVE_1500MS = 0x03, REACTIVE_2000MS = 0x04 };

// Starlight
enum StarlightSpeed { STARLIGHT_FAST = 0x01, STARLIGHT_NORMAL = 0x02, STARLIGHT_SLOW = 0x03 };

// Ripple
const double RIPPLE_REFRESH_RATE = 0.05;

enum LEDEffect { STATIC = 0x00, BLINKING = 0x01, PULSATE = 0x02, SPECTRUM = 0x04 };

// Poll rates
enum PollRate { POLL_1000HZ = 1000, POLL_500HZ = 500, POLL_125HZ = 125 };

enum DaemonStatus { Enabled, Disabled, NotInstalled, NoSystemd, Unknown };

const static QList<RazerCapability> ledFxList {
    RazerCapability("off", "Off", 0),
    RazerCapability("static", "Static", 1),
    RazerCapability("breathing", "Breathing", 1),
    RazerCapability("breathing_dual", "Breathing Dual", 2),
    RazerCapability("breathing_random", "Breathing Random", 0),
    RazerCapability("blinking", "Blinking", 1),
    RazerCapability("spectrum", "Spectrum", 0),
    RazerCapability("wave", "Wave", true),
    RazerCapability("reactive", "Reactive", 1),
};

void printDBusError(QDBusError error, const char *functionname);
bool handleBoolReply (QDBusReply<bool> reply, const char *functionname);
QString handleStringReply(QDBusReply<QString> reply, const char *functionname);

class Manager : public QObject
{
    Q_OBJECT
private:
    QDBusInterface *iface = nullptr;
    QDBusInterface *managerIface();

public:
    Manager();

    QList<QDBusObjectPath> getDevices();
    QString getDaemonVersion();
    bool stopDaemon();
    bool isDaemonRunning();

    QVariantHash getSupportedDevices();

    // Sync
    bool syncEffects(bool yes);
    bool getSyncEffects();

    // Screensaver
    bool setTurnOffOnScreensaver(bool turnOffOnScreensaver);
    bool getTurnOffOnScreensaver();

    // Misc
    DaemonStatus getDaemonStatus();
    QString getDaemonStatusOutput();
    bool enableDaemon();
    // bool disableDaemon();

    // - Signal Connect Mehtods -
    bool connectDeviceAdded(QObject *receiver, const char *slot);
    bool connectDeviceRemoved(QObject *receiver, const char *slot);
};

class Device : public QObject
{
    Q_OBJECT
private:
    QDBusInterface *iface = nullptr;
    QDBusInterface *deviceIface();

    QDBusObjectPath mObjectPath;

    QStringList supportedFx;
    QStringList supportedFeatures;

public:
    Device(QDBusObjectPath objectPath);
    ~Device() override;

    QDBusObjectPath objectPath();
    bool hasFx(const QString &fxStr);
    bool hasFeature(const QString &featureStr);
    QString getPngFilename();
    QString getPngUrl();

    QList<QDBusObjectPath> getLeds();

    QStringList getSupportedFx();
    QStringList getSupportedFeatures();

    // --- MISC METHODS ---
    QString getDeviceMode();
    bool setDeviceMode(uchar mode_id, uchar param);
    QString getSerial();
    QString getDeviceName();
    QString getDeviceType();
    QString getFirmwareVersion();
    QString getKeyboardLayout();
    QVariantHash getRazerUrls();

    // --- POLL RATE ---
    ushort getPollRate();
    bool setPollRate(PollRate pollrate);

    // --- DPI ---
    bool setDPI(razer_test::RazerDPI dpi);
    razer_test::RazerDPI getDPI();
    ushort maxDPI();

    // - Custom frame -
    bool displayCustomFrame();
    bool defineCustomFrame(uchar row, uchar startColumn, uchar endColumn, QVector<QColor> colorData);
    razer_test::MatrixDimensions getMatrixDimensions();
};

class Led : public QObject
{
    Q_OBJECT
private:
    QDBusInterface *iface = nullptr;
    QDBusInterface *ledIface();

    QDBusObjectPath mObjectPath;

public:
    Led(QDBusObjectPath objectPath);
    ~Led() override;

    QDBusObjectPath getObjectPath();

    bool setNone();
    bool setStatic(QColor color);
    bool setBreathing(QColor color);
    bool setBreathingDual(QColor color, QColor color2);
    bool setBreathingRandom();
    bool setBlinking(QColor color);
    bool setSpectrum();
    bool setWave(WaveDirection direction);
    bool setReactive(QColor color, ReactiveSpeed speed);

    bool setBrightness(uchar brightness);
    uchar getBrightness();
};

}

// Needed for casting from QVariant
Q_DECLARE_METATYPE(libopenrazer::PollRate)

#endif // LIBRAZER_H
