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
        QList<librazer::Device::lightingLocations> lightingLocationsTodo;

        // Check what lighting locations the device has
        if(currentDevice->hasCapability("lighting"))
            lightingLocationsTodo.append(librazer::Device::lighting);
        if(currentDevice->hasCapability("lighting_logo"))
            lightingLocationsTodo.append(librazer::Device::lighting_logo);
        if(currentDevice->hasCapability("lighting_scroll"))
            lightingLocationsTodo.append(librazer::Device::lighting_scroll);

        // Declare header font
        QFont headerFont("Arial", 15, QFont::Bold);

        // Lighting header
        if(lightingLocationsTodo.size() != 0) {
            QLabel *lightingHeader = new QLabel("Lighting", widget);
            lightingHeader->setFont(headerFont);
            verticalLayout->addWidget(lightingHeader);
        }

        // Iterate through lighting locations
        while(lightingLocationsTodo.size() != 0) {
            // Get location we are iterating through
            librazer::Device::lightingLocations currentLocation = lightingLocationsTodo.takeFirst();

            QLabel *lightingLocationLabel;

            // Set appropriate text
            if(currentLocation == librazer::Device::lighting) {
                lightingLocationLabel = new QLabel("Lighting");
            } else if(currentLocation == librazer::Device::lighting_logo) {
                lightingLocationLabel = new QLabel("Lighting Logo");
            } else if(currentLocation == librazer::Device::lighting_scroll) {
                lightingLocationLabel = new QLabel("Lighting Scroll");
            } else {
                // Houston, we have a problem.
            }

            QHBoxLayout *lightingHBox = new QHBoxLayout(widget);
            verticalLayout->addWidget(lightingLocationLabel);
            verticalLayout->addLayout(lightingHBox);

            QComboBox *comboBox = new QComboBox;
            QLabel *brightnessLabel = NULL;
            QSlider *brightnessSlider = NULL;

            comboBox->setObjectName(QString::number(currentLocation));
            //TODO More elegant solution instead of the sizePolicy?
            comboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

            //TODO Speed for reactive
            //TODO Battery
            //TODO Keyboard stuff (dunno what exactly)
            //TODO Sync effects in comboboxes & colorStuff when the sync checkbox is active
            //TODO poll rate
            //TODO Matrix stuff
            //TODO Apply effect color after the color is set

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
                lightingHBox->addWidget(comboBox);

                /* Color buttons */
                for(int i=1; i<=3; i++) {
                    QPushButton *colorButton = new QPushButton(widget);
                    QPalette pal = colorButton->palette();
                    // TODO: Set color when set
                    pal.setColor(QPalette::Button, QColor(Qt::green));
                    colorButton->setAutoFillBackground(true);
                    colorButton->setFlat(true);
                    colorButton->setPalette(pal);
                    colorButton->setMaximumWidth(70);
                    colorButton->setObjectName(QString::number(currentLocation) + "_colorbutton" + QString::number(i));
                    lightingHBox->addWidget(colorButton);

                    librazer::RazerCapability capability = comboBox->currentData().value<librazer::RazerCapability>();
                    if(capability.getNumColors() < i)
                        colorButton->hide();
                    connect(colorButton, &QPushButton::clicked, this, &kcm_razerdrivers::colorButtonClicked);
                }

                /* Wave left/right radio buttons */
                for(int i=1; i<=2; i++) {
                    QString name;
                    if(i==1)
                        name = "Left";
                    else
                        name = "Right";
                    QRadioButton *radio = new QRadioButton(name, widget);
                    radio->setObjectName(QString::number(currentLocation) + "_radiobutton" + QString::number(i));
//                     radio->hide();
                    lightingHBox->addWidget(radio);
                }
            }

            // 'Set Logo Active' checkbox
            //TODO New location for the checkbox?
            if(currentLocation == librazer::Device::lighting_logo) {
                // Show if the device has 'setActive' but not 'setNone' as it would be basically a duplicate action
                if(currentDevice->hasCapability("lighting_logo_active") && !currentDevice->hasCapability("lighting_logo_none")) {
                    QCheckBox *activeCheckbox = new QCheckBox("Set Logo Active", widget);
                    activeCheckbox->setChecked(currentDevice->getLogoActive());
                    verticalLayout->addWidget(activeCheckbox);
                    connect(activeCheckbox, &QCheckBox::clicked, this, &kcm_razerdrivers::activeCheckbox);
                }
            }
            //TODO setScrollActive checkbox, probably revamp 'if' above (is there a .chroma setActive?)

            /* Brightness sliders */
            if(brightnessLabel != NULL && brightnessSlider != NULL) { // only if brightness capability exists
                verticalLayout->addWidget(brightnessLabel);
                QHBoxLayout *hboxSlider = new QHBoxLayout(widget);
                QLabel *brightnessSliderValue = new QLabel;
                hboxSlider->addWidget(brightnessSlider);
                hboxSlider->addWidget(brightnessSliderValue);
                verticalLayout->addLayout(hboxSlider);
            }
        }

        // DPI slider
        if(currentDevice->hasCapability("dpi")) {
            // HBoxes
            QHBoxLayout *dpiXHBox = new QHBoxLayout(widget);
            QHBoxLayout *dpiYHBox = new QHBoxLayout(widget);
            QHBoxLayout *dpiHeaderHBox = new QHBoxLayout(widget);

            // Header
            QLabel *dpiHeader = new QLabel("DPI", widget);
            dpiHeader->setFont(headerFont);
            dpiHeaderHBox->addWidget(dpiHeader);

            verticalLayout->addLayout(dpiHeaderHBox);

            // Labels
            QLabel *dpiXLabel = new QLabel("DPI X");
            QLabel *dpiYLabel = new QLabel("DPI Y");

            // Read-only textboxes
            QTextEdit *dpiXText = new QTextEdit(widget);
            QTextEdit *dpiYText = new QTextEdit(widget);
            dpiXText->setMaximumWidth(60);
            dpiYText->setMaximumWidth(60);
            dpiXText->setMaximumHeight(30);
            dpiYText->setMaximumHeight(30);
            dpiXText->setObjectName("dpiXText");
            dpiYText->setObjectName("dpiYText");
            dpiXText->setEnabled(false);
            dpiYText->setEnabled(false);

            // Sliders
            QSlider *dpiXSlider = new QSlider(Qt::Horizontal, widget);
            QSlider *dpiYSlider = new QSlider(Qt::Horizontal, widget);
            dpiXSlider->setObjectName("dpiX");
            dpiYSlider->setObjectName("dpiY");

            // Sync checkbox
            QLabel *dpiSyncLabel = new QLabel("Lock X/Y", widget);
            QCheckBox *dpiSyncCheckbox = new QCheckBox(widget);

            // Get the current DPI and set the slider&text
            QList<int> currDPI = currentDevice->getDPI();
            dpiXSlider->setValue(currDPI[0]/100);
            dpiYSlider->setValue(currDPI[1]/100);
            dpiXText->setText(QString::number(currDPI[0]));
            dpiYText->setText(QString::number(currDPI[1]));

            //TODO Do a dynamic max? Needs daemon support for max or test it out by setting to 50000 and then get the value. Would that work?
            dpiXSlider->setMaximum(160);
            dpiYSlider->setMaximum(160);

            dpiXSlider->setTickInterval(10);
            dpiYSlider->setTickInterval(10);
            dpiXSlider->setTickPosition(QSlider::TickPosition::TicksBelow);
            dpiYSlider->setTickPosition(QSlider::TickPosition::TicksBelow);

            dpiSyncCheckbox->setChecked(syncDpi); // set enabled by default

            dpiXHBox->addWidget(dpiXLabel);
            dpiXHBox->addWidget(dpiXText);
            dpiXHBox->addWidget(dpiXSlider);

            dpiYHBox->addWidget(dpiYLabel);
            dpiYHBox->addWidget(dpiYText);
            dpiYHBox->addWidget(dpiYSlider);

            dpiHeaderHBox->addWidget(dpiSyncLabel);
            dpiHeaderHBox->addWidget(dpiSyncCheckbox);
            // TODO Better solution/location for 'Sync' checkbox
            dpiHeaderHBox->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

            connect(dpiXSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::dpiChanged);
            connect(dpiYSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::dpiChanged);
            connect(dpiSyncCheckbox, &QCheckBox::clicked, this, &kcm_razerdrivers::dpiSyncCheckbox);

            verticalLayout->addLayout(dpiXHBox);
            verticalLayout->addLayout(dpiYHBox);
        }

        /* Spacer to bottom */
        QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        verticalLayout->addItem(spacer);

        /* Serial and firmware version labels */
        QLabel *serialLabel = new QLabel("Serial number: " + serial);
        verticalLayout->addWidget(serialLabel);

        QLabel *fwVerLabel = new QLabel("Firmware version: " + currentDevice->getFirmwareVersion());
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

    if(serialnrs.size() == 0) {
        showError("The daemon doesn't see any devices. Make sure they are connected!");
    }
}

