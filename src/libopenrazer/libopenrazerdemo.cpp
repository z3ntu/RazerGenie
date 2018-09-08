#include "libopenrazer.h"
#include <QDebug>

// Main method for testing / playing.
int main()
{
    libopenrazer::Manager *manager = new libopenrazer::Manager();

    qDebug() << "Daemon running:" << manager->isDaemonRunning();
    qDebug() << "Daemon version:" << manager->getDaemonVersion();
    qDebug() << "Supported devices:" << manager->getSupportedDevices();
    manager->syncEffects(false);

    foreach (const QDBusObjectPath &devicePath, manager->getDevices()) {
        qDebug() << "-----------------";
        libopenrazer::Device *device = new libopenrazer::Device(devicePath);
        qDebug() << "Devicename:" << device->getDeviceName();
        qDebug() << "Devicemode:" << device->getDeviceMode();
//         device->setDeviceMode(0x03, 0x00);
//         qDebug() << "Devicemode:" << device->getDeviceMode();
//         qDebug() << "Driver version:" << device->getDriverVersion();
        qDebug() << "Serial: " << device->getSerial();

        if(device->hasFeature("dpi")) {
            qDebug() << "DPI";
            razer_test::RazerDPI dpi = device->getDPI();
            qDebug() << dpi.dpi_x << dpi.dpi_y;
            device->setDPI({500, 500});
            dpi = device->getDPI();
            qDebug() << dpi.dpi_x << dpi.dpi_y;
            qDebug() << "maxdpi: " << device->maxDPI();
        }

        if(device->hasFeature("poll_rate")) {
            qDebug() << "Set_pollrate:" << device->setPollRate(libopenrazer::POLL_125HZ);
            qDebug() << "Pollrate:" << device->getPollRate();
            qDebug() << "Set_pollrate:" << device->setPollRate(libopenrazer::POLL_1000HZ);
            qDebug() << "Pollrate:" << device->getPollRate();
        }

        foreach (const QDBusObjectPath &ledPath, device->getLeds()) {
            libopenrazer::Led *led = new libopenrazer::Led(ledPath);

            if(device->hasFx("brightness")) {
                qDebug() << "getBrightness";
                qDebug() << led->getBrightness();
            }
        }

        if(device->hasFeature("kbd_layout")) {
            qDebug() << "Keyboard layout:";
            qDebug() << device->getKeyboardLayout();
        }

//         if(device->hasMatrix()) {
//             QList<int> dimen = device->getMatrixDimensions();
//             qDebug() << dimen;
//             qDebug() << dimen[0] << "-" << dimen[1];
//             QList<QColor> colors = QList<QColor>();
//             for(int i=0; i<dimen[1]; i++)
//                 colors << QColor("yellow");
//             qDebug() << "size:" << colors.size();
//             for(int i=0; i<dimen[0]; i++) {
//                 qDebug() << i;
//                 device->setKeyRow(i, 0, dimen[1]-1, colors);
//                 device->setCustom();
//                 qDebug() << "Press Enter to continue.";
//                 std::cin.ignore();
//             }
//         }

//         QHash<QString, bool> hash = device->getAllCapabilities();
//         for (QHash<QString, bool>::iterator i = hash.begin(); i != hash.end(); ++i)
//             qDebug() << i.key() << ": " << i.value();

        delete device;
    }
}
