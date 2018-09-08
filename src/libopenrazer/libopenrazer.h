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

#include "razercapability.h"
#include "razer_test.h"

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

const static QList<RazerCapability> lightingComboBoxCapabilites {
    RazerCapability("lighting_breath_single", "Breath Single", 1),
    RazerCapability("lighting_breath_dual", "Breath Dual", 2),
    RazerCapability("lighting_breath_triple", "Breath Triple", 3),
    RazerCapability("lighting_breath_random", "Breath Random", 0),
    RazerCapability("lighting_wave", "Wave", true),
    RazerCapability("lighting_reactive", "Reactive", 1),
    RazerCapability("lighting_none", "None", 0),
    RazerCapability("lighting_spectrum", "Spectrum", 0),
    RazerCapability("lighting_static", "Static", 1),
    RazerCapability("lighting_ripple", "Ripple", 1), // Needs "refresh_rate"
    RazerCapability("lighting_ripple_random", "Ripple random", 0), // Needs "refresh_rate"
    RazerCapability("lighting_pulsate", "Pulsate", 0),
    RazerCapability("lighting_static_bw2013", "Static", 0),
};

const static QList<RazerCapability> logoComboBoxCapabilites {
    RazerCapability("lighting_logo_blinking", "Blinking", 1),
    RazerCapability("lighting_logo_pulsate", "Pulsate", 1),
    RazerCapability("lighting_logo_spectrum", "Spectrum", 0),
    RazerCapability("lighting_logo_static", "Static", 1),
    RazerCapability("lighting_logo_none", "None", 0),
    RazerCapability("lighting_logo_reactive", "Reactive", 1),
    RazerCapability("lighting_logo_breath_single", "Breath Single", 1),
    RazerCapability("lighting_logo_breath_dual", "Breath Dual", 2),
    RazerCapability("lighting_logo_breath_random", "Breath random", 0),
};

const static QList<RazerCapability> scrollComboBoxCapabilites {
    RazerCapability("lighting_scroll_blinking", "Blinking", 1),
    RazerCapability("lighting_scroll_pulsate", "Pulsate", 1),
    RazerCapability("lighting_scroll_spectrum", "Spectrum", 0),
    RazerCapability("lighting_scroll_static", "Static", 1),
    RazerCapability("lighting_scroll_none", "None", 0),
    RazerCapability("lighting_scroll_reactive", "Reactive", 1),
    RazerCapability("lighting_scroll_breath_single", "Breath Single", 1),
    RazerCapability("lighting_scroll_breath_dual", "Breath Dual", 2),
    RazerCapability("lighting_scroll_breath_random", "Breath random", 0),
};

const static QList<RazerCapability> backlightComboBoxCapabilites {
    RazerCapability("lighting_backlight_spectrum", "Spectrum", 0),
    RazerCapability("lighting_backlight_static", "Static", 1),
};

class Manager : public QObject
{
    Q_OBJECT
private:
    QDBusInterface *iface = nullptr;
    QDBusInterface *managerIface();

public:
    Manager();

    // Daemon controls
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
    ~Device();

    QDBusObjectPath objectPath();
    bool hasCapability(const QString &name); // TODO REMOVE
    bool hasFx(const QString &fxStr);
    bool hasFeature(const QString &featureStr);
    QHash<QString, bool> getAllCapabilities();
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
    uint getPollRate();
    bool setPollRate(PollRate pollrate);

    // --- DPI ---
    bool setDPI(razer_test::RazerDPI dpi);
    razer_test::RazerDPI getDPI();
    int maxDPI();

    // - Custom frame -
    bool setCustom();
    bool setKeyRow(uchar row, uchar startcol, uchar endcol, QVector<QColor> colors);
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
    ~Led();

    bool setNone();
    bool setStatic(QColor color);
    bool setBreathingSingle(QColor color);
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
