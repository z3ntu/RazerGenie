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

#include "razergenie.h"
#include "librazer/librazer.h"
#include "librazer/razercapability.h"
#include "razerimagedownloader.h"
#include "razerdevicewidget.h"
#include "customeditor.h"

RazerGenie::RazerGenie(QWidget *parent) : QWidget(parent)
{
    // Watch for dbus service changes (= daemon ends or gets started)
    QDBusServiceWatcher *watcher = new QDBusServiceWatcher("org.razer", QDBusConnection::sessionBus());
    connect(watcher, &QDBusServiceWatcher::serviceRegistered,
            this, &RazerGenie::dbusServiceRegistered);
    connect(watcher, &QDBusServiceWatcher::serviceUnregistered,
            this, &RazerGenie::dbusServiceUnregistered);

    // Check if daemon available
    if(!librazer::isDaemonRunning()) {
        setupErrorUi();
    } else {
        setupUi();
    }
}

RazerGenie::~RazerGenie()
{
//    delete ui;
}

void RazerGenie::setupErrorUi()
{
    qDebug() << "DAEMON IS NOT RUNNING. ABORTING!";
    ui_error.setupUi(this);
}

void RazerGenie::setupUi()
{
    ui_main.setupUi(this);

    ui_main.versionLabel->setText("Daemon version: " + librazer::getDaemonVersion());

    fillList();

    //Connect signals
    connect(ui_main.syncCheckBox, &QCheckBox::clicked, this, &RazerGenie::toggleSync);
    ui_main.syncCheckBox->setChecked(librazer::getSyncEffects());
    connect(ui_main.screensaverCheckBox, &QCheckBox::clicked, this, &RazerGenie::toggleOffOnScreesaver);
    ui_main.screensaverCheckBox->setChecked(librazer::getTurnOffOnScreensaver());

    librazer::connectDeviceAdded(this, SLOT(deviceAdded()));
    librazer::connectDeviceRemoved(this, SLOT(deviceRemoved()));
}

void RazerGenie::dbusServiceRegistered(const QString &serviceName)
{
    qDebug() << "Registered! " << serviceName;
    showInfo("Please restart the application to see the interface for now.");
//     setupUi();
}

void RazerGenie::dbusServiceUnregistered(const QString &serviceName)
{
    qDebug() << "Unregistered! " << serviceName;
    showError("The dbus service connection was lost. Please restart the daemon (\"razer-service\")");
}

