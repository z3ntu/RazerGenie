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

#ifndef LIBRAZER_H
#define LIBRAZER_H

#include <QDomDocument>
#include <QDBusMessage>
#include "razercapability.h"

namespace librazer
{
// --- UPDATE FROM https://raw.githubusercontent.com/terrycain/razer-drivers/master/pylib/razer/client/constants.py ---
// Macro LED effect ID's
const int MACRO_LED_STATIC = 0x00;
const int MACRO_LED_BLINK = 0x01;

// Wave Directions
const int WAVE_RIGHT = 0x01;
const int WAVE_LEFT = 0x02;

// Reactive
const int REACTIVE_500MS = 0x01;
const int REACTIVE_1000MS = 0x02;
const int REACTIVE_1500MS = 0x03;
const int REACTIVE_2000MS = 0x04;

// Ripple
const double RIPPLE_REFRESH_RATE = 0.05;

// Poll rates
const int POLL_1000HZ = 1000;
const int POLL_500HZ = 500;
const int POLL_128HZ = 128;

QStringList getConnectedDevices();
QString getDaemonVersion();
bool syncEffects(bool yes);
bool getSyncEffects();
bool setTurnOffOnScreensaver(bool turnOffOnScreensaver);
bool getTurnOffOnScreensaver();
bool stopDaemon();
bool isDaemonRunning();
/* Helper methods */
bool QDBusMessageToBool(const QDBusMessage &message);
double QDBusMessageToDouble(const QDBusMessage &message);
QString QDBusMessageToString(const QDBusMessage &message);
QStringList QDBusMessageToStringList(const QDBusMessage &message);
QList<int> QDBusMessageToIntArray(const QDBusMessage &message);
QDomDocument QDBusMessageToXML(const QDBusMessage &message);
bool QDBusMessageToVoid(const QDBusMessage& message);
QDBusMessage prepareGeneralQDBusMessage(const QString &interface, const QString &method);

// razer-drivers name to picture url, relative to http://assets.razerzone.com/eeimages/products/. Images are available on the product pages and/or store pages.
/*const static QHash<int, QString> urlLookup {
    {21, "22133/razer-naga-classic-gallery-4.png"}, // Razer Naga
    {47, "37/razer-imperator-gallery-5.png"}, // Razer Imperator 2012
    {57, "6713/razer-orochi-2013-gallery-1.png"}, // Razer Orochi 2013
    {62, "20776/rzrnagaepicchroma_06a.png"}, // Razer Naga Epic Chroma (mouse)
    {63, "20776/rzrnagaepicchroma_06a.png"}, // Razer Naga Epic Chroma (dock)
    {66, "17026/abyssus2014_gallery_1.png"}, // Razer Abyssus 2014
    {67, "17531/deathadder_chroma_gallery_2.png"}, // Razer DeathAdder Chroma
    {68, "22343/razer-mamba-gallery-03.png"}, // Razer Mamba (wired)
    {69, "22343/razer-mamba-gallery-03.png"}, // Razer Mamba (wireless)
    {70, "22294/mambategallery-800x800-1.png"}, // Razer Mamba Tournament Edition
    {72, "22770/razer-orochi-05-01.png"}, // Razer Orochi 2015
    {73, "22770/razer-orochi-01-01.png"}, //FIXME Orochi wireless, usb pid unknown, just a guess
    {76, "22772/rzr_diamondback_01.png"}, // Razer Diamondback Chroma
    {80, "25031/nagahexv2-gallery-2.png"}, // Razer Naga Hex V2
    {83, "23322/rzrnagachroma_gallery01.png"}, // Razer Naga Chroma
    {92, "25919/daelite_gallery01.png"}, // Razer DeathAdder Elite
    {519, "22627/razer-orbweaver-chroma-800x800-1.png"}, // Razer Orbweaver Chroma
    {520, "22356/razer-tartarus-chroma-01-02.png"}, // Razer Tartarus Chroma
    {1284, "17519/01.png"}, // Razer Kraken 7.1 Chroma
    {1296, "26005/kraken71v2_gallery04-v2.png"}, // Razer Kraken 7.1 v2
    {2562, "24708/manowar-gallery-v1-3.png"}, // Razer ManO'War
    {3072, "21936/rzr_firefly_gallery-2.png"} // Razer Firefly
    //{3847, "mug"} no image :(
};*/

const static QList<RazerCapability> lightingComboBoxCapabilites {
    RazerCapability("lighting_breath_single", "Breath Single", 1),
    RazerCapability("lighting_breath_dual", "Breath Dual", 2),
    RazerCapability("lighting_breath_triple", "Breath Triple", 3),
    RazerCapability("lighting_breath_random", "Breath Random", 0),
    RazerCapability("lighting_wave", "Wave", 0), // Needs "direction"
    RazerCapability("lighting_reactive", "Reactive", 1),
    RazerCapability("lighting_none", "None", 0),
    RazerCapability("lighting_spectrum", "Spectrum", 0),
    RazerCapability("lighting_static", "Static", 1),
    RazerCapability("lighting_ripple", "Ripple", 1), // Needs "refresh_rate"
    RazerCapability("lighting_ripple_random", "Ripple random", 0), // Needs "refresh_rate"
    RazerCapability("lighting_pulsate", "Pulsate", 1),
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

class Device //: public QObject
{
    //Q_OBJECT
private:
    QString serial;
    QStringList introspection;
    QHash<QString, bool> capabilities;

    QDBusMessage prepareDeviceQDBusMessage(const QString &interface, const QString &method);
    void Introspect();
    void setupCapabilities();

    bool hasCapabilityInternal(const QString &interface, const QString &method = QString());
private slots:
    void deviceAdded();
    void deviceRemoved(uint);
public:
    Device(QString serial);
    ~Device();
    // Misc methods
    QString getDeviceName();
    QString getDeviceType();
    QString getFirmwareVersion();
    QString getPngFilename();
    QString getPngUrl();
    QVariantHash getRazerUrls();
    bool hasCapability(const QString &name);
    QHash<QString, bool> getAllCapabilities();

    // VID / PID
    int getVid();
    int getPid();

    // Battery
    bool isCharging();
    double getBatteryLevel();
    bool setIdleTime(int idle_time);
    bool setLowBatteryThreshold(int threshold);

    // --- LIGHTING EFFECTS ---
    // - Default -
    bool setStatic(int r, int g, int b);
    bool setBreathSingle(int r, int g, int b);
    bool setBreathDual(int r, int g, int b, int r2, int g2, int b2);
    bool setBreathTriple(int r, int g, int b, int r2, int g2, int b2, int r3, int g3, int b3);
    bool setBreathRandom();
    bool setReactive(int r, int g, int b, int speed);
    bool setSpectrum();
    bool setWave(const int direction);
    bool setNone();

    bool setBrightness(double brightness);
    double getBrightness();

    // - Logo -
    bool setLogoStatic(int r, int g, int b);
    bool setLogoActive(bool active);
    bool getLogoActive();

    bool setLogoBrightness(double brightness);
    double getLogoBrightness();

    // - Scroll -
    bool setScrollStatic(int r, int g, int b);

    bool setScrollBrightness(double brightness);
    double getScrollBrightness();


    enum lightingLocations {
        lighting, lighting_logo, lighting_scroll
    };
};

}

#endif // LIBRAZER_H
