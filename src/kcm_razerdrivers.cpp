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

#include <iostream>
#include <QtDBus/QDBusConnection>
#include <QDBusServiceWatcher>
#include <QtWidgets>

#include <config.h>

#include "kcm_razerdrivers.h"
#include "librazer/librazer.h"
#include "librazer/razercapability.h"
#include "razerimagedownloader.h"
#include "razerpagewidgetitem.h"

kcm_razerdrivers::kcm_razerdrivers(QWidget *parent) : QWidget(parent)
{
    // About dialog
    /*
    KAboutData *about = new KAboutData(
        "kcm_razerdrivers",
        "RazerDrivers",
        KCM_RAZERDRIVERS_VERSION,
        QString("A KDE system settings module for managing razer devices."),
        KAboutLicense::GPL_V3,
        "Copyright (C) 2016-2017 Luca Weiss",
        QString(),
        "https://github.com/z3ntu/kcm_razerdrivers");
    // Obfuscation just for spiders :)
    about->addAuthor("Luca Weiss", "Main Developer", QString("luca%1z3ntu%2xyz").arg("@", "."), "https://z3ntu.xyz");
    about->addCredit("Terry Cain", "razer-drivers project", QString(), "https://terrycain.github.io/razer-drivers");
    setAboutData(about);
    */
    // Watch for dbus service changes (= daemon ends or gets started)
    QDBusServiceWatcher *watcher = new QDBusServiceWatcher("org.razer", QDBusConnection::sessionBus());
    connect(watcher, &QDBusServiceWatcher::serviceRegistered,
            this, &kcm_razerdrivers::dbusServiceRegistered);
    connect(watcher, &QDBusServiceWatcher::serviceUnregistered,
            this, &kcm_razerdrivers::dbusServiceUnregistered);

    // Check if daemon available
    if(!librazer::isDaemonRunning()) {
        setupErrorUi();
    } else {
        setupUi();
    }
}

kcm_razerdrivers::~kcm_razerdrivers()
{
//    delete ui;
}

void kcm_razerdrivers::setupErrorUi()
{
    qDebug() << "DAEMON IS NOT RUNNING. ABORTING!";
    ui_error.setupUi(this);
}

void kcm_razerdrivers::setupUi()
{
    ui_main.setupUi(this);

    ui_main.versionLabel->setText("Daemon version: " + librazer::getDaemonVersion());

    fillList();

    //Connect signals
    connect(ui_main.syncCheckBox, &QCheckBox::clicked, this, &kcm_razerdrivers::toggleSync);
    ui_main.syncCheckBox->setChecked(librazer::getSyncEffects());
    connect(ui_main.screensaverCheckBox, &QCheckBox::clicked, this, &kcm_razerdrivers::toggleOffOnScreesaver);
    ui_main.screensaverCheckBox->setChecked(librazer::getTurnOffOnScreensaver());
}

void kcm_razerdrivers::dbusServiceRegistered(const QString &serviceName)
{
    std::cout << "Registered! " << serviceName.toStdString() << std::endl;
    showInfo("Please restart the application to see the interface for now.");
//     setupUi();
}

void kcm_razerdrivers::dbusServiceUnregistered(const QString &serviceName)
{
    std::cout << "Unregistered! " << serviceName.toStdString() << std::endl;
    showError("The dbus service connection was lost. Please restart the daemon (\"razer-service\")");
}