void RazerGenie::fillList()
{
    //TODO FIX THIS!!!
    // Remove all widgets from the stackedWidget
    for(int i = ui_main.stackedWidget->count(); i >= 0; i--) {
        QWidget* widget = ui_main.stackedWidget->widget(i);
        ui_main.stackedWidget->removeWidget(widget);
        widget->deleteLater();
    }

    // Get all connected devices
    QStringList serialnrs = librazer::getConnectedDevices();

    // Iterate through all devices
    foreach (const QString &serial, serialnrs) {

        // Create device instance with current serial
        librazer::Device *currentDevice = new librazer::Device(serial);

        // Download image for device
        if(!currentDevice->getPngFilename().isEmpty()) {
            RazerImageDownloader *dl = new RazerImageDownloader(serial, QUrl(currentDevice->getPngUrl()), this);
            connect(dl, &RazerImageDownloader::downloadFinished, this, &RazerGenie::imageDownloaded);
        } else {
            qDebug() << ".png mapping for device '" + currentDevice->getDeviceName() + "' (PID "+QString::number(currentDevice->getPid())+") missing.";
        }

        // Setup variables for easy access
        QString type = currentDevice->getDeviceType();
        QString name = currentDevice->getDeviceName();

        qDebug() << serial;
        qDebug() << name;

        // Insert current device pointer with serial lookup into a QHash
        devices.insert(serial, currentDevice);

        // Types known for now: headset, mouse, mug, keyboard, tartarus, core, orbweaver
        qDebug() << type;

        /* Create actual PageWidgetItem */
        RazerDeviceWidget *widget = new RazerDeviceWidget(name, serial);

        QVBoxLayout *verticalLayout = new QVBoxLayout(widget);

        // List of locations to iterate through
        QList<librazer::Device::lightingLocation> lightingLocationsTodo;

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
            librazer::Device::lightingLocation currentLocation = lightingLocationsTodo.takeFirst();

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
                showError("Unhanded lighting location in fillList()");
            }

            QHBoxLayout *lightingHBox = new QHBoxLayout(widget);
            verticalLayout->addWidget(lightingLocationLabel);
            verticalLayout->addLayout(lightingHBox);

            QComboBox *comboBox = new QComboBox;
            QLabel *brightnessLabel = NULL;
            QSlider *brightnessSlider = NULL;

            comboBox->setObjectName(QString::number(currentLocation));
            qDebug() << "CURRENT LOCATION: " << QString::number(currentLocation);
            //TODO More elegant solution instead of the sizePolicy?
            comboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

            //TODO Speed for reactive
            //TODO Battery
            //TODO Keyboard stuff (dunno what exactly)
            //TODO Sync effects in comboboxes & colorStuff when the sync checkbox is active
            //TODO Matrix stuff

            if(currentLocation == librazer::Device::lighting) {
                // Add items from capabilities
                for(int i=0; i<librazer::lightingComboBoxCapabilites.size(); i++) {
                    if(currentDevice->hasCapability(librazer::lightingComboBoxCapabilites[i].getIdentifier())) {
                        comboBox->addItem(librazer::lightingComboBoxCapabilites[i].getDisplayString(), QVariant::fromValue(librazer::lightingComboBoxCapabilites[i]));
                    }
                }

                // Connect signal from combobox
                connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &RazerGenie::standardCombo);

                // Brightness slider
                if(currentDevice->hasCapability("brightness")) {
                    brightnessLabel = new QLabel("Brightness");
                    brightnessSlider = new QSlider(Qt::Horizontal, widget);
                    if(currentDevice->hasCapability("get_brightness")) {
                        brightnessSlider->setValue(currentDevice->getBrightness());
                    }
                    connect(brightnessSlider, &QSlider::valueChanged, this, &RazerGenie::brightnessChanged);
                }

            } else if(currentLocation == librazer::Device::lighting_logo) {
                // Add items from capabilities
                for(int i=0; i<librazer::logoComboBoxCapabilites.size(); i++) {
                    if(currentDevice->hasCapability(librazer::logoComboBoxCapabilites[i].getIdentifier())) {
                        comboBox->addItem(librazer::logoComboBoxCapabilites[i].getDisplayString(), QVariant::fromValue(librazer::logoComboBoxCapabilites[i]));
                    }
                }

                // Connect signal from combobox
                connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &RazerGenie::logoCombo);

                // Brightness slider
                if(currentDevice->hasCapability("lighting_logo_brightness")) {
                    brightnessLabel = new QLabel("Brightness Logo");
                    brightnessSlider = new QSlider(Qt::Horizontal, widget);
                    if(currentDevice->hasCapability("get_lighting_logo_brightness")) {
                        brightnessSlider->setValue(currentDevice->getLogoBrightness());
                    }
                    connect(brightnessSlider, &QSlider::valueChanged, this, &RazerGenie::logoBrightnessChanged);
                }

            } else if(currentLocation == librazer::Device::lighting_scroll) {
                // Add items from capabilities
                for(int i=0; i<librazer::scrollComboBoxCapabilites.size(); i++) {
                    if(currentDevice->hasCapability(librazer::scrollComboBoxCapabilites[i].getIdentifier())) {
                        comboBox->addItem(librazer::scrollComboBoxCapabilites[i].getDisplayString(), QVariant::fromValue(librazer::scrollComboBoxCapabilites[i]));
                    }
                }

                // Connect signal from combobox
                connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &RazerGenie::scrollCombo);

                // Brightness slider
                if(currentDevice->hasCapability("lighting_scroll_brightness")) {
                    brightnessLabel = new QLabel("Brightness Scroll");
                    brightnessSlider = new QSlider(Qt::Horizontal, widget);
                    if(currentDevice->hasCapability("get_lighting_scroll_brightness")) {
                        brightnessSlider->setValue(currentDevice->getScrollBrightness());
                    }
                    connect(brightnessSlider, &QSlider::valueChanged, this, &RazerGenie::scrollBrightnessChanged);
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
                    connect(colorButton, &QPushButton::clicked, this, &RazerGenie::colorButtonClicked);
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
                    if(i==1) // set the 'left' checkbox to activated
                        radio->setChecked(true);
                    // hide by default
                    radio->hide();
                    lightingHBox->addWidget(radio);
                    if(currentLocation == librazer::Device::lightingLocation::lighting) {
                        connect(radio, &QRadioButton::toggled, this, &RazerGenie::waveRadioButtonStandard);
                    } else if(currentLocation == librazer::Device::lightingLocation::lighting_logo) {
                        connect(radio, &QRadioButton::toggled, this, &RazerGenie::waveRadioButtonLogo);
                    } else if(currentLocation == librazer::Device::lightingLocation::lighting_scroll) {
                        connect(radio, &QRadioButton::toggled, this, &RazerGenie::waveRadioButtonScroll);
                    } else {
                        qDebug() << "ERROR!! New lightingLocation which is not handled with the radio buttons.";
                    }
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
                    connect(activeCheckbox, &QCheckBox::clicked, this, &RazerGenie::logoActiveCheckbox);
                }
            }

            // 'Set Scroll Active' checkbox
            if(currentLocation == librazer::Device::lighting_scroll) {
                // Show if the device has 'setActive' but not 'setNone' as it would be basically a duplicate action
                if(currentDevice->hasCapability("lighting_scroll_active") && !currentDevice->hasCapability("lighting_scroll_none")) {
                    QCheckBox *activeCheckbox = new QCheckBox("Set Scroll Active", widget);
                    activeCheckbox->setChecked(currentDevice->getScrollActive());
                    verticalLayout->addWidget(activeCheckbox);
                    connect(activeCheckbox, &QCheckBox::clicked, this, &RazerGenie::scrollActiveCheckbox);
                }
            }

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

        /* DPI sliders */
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
            qDebug() << "currDPI:" << currDPI;
            dpiXSlider->setValue(currDPI[0]/100);
            dpiYSlider->setValue(currDPI[1]/100);
            dpiXText->setText(QString::number(currDPI[0]));
            dpiYText->setText(QString::number(currDPI[1]));

            int maxDPI = currentDevice->maxDPI();
            qDebug() << "maxDPI:" << maxDPI;
            dpiXSlider->setMaximum(maxDPI/100);
            dpiYSlider->setMaximum(maxDPI/100);

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

            dpiHeaderHBox->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
            dpiHeaderHBox->addWidget(dpiSyncLabel);
            // TODO Better solution/location for 'Sync' checkbox
            dpiHeaderHBox->addWidget(dpiSyncCheckbox);

            connect(dpiXSlider, &QSlider::valueChanged, this, &RazerGenie::dpiChanged);
            connect(dpiYSlider, &QSlider::valueChanged, this, &RazerGenie::dpiChanged);
            connect(dpiSyncCheckbox, &QCheckBox::clicked, this, &RazerGenie::dpiSyncCheckbox);

            verticalLayout->addLayout(dpiXHBox);
            verticalLayout->addLayout(dpiYHBox);
        }

        /* Poll rate */
        if(currentDevice->hasCapability("poll_rate")) {
            qDebug() << "ADD POLLING RATE COMBOBOXES!";

            QLabel *pollRateHeader = new QLabel("Polling rate", widget);
            pollRateHeader->setFont(headerFont);
            verticalLayout->addWidget(pollRateHeader);

            QComboBox *pollComboBox = new QComboBox;
            pollComboBox->addItem("125 Hz", librazer::POLL_125HZ);
            pollComboBox->addItem("500 Hz", librazer::POLL_500HZ);
            pollComboBox->addItem("1000 Hz", librazer::POLL_1000HZ);
            pollComboBox->setCurrentText(QString::number(currentDevice->getPollRate()) + " Hz");
            verticalLayout->addWidget(pollComboBox);

            connect(pollComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &RazerGenie::pollCombo);
        }