void kcm_razerdrivers::imageDownloaded(QString &serial, QString &filename)
{
    //TODO: Set image at runtime
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

QPair<librazer::Device*, QString> kcm_razerdrivers::commonCombo(int index)
{
    QComboBox *sender = qobject_cast<QComboBox*>(QObject::sender());
    librazer::RazerCapability capability = sender->itemData(index).value<librazer::RazerCapability>();
    QString identifier = capability.getIdentifier();

    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui_main.kpagewidget->currentPage());
    librazer::Device *dev = devices.value(item->getSerial());

    if(capability.getNumColors() == 0) { // hide all
        for(int i=1; i<=3; i++)
            item->widget()->findChild<QPushButton*>(sender->objectName() + "_colorbutton" + QString::number(i))->hide();
    } else {
        for(int i=1; i<=3; i++) {
            if(capability.getNumColors() < i)
                item->widget()->findChild<QPushButton*>(sender->objectName() + "_colorbutton" + QString::number(i))->hide();
            else
                item->widget()->findChild<QPushButton*>(sender->objectName() + "_colorbutton" + QString::number(i))->show();
        }
    }

    return qMakePair(dev, identifier);
}

void kcm_razerdrivers::standardCombo(int index)
{
    QPair<librazer::Device*, QString> tuple = commonCombo(index);
    librazer::Device *dev = tuple.first;
    QString identifier = tuple.second;

    qDebug() << tuple;

    librazer::Device::lightingLocations zone = librazer::Device::lightingLocations::lighting;

    if(identifier == "lighting_breath_single") {
        QColor c = getColorForButton(1, zone);
        dev->setBreathSingle(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_breath_dual") {
        QColor c1 = getColorForButton(1, zone);
        QColor c2 = getColorForButton(2, zone);
        dev->setBreathDual(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue());
    } else if(identifier == "lighting_breath_triple") {
        QColor c1 = getColorForButton(1, zone);
        QColor c2 = getColorForButton(2, zone);
        QColor c3 = getColorForButton(3, zone);
        dev->setBreathTriple(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue(), c3.red(), c3.green(), c3.blue());
    } else if(identifier == "lighting_breath_random") {
        dev->setBreathRandom();
    } else if(identifier == "lighting_wave") {
        dev->setWave(librazer::WAVE_RIGHT); // TODO Left/right button
    } else if(identifier == "lighting_reactive") {
        QColor c = getColorForButton(1, zone);
        dev->setReactive(c.red(), c.green(), c.blue(), librazer::REACTIVE_500MS); // TODO Configure speed?
    } else if(identifier == "lighting_none") {
        dev->setNone();
    } else if(identifier == "lighting_spectrum") {
        dev->setSpectrum();
    } else if(identifier == "lighting_static") {
        QColor c = getColorForButton(1, zone);
        dev->setStatic(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_ripple") {
        QColor c = getColorForButton(1, zone);
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

    librazer::Device::lightingLocations zone = librazer::Device::lightingLocations::lighting_scroll;

    if(identifier == "lighting_scroll_blinking") {
        QColor c = getColorForButton(1, zone);
        dev->setScrollBlinking(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_pulsate") {
        QColor c = getColorForButton(1, zone);
        dev->setScrollPulsate(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_spectrum") {
        dev->setScrollSpectrum();
    } else if(identifier == "lighting_scroll_static") {
        QColor c = getColorForButton(1, zone);
        dev->setScrollStatic(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_none") {
        dev->setScrollNone();
    } else if(identifier == "lighting_scroll_reactive") {
        QColor c = getColorForButton(1, zone);
        dev->setScrollReactive(c.red(), c.green(), c.blue(), librazer::REACTIVE_500MS); // TODO Configure speed?
    } else if(identifier == "lighting_scroll_breath_single") {
        QColor c = getColorForButton(1, zone);
        dev->setScrollBreathSingle(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_breath_dual") {
        QColor c1 = getColorForButton(1, zone);
        QColor c2 = getColorForButton(2, zone);
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

    qDebug() << tuple;

    librazer::Device::lightingLocations zone = librazer::Device::lightingLocations::lighting_logo;

    if(identifier == "lighting_logo_blinking") {
        QColor c = getColorForButton(1, zone);
        dev->setLogoBlinking(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_pulsate") {
        QColor c = getColorForButton(1, zone);
        dev->setLogoPulsate(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_spectrum") {
        dev->setLogoSpectrum();
    } else if(identifier == "lighting_logo_static") {
        QColor c = getColorForButton(1, zone);
        dev->setLogoStatic(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_none") {
        dev->setLogoNone();
    } else if(identifier == "lighting_logo_reactive") {
        QColor c = getColorForButton(1, zone);
        dev->setLogoReactive(c.red(), c.green(), c.blue(), librazer::REACTIVE_500MS); // TODO Configure speed?
    } else if(identifier == "lighting_logo_breath_single") {
        QColor c = getColorForButton(1, zone);
        dev->setLogoBreathSingle(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_breath_dual") {
        QColor c1 = getColorForButton(1, zone);
        QColor c2 = getColorForButton(2, zone);
        dev->setLogoBreathDual(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue());
    } else if(identifier == "lighting_logo_breath_random") {
        dev->setLogoBreathRandom();
    } else {
        std::cout << identifier.toStdString() << " is not implemented yet!" << std::endl;
    }
}

QColor kcm_razerdrivers::getColorForButton(int num, librazer::Device::lightingLocations location)
{
    //TODO Probably doesn't work with multiple lighting zones, TEST!
    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui_main.kpagewidget->currentPage());
    QPalette pal = item->widget()->findChild<QPushButton*>(QString::number(location) + "_colorbutton" + QString::number(num))->palette();
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

void kcm_razerdrivers::dpiChanged(int orig_value)
{
    int value = orig_value*100;

    QSlider *sender = qobject_cast<QSlider*>(QObject::sender());

    qDebug() << value;
    qDebug() << sender->objectName();

    // if DPI should be synced
    if(syncDpi) {
        if(sender->objectName() == "dpiX") {
            // set the other slider
            QSlider *slider = sender->parentWidget()->findChild<QSlider*>("dpiY");
            slider->setValue(orig_value);

            // get device pointer
            RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui_main.kpagewidget->currentPage());
            librazer::Device *dev = devices.value(item->getSerial());
            // set DPI
            dev->setDPI(value, value); // set for both X & Y
        } else {
            // just set the slider (as the rest was done already or will be done)
            QSlider *slider = sender->parentWidget()->findChild<QSlider*>("dpiX");
            slider->setValue(orig_value);
        }
    } /* if DPI should NOT be synced */ else {
        // get device pointer
        RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui_main.kpagewidget->currentPage());
        librazer::Device *dev = devices.value(item->getSerial());

        // set DPI (with value from other slider)
        if(sender->objectName() == "dpiX") {
            QSlider *slider = sender->parentWidget()->findChild<QSlider*>("dpiY");
            dev->setDPI(value, slider->value()*100);
        } else {
            QSlider *slider = sender->parentWidget()->findChild<QSlider*>("dpiX");
            dev->setDPI(slider->value()*100, value);
        }
    }
    // Update textbox with new value
    QTextEdit *dpitextbox = sender->parentWidget()->findChild<QTextEdit*>(sender->objectName() + "Text");
    dpitextbox->setText(QString::number(value));
}

void kcm_razerdrivers::dpiSyncCheckbox(bool checked)
{
    // TODO Sync DPI here? Or just at next change (current behaviour)?
    syncDpi = checked;
}

void kcm_razerdrivers::activeCheckbox(bool checked)
{
    // get device pointer
    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui_main.kpagewidget->currentPage());
    librazer::Device *dev = devices.value(item->getSerial());

    dev->setLogoActive(checked);
    qDebug() << checked;
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