void kcm_razerdrivers::fillList()
{
    // Get all connected devices
    QStringList serialnrs = librazer::getConnectedDevices();

    // Iterate through all devices
    foreach (const QString &serial, serialnrs) {
        // Create device instance with current serial
        librazer::Device *currentDevice = new librazer::Device(serial);

        // Download image for device
        if(!currentDevice->getPngFilename().isEmpty()) {
            RazerImageDownloader *dl = new RazerImageDownloader(serial, QUrl(currentDevice->getPngUrl()), this);
            connect(dl, &RazerImageDownloader::downloadFinished, this, &kcm_razerdrivers::imageDownloaded);
        } else {
            qDebug() << ".png mapping for device '" + currentDevice->getDeviceName() + "' (PID "+QString::number(currentDevice->getPid())+") missing.";
        }

        // Setup variables for easy access
        QString type = currentDevice->getDeviceType();
        QString name = currentDevice->getDeviceName();

        qDebug() << serial;
        qDebug() << name;

        // TODO needed?
        // Insert current device with serial lookup into a QHash
        devices.insert(serial, currentDevice);

        // Types known for now: headset, mouse, mug, keyboard, tartarus, core, orbweaver
        qDebug() << type;

        // Create widget to add into the page
        QWidget *widget = new QWidget();
        QVBoxLayout *verticalLayout = new QVBoxLayout(widget);

        // List of locations to iterate through
        QList<librazer::Device::lightingLocations> locationsTodo;

        // Check what lighting locations the device has
        if(currentDevice->hasCapability("lighting"))
            locationsTodo.append(librazer::Device::lighting);
        if(currentDevice->hasCapability("lighting_logo"))
            locationsTodo.append(librazer::Device::lighting_logo);
        if(currentDevice->hasCapability("lighting_scroll"))
            locationsTodo.append(librazer::Device::lighting_scroll);

        // Iterate through lighting locations
        while(locationsTodo.size() != 0) {
            // Get location we are iterating through
            librazer::Device::lightingLocations currentLocation = locationsTodo.takeFirst();

            QLabel *text;

            // Set appropriate text
            if(currentLocation == librazer::Device::lighting) {
                text = new QLabel("Lighting");
            } else if(currentLocation == librazer::Device::lighting_logo) {
                text = new QLabel("Lighting Logo");
            } else if(currentLocation == librazer::Device::lighting_scroll) {
                text = new QLabel("Lighting Scroll");
            } else {
                // Houston, we have a problem.
            }

            QHBoxLayout *hbox = new QHBoxLayout();
            verticalLayout->addWidget(text);
            verticalLayout->addLayout(hbox);
            QComboBox *comboBox = new QComboBox;
            QLabel *brightnessLabel = NULL;
            QSlider *brightnessSlider = NULL;

            //TODO Remove combobox if no capability is available (Abyssus)
            //TODO Toggle for Abyssus
            //TODO Speed for reactive
            //TODO Battery
            //TODO Keyboard stuff (dunno what exactly)
            //TODO Sync effects in comboboxes & colorStuff when the sync checkbox is active
            //TODO DPI
            //TODO poll rate
            //TODO Matrix stuff

            if(currentLocation == librazer::Device::lighting) {
                // Add items from capabilities
                for(int i=0; i<librazer::lightingComboBoxCapabilites.size(); i++) {
                    if(currentDevice->hasCapability(librazer::lightingComboBoxCapabilites[i].getIdentifier())) {
                        comboBox->addItem(librazer::lightingComboBoxCapabilites[i].getDisplayString(), QVariant::fromValue(librazer::lightingComboBoxCapabilites[i]));
                    }
                }

                // Connect signal from combobox
                connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::standardCombo);

                // Brightness slider
                if(currentDevice->hasCapability("brightness")) {
                    brightnessLabel = new QLabel("Brightness");
                    brightnessSlider = new QSlider(Qt::Horizontal, widget);
                    if(currentDevice->hasCapability("get_brightness")) {
                        brightnessSlider->setValue(currentDevice->getBrightness());
                    }
                    connect(brightnessSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::brightnessChanged);
                }

            } else if(currentLocation == librazer::Device::lighting_logo) {
                // Add items from capabilities
                for(int i=0; i<librazer::logoComboBoxCapabilites.size(); i++) {
                    if(currentDevice->hasCapability(librazer::logoComboBoxCapabilites[i].getIdentifier())) {
                        comboBox->addItem(librazer::logoComboBoxCapabilites[i].getDisplayString(), QVariant::fromValue(librazer::logoComboBoxCapabilites[i]));
                    }
                }

                // Connect signal from combobox
                connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::logoCombo);

                // Brightness slider
                if(currentDevice->hasCapability("lighting_logo_brightness")) {
                    brightnessLabel = new QLabel("Brightness Logo");
                    brightnessSlider = new QSlider(Qt::Horizontal, widget);
                    if(currentDevice->hasCapability("get_lighting_logo_brightness")) {
                        brightnessSlider->setValue(currentDevice->getLogoBrightness());
                    }
                    connect(brightnessSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::logoBrightnessChanged);
                }

            } else if(currentLocation == librazer::Device::lighting_scroll) {
                // Add items from capabilities
                for(int i=0; i<librazer::scrollComboBoxCapabilites.size(); i++) {
                    if(currentDevice->hasCapability(librazer::scrollComboBoxCapabilites[i].getIdentifier())) {
                        comboBox->addItem(librazer::scrollComboBoxCapabilites[i].getDisplayString(), QVariant::fromValue(librazer::scrollComboBoxCapabilites[i]));
                    }
                }

                // Connect signal from combobox
                connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::scrollCombo);

                // Brightness slider
                if(currentDevice->hasCapability("lighting_scroll_brightness")) {
                    brightnessLabel = new QLabel("Brightness Scroll");
                    brightnessSlider = new QSlider(Qt::Horizontal, widget);
                    if(currentDevice->hasCapability("get_lighting_scroll_brightness")) {
                        brightnessSlider->setValue(currentDevice->getScrollBrightness());
                    }
                    connect(brightnessSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::scrollBrightnessChanged);
                }
            }

            // Only add combobox if a capability was actually added
            if(comboBox->count() != 0) {
                hbox->addWidget(comboBox);

                /* Color buttons */ //TODO Connect (plz be ez)
                for(int i=1; i<=3; i++) {
                    QPushButton *colorButton = new QPushButton(widget);
                    QPalette pal = colorButton->palette();
                    // TODO: Set color when set
                    pal.setColor(QPalette::Button, QColor(Qt::green));
                    colorButton->setAutoFillBackground(true);
                    colorButton->setFlat(true);
                    colorButton->setPalette(pal);
                    colorButton->setMaximumWidth(70);
                    colorButton->setObjectName("colorbutton" + QString::number(i));
                    hbox->addWidget(colorButton);

                    librazer::RazerCapability capability = comboBox->currentData().value<librazer::RazerCapability>();
                    if(capability.getNumColors() < i)
                        colorButton->hide();
                    connect(colorButton, &QPushButton::clicked, this, &kcm_razerdrivers::colorButtonClicked);
                }
            } else { //TODO Should that button also be added always?
                if(currentDevice->hasCapability("lighting_logo_active")) {
                    // TODO Add toggle
                }
            }
            /* Brightness sliders */
            if(brightnessLabel != NULL && brightnessSlider != NULL) { // only if brightness capability exists
                verticalLayout->addWidget(brightnessLabel);
                QHBoxLayout *hboxSlider = new QHBoxLayout();
                QLabel *brightnessSliderValue = new QLabel;
                hboxSlider->addWidget(brightnessSlider);
                hboxSlider->addWidget(brightnessSliderValue);
                verticalLayout->addLayout(hboxSlider);
            }
        }

        // DPI slider
        if(currentDevice->hasCapability("dpi")) {
            QLabel *dpiLabel = new QLabel("DPI");
            QSlider *dpiXSlider = new QSlider(Qt::Horizontal, widget);
            dpiXSlider->setObjectName("dpiX");
            QSlider *dpiYSlider = new QSlider(Qt::Horizontal, widget);
            dpiYSlider->setObjectName("dpiY");
            QList<int> currDPI = currentDevice->getDPI();
            dpiXSlider->setValue(currDPI[0]);
            dpiYSlider->setValue(currDPI[1]);
            connect(dpiXSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::dpiChanged);
        }

        /* Spacer to bottom */
        QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        verticalLayout->addItem(spacer);

        /* Serial and firmware version labels */
        QLabel *serialLabel = new QLabel("Serial number: " + serial);
        verticalLayout->addWidget(serialLabel);

        QLabel *fwVerLabel = new QLabel("Firmware version: " + currentDevice->getFirmwareVersion()); //TODO
        verticalLayout->addWidget(fwVerLabel);

        /* Create actual PageWidgetItem */
        RazerPageWidgetItem *item = new RazerPageWidgetItem(widget, name, serial);

        // Set icon (only works the second time the application is opened due to the images being downloaded the first time. TODO: Find solution
        if(!currentDevice->getPngFilename().isEmpty()) {
            //std::cout << currentDevice->getPngFilename().toStdString() << std::endl;
            QIcon *icon = new QIcon(RazerImageDownloader::getDownloadPath() + currentDevice->getPngFilename());
            item->setIcon(*icon);
        }
        item->setHeader(name);

        ui_main.kpagewidget->addPage(item);
    }
}

