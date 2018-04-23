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

// Daemon controls
QStringList getConnectedDevices();
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

class Device
{
private:
    QString mSerial;
    QStringList introspection;
    QHash<QString, bool> capabilities;

    QDBusMessage prepareDeviceQDBusMessage(const QString &interface, const QString &method);
    void Introspect();
    void setupCapabilities();

    bool hasCapabilityInternal(const QString &interface, const QString &method = QString());
public:
    Device(QString serial);
    ~Device();

    QString serial();
    bool hasCapability(const QString &name);
    QHash<QString, bool> getAllCapabilities();
    QString getPngFilename();
    QString getPngUrl();

    // --- MISC METHODS ---
    QString getDeviceMode(); //FIXME
    bool setDeviceMode(uchar mode_id, uchar param); //FIXME
    QString getDeviceName();
    QString getDeviceType();
    QString getDriverVersion(); //FIXME
    QString getFirmwareVersion();
    QString getKeyboardLayout();
    QVariantHash getRazerUrls();
    // VID / PID
    int getVid();
    int getPid();

    // --- MACRO ---
    bool hasDedicatedMacroKeys();
    //TODO Rest

    // --- MATRIX ---
    bool hasMatrix();
    QList<int> getMatrixDimensions();

    // --- POLL RATE ---
    int getPollRate();
    bool setPollRate(ushort pollrate);

    // --- DPI ---
    bool setDPI(int dpi_x, int dpi_y);
    QList<int> getDPI();
    int maxDPI();

    // --- BATTERY ----
    bool isCharging();
    double getBatteryLevel();
    bool setIdleTime(ushort idle_time);
    bool setLowBatteryThreshold(uchar threshold);

    // --- MUG ---
    bool isMugPresent();

    // --- LIGHTING EFFECTS ---
    // - Default -
    bool setStatic(uchar r, uchar g, uchar b);
    bool setBreathSingle(uchar r, uchar g, uchar b);
    bool setBreathDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2);
    bool setBreathTriple(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2, uchar r3, uchar g3, uchar b3);
    bool setBreathRandom();
    bool setReactive(uchar r, uchar g, uchar b, uchar speed);
    bool setSpectrum();
    bool setWave(WaveDirection direction);
    bool setNone();
    // Starlight
    bool setStarlightSingle(uchar r, uchar g, uchar b, uchar speed);
    bool setStarlightDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2, uchar speed);
    bool setStarlightRandom(uchar speed);
    // bw2013
    bool setStatic_bw2013();
    bool setPulsate();

    bool getBacklightActive();
    bool setBacklightActive(bool active);
    uchar getBacklightEffect();
    bool setBacklightBrightness(double brightness);
    double getBacklightBrightness();
    bool setBacklightStatic(uchar r, uchar g, uchar b);
    bool setBacklightSpectrum();

    // - Custom(?) -
    bool setCustom();
    bool setKeyRow(uchar row, uchar startcol, uchar endcol, QVector<QColor> colors);

    // - Custom -
    bool setRipple(uchar r, uchar g, uchar b, double refresh_rate);
    bool setRippleRandomColor(double refresh_rate);

    bool setBrightness(double brightness);
    double getBrightness();

    // - Logo -
    bool setLogoStatic(uchar r, uchar g, uchar b);
    bool setLogoActive(bool active);
    bool getLogoActive();
    uchar getLogoEffect();
    bool setLogoBlinking(uchar r, uchar g, uchar b);
    bool setLogoPulsate(uchar r, uchar g, uchar b);
    bool setLogoSpectrum();
    bool setLogoNone();
    bool setLogoReactive(uchar r, uchar g, uchar b, uchar speed);
    bool setLogoBreathSingle(uchar r, uchar g, uchar b);
    bool setLogoBreathDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2);
    bool setLogoBreathRandom();

    bool setLogoBrightness(double brightness);
    double getLogoBrightness();

    // - Scroll -
    bool setScrollStatic(uchar r, uchar g, uchar b);
    bool setScrollActive(bool active);
    bool getScrollActive();
    uchar getScrollEffect();
    bool setScrollBlinking(uchar r, uchar g, uchar b);
    bool setScrollPulsate(uchar r, uchar g, uchar b);
    bool setScrollSpectrum();
    bool setScrollNone();
    bool setScrollReactive(uchar r, uchar g, uchar b, uchar speed);
    bool setScrollBreathSingle(uchar r, uchar g, uchar b);
    bool setScrollBreathDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2);
    bool setScrollBreathRandom();

    bool setScrollBrightness(double brightness);
    double getScrollBrightness();

    // - Profile LED -
    bool getBlueLED();
    bool setBlueLED(bool on);
    bool getGreenLED();
    bool setGreenLED(bool on);
    bool getRedLED();
    bool setRedLED(bool on);

    enum lightingLocation {
        lighting, lighting_logo, lighting_scroll, lighting_backlight
    };
};

}

#endif // LIBRAZER_H