#ifdef ENABLE_EXPERIMENTAL
        /* Custom lighting */
        if(currentDevice->hasCapability("lighting_led_matrix")) {
            QPushButton *button = new QPushButton(widget);
            button->setText("Open custom editor (unfinished right now)"); // TODO Finish custom editor
            verticalLayout->addWidget(button);
            connect(button, &QPushButton::clicked, this, &RazerGenie::openCustomEditor);
        }
#endif

        /* Spacer to bottom */
        QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        verticalLayout->addItem(spacer);

        /* Serial and firmware version labels */
        QLabel *serialLabel = new QLabel("Serial number: " + serial);
        verticalLayout->addWidget(serialLabel);

        QLabel *fwVerLabel = new QLabel("Firmware version: " + currentDevice->getFirmwareVersion());
        verticalLayout->addWidget(fwVerLabel);

        // Set icon (only works the second time the application is opened due to the images being downloaded the first time. TODO: Find solution
        if(!currentDevice->getPngFilename().isEmpty()) {
            QIcon *icon = new QIcon(RazerImageDownloader::getDownloadPath() + currentDevice->getPngFilename());
//             item->setIcon(*icon);
        }
//         item->setHeader(name);

        ui_main.stackedWidget->addWidget(widget);
        qDebug() << "Stacked widget count:" << ui_main.stackedWidget->count();
    }

    if(serialnrs.size() == 0) {
        showError("The daemon doesn't see any devices. Make sure they are connected!");
    }
}