void kcm_razerdrivers::imageDownloaded(QString &serial, QString &filename)
{
    //TODO: Set image
    std::cout << "Download of image completed for " << serial.toStdString() << " at " << filename.toStdString() << std::endl;
}

void kcm_razerdrivers::toggleSync(bool sync)
{
    if(!librazer::syncEffects(sync))
        showError("Error while syncing devices.");
}

void kcm_razerdrivers::toggleOffOnScreesaver(bool on)
{
    if(!librazer::setTurnOffOnScreensaver(on))
        showError("Error while toggling 'turn off on screensaver'");
}

void kcm_razerdrivers::colorButtonClicked()
{
    std::cout << "color dialog" << std::endl;

    QPushButton *sender = qobject_cast<QPushButton*>(QObject::sender());
    std::cout << sender->objectName().toStdString() << std::endl;

    QPalette pal(sender->palette());

    QColor oldColor = pal.color(QPalette::Button);

    QColor color = QColorDialog::getColor(oldColor);
    if(color.isValid()) {
        std::cout << color.name().toStdString() << std::endl;
        pal.setColor(QPalette::Button, color);
        sender->setPalette(pal);
    } else {
        std::cout << "User cancelled the dialog." << std::endl;
    }
}
/*
void kcm_razerdrivers::logoColorButton()
{
    std::cout << "color dialog" << std::endl;

    QColor color = QColorDialog::getColor(Qt::white);
    std::cout << color.name().toStdString() << std::endl;
}

void kcm_razerdrivers::scrollColorButton()
{
    std::cout << "color dialog" << std::endl;

    QColor color = QColorDialog::getColor(Qt::white);
    std::cout << color.name().toStdString() << std::endl;
}
*/

