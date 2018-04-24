#include "libopenrazer.h"
#include <QDebug>

// Main method for testing / playing.
int main()
{
    qDebug() << "Daemon running:" << libopenrazer::isDaemonRunning();
    qDebug() << "Daemon version:" << libopenrazer::getDaemonVersion();
    qDebug() << "Supported devices:" << libopenrazer::getSupportedDevices();
    QStringList serialnrs = libopenrazer::getConnectedDevices();
    libopenrazer::syncEffects(false);
    foreach (const QString &str, serialnrs) {
        qDebug() << "-----------------";
        libopenrazer::Device device = libopenrazer::Device(str);
        qDebug() << "Devicename:" << device.getDeviceName();
        qDebug() << "Devicemode:" << device.getDeviceMode();
//         device.setDeviceMode(0x03, 0x00);
//         qDebug() << "Devicemode:" << device.getDeviceMode();
        qDebug() << "Driver version:" << device.getDriverVersion();
        qDebug() << "Serial: " << str;

        if(device.hasCapability("dpi")) {
            qDebug() << "DPI";
            qDebug() << device.getDPI();
            device.setDPI(500, 500);
            qDebug() << device.getDPI();
            qDebug() << "maxdpi: " << device.maxDPI();
        }

        if(device.hasCapability("mug")) {
            qDebug() << "isMugPresent";
            qDebug() << device.isMugPresent();
        }

        if(device.hasCapability("poll_rate")) {
            qDebug() << "Set_pollrate:" << device.setPollRate(libopenrazer::POLL_125HZ);
            qDebug() << "Pollrate:" << device.getPollRate();
            qDebug() << "Set_pollrate:" << device.setPollRate(libopenrazer::POLL_1000HZ);
            qDebug() << "Pollrate:" << device.getPollRate();
        }

        if(device.hasCapability("get_brightness")) {
            qDebug() << "getBrightness";
            qDebug() << device.getBrightness();
        }
        if(device.hasCapability("get_lighting_logo_brightness")) {
            qDebug() << "getLogoBrightness";
            qDebug() << device.getLogoBrightness();
        }
        if(device.hasCapability("get_lighting_scroll_brightness")) {
            qDebug() << "getScrollBrightness";
            qDebug() << device.getScrollBrightness();
        }
        if(device.hasCapability("backlight")) {
            qDebug() << "Backlight:";
            qDebug() << device.getBacklightActive();
            qDebug() << device.setBacklightActive(false);
            qDebug() << device.getBacklightActive();
        }
        if(device.hasCapability("kbd_layout")) {
            qDebug() << "Keyboard layout:";
            qDebug() << device.getKeyboardLayout();
        }
        if(device.hasCapability("battery")) {
            qDebug() << "Battery:";
            qDebug() << "level: " << device.getBatteryLevel();
            qDebug() << "isCharging: " << device.isCharging();
            device.setIdleTime(10);
        }

//         if(device.hasMatrix()) {
//             QList<int> dimen = device.getMatrixDimensions();
//             qDebug() << dimen;
//             qDebug() << dimen[0] << "-" << dimen[1];
//             QList<QColor> colors = QList<QColor>();
//             for(int i=0; i<dimen[1]; i++)
//                 colors << QColor("yellow");
//             qDebug() << "size:" << colors.size();
//             for(int i=0; i<dimen[0]; i++) {
//                 qDebug() << i;
//                 device.setKeyRow(i, 0, dimen[1]-1, colors);
//                 device.setCustom();
//                 qDebug() << "Press Enter to continue.";
//                 std::cin.ignore();
//             }
//         }

//         QHash<QString, bool> hash = device.getAllCapabilities();
//         for (QHash<QString, bool>::iterator i = hash.begin(); i != hash.end(); ++i)
//             qDebug() << i.key() << ": " << i.value();
    }
}