void RazerGenie::imageDownloaded(QString &serial, QString &filename)
{
    //TODO: Set image at runtime
    qDebug() << "Download of image completed for " << serial << " at " << filename;
}

void RazerGenie::toggleSync(bool sync)
{
    if(!librazer::syncEffects(sync))
        showError("Error while syncing devices.");
}

void RazerGenie::toggleOffOnScreesaver(bool on)
{
    if(!librazer::setTurnOffOnScreensaver(on))
        showError("Error while toggling 'turn off on screensaver'");
}

void RazerGenie::colorButtonClicked()
{
    qDebug() << "color dialog";

    QPushButton *sender = qobject_cast<QPushButton*>(QObject::sender());
    qDebug() << sender->objectName();

    QPalette pal(sender->palette());

    QColor oldColor = pal.color(QPalette::Button);

    QColor color = QColorDialog::getColor(oldColor);
    if(color.isValid()) {
        qDebug() << color.name();
        pal.setColor(QPalette::Button, color);
        sender->setPalette(pal);
    } else {
        qDebug() << "User cancelled the dialog.";
    }
    // objectName is location(int)_colorbuttonNR(1-3)
    // TODO: We shouldn't assume the world to be perfect!
    applyEffect(static_cast<librazer::Device::lightingLocation>(sender->objectName().split("_")[0].toInt()));
}

QPair<librazer::Device*, QString> RazerGenie::commonCombo(int index)
{
    QComboBox *sender = qobject_cast<QComboBox*>(QObject::sender());
    librazer::RazerCapability capability = sender->itemData(index).value<librazer::RazerCapability>();
    QString identifier = capability.getIdentifier();

    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    librazer::Device *dev = devices.value(item->getSerial());

    // Show/hide the color buttons
    if(capability.getNumColors() == 0) { // hide all
        for(int i=1; i<=3; i++)
            item->findChild<QPushButton*>(sender->objectName() + "_colorbutton" + QString::number(i))->hide();
    } else {
        for(int i=1; i<=3; i++) {
            if(capability.getNumColors() < i)
                item->findChild<QPushButton*>(sender->objectName() + "_colorbutton" + QString::number(i))->hide();
            else
                item->findChild<QPushButton*>(sender->objectName() + "_colorbutton" + QString::number(i))->show();
        }
    }

    // Show/hide the wave radiobuttons
    if(capability.isWave() == 0) {
        item->findChild<QRadioButton*>(sender->objectName() + "_radiobutton1")->hide();
        item->findChild<QRadioButton*>(sender->objectName() + "_radiobutton2")->hide();
    } else {
        item->findChild<QRadioButton*>(sender->objectName() + "_radiobutton1")->show();
        item->findChild<QRadioButton*>(sender->objectName() + "_radiobutton2")->show();
    }

    return qMakePair(dev, identifier);
}

void RazerGenie::standardCombo(int index)
{
    QPair<librazer::Device*, QString> tuple = commonCombo(index);
    librazer::Device *dev = tuple.first;
    QString identifier = tuple.second;

    qDebug() << tuple;

    applyEffectStandardLoc(identifier, dev);
}

void RazerGenie::scrollCombo(int index)
{
    QPair<librazer::Device*, QString> tuple = commonCombo(index);
    librazer::Device *dev = tuple.first;
    QString identifier = tuple.second;

    qDebug() << tuple;

    applyEffectScrollLoc(identifier, dev);
}

void RazerGenie::logoCombo(int index)
{
    QPair<librazer::Device*, QString> tuple = commonCombo(index);
    librazer::Device *dev = tuple.first;
    QString identifier = tuple.second;

    qDebug() << tuple;

    applyEffectLogoLoc(identifier, dev);
}

QColor RazerGenie::getColorForButton(int num, librazer::Device::lightingLocation location)
{
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    QPalette pal = item->findChild<QPushButton*>(QString::number(location) + "_colorbutton" + QString::number(num))->palette();
    return pal.color(QPalette::Button);
}