QPair<librazer::Device*, QString> kcm_razerdrivers::commonCombo(int index)
{
    QComboBox *sender = qobject_cast<QComboBox*>(QObject::sender());
    librazer::RazerCapability capability = sender->itemData(index).value<librazer::RazerCapability>();
    QString identifier = capability.getIdentifier();

    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui_main.kpagewidget->currentPage());
    librazer::Device *dev = devices.value(item->getSerial());

    if(capability.getNumColors() == 0) { // hide all
        for(int i=1; i<=3; i++)
            item->widget()->findChild<QPushButton*>("colorbutton" + QString::number(i))->hide();
    } else {
        for(int i=1; i<=3; i++) {
            if(capability.getNumColors() < i)
                item->widget()->findChild<QPushButton*>("colorbutton" + QString::number(i))->hide();
            else
                item->widget()->findChild<QPushButton*>("colorbutton" + QString::number(i))->show();
        }
    }

    return qMakePair(dev, identifier);
}

void kcm_razerdrivers::standardCombo(int index)
{
    QPair<librazer::Device*, QString> tuple = commonCombo(index);
    librazer::Device *dev = tuple.first;
    QString identifier = tuple.second;

    if(identifier == "lighting_breath_single") {
        QColor c = getColorForButton(1);
        dev->setBreathSingle(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_breath_dual") {
        QColor c1 = getColorForButton(1);
        QColor c2 = getColorForButton(2);
        dev->setBreathDual(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue());
    } else if(identifier == "lighting_breath_triple") {
        QColor c1 = getColorForButton(1);
        QColor c2 = getColorForButton(2);
        QColor c3 = getColorForButton(3);
        dev->setBreathTriple(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue(), c3.red(), c3.green(), c3.blue());
    } else if(identifier == "lighting_breath_random") {
        dev->setBreathRandom();
    } else if(identifier == "lighting_wave") {
        dev->setWave(librazer::WAVE_RIGHT); // TODO Left/right button
    } else if(identifier == "lighting_reactive") {
        QColor c = getColorForButton(1);
        dev->setReactive(c.red(), c.green(), c.blue(), librazer::REACTIVE_500MS); // TODO Configure speed?
    } else if(identifier == "lighting_none") {
        dev->setNone();
    } else if(identifier == "lighting_spectrum") {
        dev->setSpectrum();
    } else if(identifier == "lighting_static") {
        QColor c = getColorForButton(1);
        dev->setStatic(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_ripple") {
        QColor c = getColorForButton(1);
        dev->setRipple(c.red(), c.green(), c.blue(), librazer::RIPPLE_REFRESH_RATE); //TODO Configure refreshrate?
    } else if(identifier == "lighting_ripple_random") {
        dev->setRippleRandomColor(librazer::RIPPLE_REFRESH_RATE); //TODO Configure refreshrate?
    } else if(identifier == "lighting_pulsate") {
        dev->setPulsate();
    } else {
        std::cout << identifier.toStdString() << " is not implemented yet!" << std::endl;
    }
}

void kcm_razerdrivers::scrollCombo(int index)
{
    QPair<librazer::Device*, QString> tuple = commonCombo(index);
    librazer::Device *dev = tuple.first;
    QString identifier = tuple.second;

    qDebug() << tuple;

    if(identifier == "lighting_scroll_blinking") {
        QColor c = getColorForButton(1);
        dev->setScrollBlinking(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_pulsate") {
        QColor c = getColorForButton(1);
        dev->setScrollPulsate(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_spectrum") {
        dev->setScrollSpectrum();
    } else if(identifier == "lighting_scroll_static") {
        QColor c = getColorForButton(1);
        dev->setScrollStatic(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_none") {
        dev->setScrollNone();
    } else if(identifier == "lighting_scroll_reactive") {
        QColor c = getColorForButton(1);
        dev->setScrollReactive(c.red(), c.green(), c.blue(), librazer::REACTIVE_500MS); // TODO Configure speed?
    } else if(identifier == "lighting_scroll_breath_single") {
        QColor c = getColorForButton(1);
        dev->setScrollBreathSingle(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_breath_dual") {
        QColor c1 = getColorForButton(1);
        QColor c2 = getColorForButton(2);
        dev->setScrollBreathDual(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue());
    } else if(identifier == "lighting_scroll_breath_random") {
        dev->setScrollBreathRandom();
    } else {
        std::cout << identifier.toStdString() << " is not implemented yet!" << std::endl;
    }
}

void kcm_razerdrivers::logoCombo(int index)
{
    QPair<librazer::Device*, QString> tuple = commonCombo(index);
    librazer::Device *dev = tuple.first;
    QString identifier = tuple.second;

    if(identifier == "lighting_logo_blinking") {
        QColor c = getColorForButton(1);
        dev->setLogoBlinking(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_pulsate") {
        QColor c = getColorForButton(1);
        dev->setLogoPulsate(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_spectrum") {
        dev->setLogoSpectrum();
    } else if(identifier == "lighting_logo_static") {
        QColor c = getColorForButton(1);
        dev->setLogoStatic(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_none") {
        dev->setLogoNone();
    } else if(identifier == "lighting_logo_reactive") {
        QColor c = getColorForButton(1);
        dev->setLogoReactive(c.red(), c.green(), c.blue(), librazer::REACTIVE_500MS); // TODO Configure speed?
    } else if(identifier == "lighting_logo_breath_single") {
        QColor c = getColorForButton(1);
        dev->setLogoBreathSingle(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_breath_dual") {
        QColor c1 = getColorForButton(1);
        QColor c2 = getColorForButton(2);
        dev->setLogoBreathDual(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue());
    } else if(identifier == "lighting_logo_breath_random") {
        dev->setLogoBreathRandom();
    } else {
        std::cout << identifier.toStdString() << " is not implemented yet!" << std::endl;
    }
}

QColor kcm_razerdrivers::getColorForButton(int num)
{
    //TODO Probably doesn't work with multiple lighting zones, TEST!
    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui_main.kpagewidget->currentPage());
    QPalette pal = item->widget()->findChild<QPushButton*>("colorbutton" + QString::number(num))->palette();
    return pal.color(QPalette::Button);
}

void kcm_razerdrivers::brightnessChanged(int value)
{
    std::cout << value << std::endl;

    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui_main.kpagewidget->currentPage());
    librazer::Device *dev = devices.value(item->getSerial());
    dev->setBrightness(value);
}

void kcm_razerdrivers::scrollBrightnessChanged(int value)
{
    std::cout << value << std::endl;

    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui_main.kpagewidget->currentPage());
    librazer::Device *dev = devices.value(item->getSerial());
    dev->setScrollBrightness(value);
}

void kcm_razerdrivers::logoBrightnessChanged(int value)
{
    std::cout << value << std::endl;

    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui_main.kpagewidget->currentPage());
    librazer::Device *dev = devices.value(item->getSerial());
    dev->setLogoBrightness(value);
}

void kcm_razerdrivers::dpiChanged(int value)
{
    std::cout << value << std::endl;
}

void kcm_razerdrivers::showError(QString error)
{
    QMessageBox messageBox;
    messageBox.critical(0, "Error!", error);
    messageBox.setFixedSize(500, 200);
}

void kcm_razerdrivers::showInfo(QString info)
{
    QMessageBox messageBox;
    messageBox.information(0, "Information!", info);
    messageBox.setFixedSize(500, 200);
}

#include "kcm_razerdrivers.moc"