const int RazerGenie::getWaveDirection(librazer::Device::lightingLocation location)
{
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());

    return item->findChild<QRadioButton*>(QString::number(location) + "_radiobutton1")->isChecked() ? librazer::WAVE_LEFT : librazer::WAVE_RIGHT;
}

void RazerGenie::brightnessChanged(int value)
{
    qDebug() << value;

    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    librazer::Device *dev = devices.value(item->getSerial());
    dev->setBrightness(value);
}

void RazerGenie::scrollBrightnessChanged(int value)
{
    qDebug() << value;

    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    librazer::Device *dev = devices.value(item->getSerial());
    dev->setScrollBrightness(value);
}

void RazerGenie::logoBrightnessChanged(int value)
{
    qDebug() << value;

    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    librazer::Device *dev = devices.value(item->getSerial());
    dev->setLogoBrightness(value);
}

void RazerGenie::dpiChanged(int orig_value)
{
    int value = orig_value * 100;

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
            RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
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
        RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
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

void RazerGenie::applyEffectStandardLoc(QString identifier, librazer::Device *device)
{
    librazer::Device::lightingLocation zone = librazer::Device::lightingLocation::lighting;

    if(identifier == "lighting_breath_single") {
        QColor c = getColorForButton(1, zone);
        device->setBreathSingle(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_breath_dual") {
        QColor c1 = getColorForButton(1, zone);
        QColor c2 = getColorForButton(2, zone);
        device->setBreathDual(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue());
    } else if(identifier == "lighting_breath_triple") {
        QColor c1 = getColorForButton(1, zone);
        QColor c2 = getColorForButton(2, zone);
        QColor c3 = getColorForButton(3, zone);
        device->setBreathTriple(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue(), c3.red(), c3.green(), c3.blue());
    } else if(identifier == "lighting_breath_random") {
        device->setBreathRandom();
    } else if(identifier == "lighting_wave") {
        device->setWave(getWaveDirection(zone));
    } else if(identifier == "lighting_reactive") {
        QColor c = getColorForButton(1, zone);
        device->setReactive(c.red(), c.green(), c.blue(), librazer::REACTIVE_500MS); // TODO Configure speed?
    } else if(identifier == "lighting_none") {
        device->setNone();
    } else if(identifier == "lighting_spectrum") {
        device->setSpectrum();
    } else if(identifier == "lighting_static") {
        QColor c = getColorForButton(1, zone);
        device->setStatic(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_ripple") {
        QColor c = getColorForButton(1, zone);
        device->setRipple(c.red(), c.green(), c.blue(), librazer::RIPPLE_REFRESH_RATE); //TODO Configure refreshrate?
    } else if(identifier == "lighting_ripple_random") {
        device->setRippleRandomColor(librazer::RIPPLE_REFRESH_RATE); //TODO Configure refreshrate?
    } else if(identifier == "lighting_pulsate") {
        device->setPulsate();
    } else {
        qDebug() << identifier << " is not implemented yet!";
    }
}

void RazerGenie::applyEffectLogoLoc(QString identifier, librazer::Device *device)
{
    librazer::Device::lightingLocation zone = librazer::Device::lightingLocation::lighting_logo;

    if(identifier == "lighting_logo_blinking") {
        QColor c = getColorForButton(1, zone);
        device->setLogoBlinking(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_pulsate") {
        QColor c = getColorForButton(1, zone);
        device->setLogoPulsate(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_spectrum") {
        device->setLogoSpectrum();
    } else if(identifier == "lighting_logo_static") {
        QColor c = getColorForButton(1, zone);
        device->setLogoStatic(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_none") {
        device->setLogoNone();
    } else if(identifier == "lighting_logo_reactive") {
        QColor c = getColorForButton(1, zone);
        device->setLogoReactive(c.red(), c.green(), c.blue(), librazer::REACTIVE_500MS); // TODO Configure speed?
    } else if(identifier == "lighting_logo_breath_single") {
        QColor c = getColorForButton(1, zone);
        device->setLogoBreathSingle(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_logo_breath_dual") {
        QColor c1 = getColorForButton(1, zone);
        QColor c2 = getColorForButton(2, zone);
        device->setLogoBreathDual(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue());
    } else if(identifier == "lighting_logo_breath_random") {
        device->setLogoBreathRandom();
    } else {
        qDebug() << identifier << " is not implemented yet!";
    }
}

void RazerGenie::applyEffectScrollLoc(QString identifier, librazer::Device *device)
{
    librazer::Device::lightingLocation zone = librazer::Device::lightingLocation::lighting_scroll;

    if(identifier == "lighting_scroll_blinking") {
        QColor c = getColorForButton(1, zone);
        device->setScrollBlinking(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_pulsate") {
        QColor c = getColorForButton(1, zone);
        device->setScrollPulsate(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_spectrum") {
        device->setScrollSpectrum();
    } else if(identifier == "lighting_scroll_static") {
        QColor c = getColorForButton(1, zone);
        device->setScrollStatic(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_none") {
        device->setScrollNone();
    } else if(identifier == "lighting_scroll_reactive") {
        QColor c = getColorForButton(1, zone);
        device->setScrollReactive(c.red(), c.green(), c.blue(), librazer::REACTIVE_500MS); // TODO Configure speed?
    } else if(identifier == "lighting_scroll_breath_single") {
        QColor c = getColorForButton(1, zone);
        device->setScrollBreathSingle(c.red(), c.green(), c.blue());
    } else if(identifier == "lighting_scroll_breath_dual") {
        QColor c1 = getColorForButton(1, zone);
        QColor c2 = getColorForButton(2, zone);
        device->setScrollBreathDual(c1.red(), c1.green(), c1.blue(), c2.red(), c2.green(), c2.blue());
    } else if(identifier == "lighting_scroll_breath_random") {
        device->setScrollBreathRandom();
    } else {
        qDebug() << identifier << " is not implemented yet!";
    }
}


void RazerGenie::applyEffect(librazer::Device::lightingLocation loc)
{
    qDebug() << "applyEffect()";
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    QComboBox *combobox = item->findChild<QComboBox*>(QString::number(loc));

    librazer::RazerCapability capability = combobox->itemData(combobox->currentIndex()).value<librazer::RazerCapability>();
    QString identifier = capability.getIdentifier();

    librazer::Device *dev = devices.value(item->getSerial());

    if(loc == librazer::Device::lightingLocation::lighting) {
        applyEffectStandardLoc(identifier, dev);
    } else if(loc == librazer::Device::lightingLocation::lighting_logo) {
        applyEffectLogoLoc(identifier, dev);
    } else if(loc == librazer::Device::lightingLocation::lighting_scroll) {
        applyEffectScrollLoc(identifier, dev);
    } else {
        showError("Unhandled lighting location in applyEffect()");
    }
}

void RazerGenie::waveRadioButtonStandard(bool enabled)
{
    if(enabled)
        applyEffect(librazer::Device::lightingLocation::lighting);
}

void RazerGenie::waveRadioButtonLogo(bool enabled)
{
    if(enabled)
        applyEffect(librazer::Device::lightingLocation::lighting_logo);
}

void RazerGenie::waveRadioButtonScroll(bool enabled)
{
    if(enabled)
        applyEffect(librazer::Device::lightingLocation::lighting_scroll);
}

void RazerGenie::dpiSyncCheckbox(bool checked)
{
    // TODO Sync DPI right here? Or just at next change (current behaviour)?
    syncDpi = checked;
}

void RazerGenie::pollCombo(int /* index */)
{
    // get device pointer
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    librazer::Device *dev = devices.value(item->getSerial());

    QComboBox *sender = qobject_cast<QComboBox*>(QObject::sender());
    dev->setPollRate(sender->currentData().toInt());
}

void RazerGenie::logoActiveCheckbox(bool checked)
{
    // get device pointer
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    librazer::Device *dev = devices.value(item->getSerial());

    dev->setLogoActive(checked);
    qDebug() << checked;
}

void RazerGenie::scrollActiveCheckbox(bool checked)
{
    // get device pointer
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    librazer::Device *dev = devices.value(item->getSerial());

    dev->setScrollActive(checked);
    qDebug() << checked;
}

void RazerGenie::openCustomEditor()
{
    CustomEditor *cust = new CustomEditor;
    cust->show();
}

void RazerGenie::deviceAdded()
{
    qDebug() << "DEVICE WAS ADDED!";
}

void RazerGenie::deviceRemoved()
{
    qDebug() << "DEVICE WAS REMOVED!";
}

void RazerGenie::showError(QString error)
{
    QMessageBox messageBox;
    messageBox.critical(0, "Error!", error);
    messageBox.setFixedSize(500, 200);
}

void RazerGenie::showInfo(QString info)
{
    QMessageBox messageBox;
    messageBox.information(0, "Information!", info);
    messageBox.setFixedSize(500, 200);
}

#include "razergenie.moc"
