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
#include "devicelistwidget.h"
#include "customeditor.h"

#define newIssueUrl "https://github.com/openrazer/openrazer/issues/new"
#define supportedDevicesUrl "https://github.com/openrazer/openrazer/blob/master/README.md#device-support"
#define troubleshootingUrl "https://github.com/openrazer/openrazer/wiki/Troubleshooting"
#define websiteUrl "https://openrazer.github.io/"

RazerGenie::RazerGenie(QWidget *parent) : QWidget(parent)
{
    // What to do:
    // If disabled, popup to enable : "The daemon service is not auto-started. Press this button to use the full potential of the daemon right after login." => DONE
    // If enabled: Do nothing => DONE
    // If not_installed: "The daemon is not installed (or the version is too old). Please follow the instructions on the website https://openrazer.github.io/"
    // If no_systemd: Check if daemon is not running: "It seems you are not using systemd as your init system. You have to find a way to auto-start the daemon yourself."
    librazer::daemonStatus daemonStatus = librazer::getDaemonStatus();

    // Check if daemon available
    if(!librazer::isDaemonRunning()) {
        // Build a UI depending on what the status is.

        if(daemonStatus == librazer::daemonStatus::not_installed) {
            //TODO: Show in error ui
            qDebug() << "Daemon not installed";
            //showError("The daemon is not installed or the version installed is too old. Please follow the instructions on the website: https://openrazer.github.io/");
            QVBoxLayout *boxLayout = new QVBoxLayout(this);
            QLabel *titleLabel = new QLabel("The daemon is not installed");
            QLabel *textLabel = new QLabel("The daemon is not installed or the version installed is too old. Please follow the installation instructions on the website!");
            QPushButton *button = new QPushButton("Open website");
            connect(button, &QPushButton::pressed, this, &RazerGenie::openWebsiteUrl);

            boxLayout->setAlignment(Qt::AlignTop);

            QFont titleFont("Arial", 18, QFont::Bold);
            titleLabel->setFont(titleFont);

            boxLayout->addWidget(titleLabel);
            boxLayout->addWidget(textLabel);
            boxLayout->addWidget(button);
        } else if(daemonStatus == librazer::daemonStatus::no_systemd) {
            qDebug() << "No systemd";
            QVBoxLayout *boxLayout = new QVBoxLayout(this);
            QLabel *titleLabel = new QLabel("The daemon is not available.");
            QLabel *textLabel = new QLabel("The openrazer daemon is not started and you are not using systemd as your init system.\nYou have to either start the daemon manually every time you log in or set up another method of autostarting the daemon.\n\nManually starting would be running \"razer-daemon\" in a terminal and re-opening RazerGenie.");

            boxLayout->setAlignment(Qt::AlignTop);

            QFont titleFont("Arial", 18, QFont::Bold);
            titleLabel->setFont(titleFont);

            boxLayout->addWidget(titleLabel);
            boxLayout->addWidget(textLabel);
        } else { // Daemon status here can be enabled, unknown (and potentially disabled)
            qDebug() << "Unknown daemon status";
            QGridLayout *gridLayout = new QGridLayout(this);
            QLabel *label = new QLabel("The daemon is currently not available. The status output is below.");
            QTextEdit *textEdit = new QTextEdit();
            QLabel *issueLabel = new QLabel("If you think, there's a bug, you can report an issue on GitHub:");
            QPushButton *issueButton = new QPushButton("Report issue");

            textEdit->setReadOnly(true);
            textEdit->setText(librazer::getDaemonStatusOutput());

            gridLayout->addWidget(label, 0, 1, 1, 2);
            gridLayout->addWidget(textEdit, 1, 1, 1, 2);
            gridLayout->addWidget(issueLabel, 2, 1);
            gridLayout->addWidget(issueButton, 2, 2);

            connect(issueButton, &QPushButton::pressed, this, &RazerGenie::openIssueUrl);
        }
        this->resize(1024, 600);
        this->setMinimumSize(QSize(800, 500));
        this->setWindowTitle("RazerGenie");
    } else {
        // Set up the normal UI
        setupUi();

        if(daemonStatus == librazer::daemonStatus::disabled) {
            qDebug() << "Daemon disabled";
            QMessageBox msgBox;
            msgBox.setText("The openrazer daemon is not set to auto-start. Click \"Enable\" to use the full potential of the daemon right after login.");
            QPushButton *enableButton = msgBox.addButton("Enable", QMessageBox::ActionRole);
            msgBox.addButton(QMessageBox::Abort);
            // Show message box
            msgBox.exec();

            if (msgBox.clickedButton() == enableButton) {
                qDebug() << "enable daemon";
                librazer::enableDaemon();
            } // ignore the cancel button
        }

        // Watch for dbus service changes (= daemon ends or gets started)
        QDBusServiceWatcher *watcher = new QDBusServiceWatcher("org.razer", QDBusConnection::sessionBus());

        connect(watcher, &QDBusServiceWatcher::serviceRegistered,
                this, &RazerGenie::dbusServiceRegistered);
        connect(watcher, &QDBusServiceWatcher::serviceUnregistered,
                this, &RazerGenie::dbusServiceUnregistered);
    }
}

RazerGenie::~RazerGenie()
{
//    delete ui;
}

void RazerGenie::setupUi()
{
    ui_main.setupUi(this);

    ui_main.versionLabel->setText("Daemon version: " + librazer::getDaemonVersion());

    fillDeviceList();

    //Connect signals
    connect(ui_main.syncCheckBox, &QCheckBox::clicked, this, &RazerGenie::toggleSync);
    ui_main.syncCheckBox->setChecked(librazer::getSyncEffects());
    connect(ui_main.screensaverCheckBox, &QCheckBox::clicked, this, &RazerGenie::toggleOffOnScreesaver);
    ui_main.screensaverCheckBox->setChecked(librazer::getTurnOffOnScreensaver());

    connect(ui_main.listWidget, &QListWidget::currentRowChanged, ui_main.stackedWidget, &QStackedWidget::setCurrentIndex);

    librazer::connectDeviceAdded(this, SLOT(deviceAdded()));
    librazer::connectDeviceRemoved(this, SLOT(deviceRemoved()));
}

void RazerGenie::dbusServiceRegistered(const QString &serviceName)
{
    qDebug() << "Registered! " << serviceName;
    fillDeviceList();
    showInfo("The D-Bus connection was re-established.");
}

void RazerGenie::dbusServiceUnregistered(const QString &serviceName)
{
    qDebug() << "Unregistered! " << serviceName;
    clearDeviceList();
    //TODO: Show another placeholder screen with information that the daemon has been stopped?
    showError("The D-Bus connection was lost.");
}

/**
 * Returns a list of connected devices, which are detected by Linux / lsusb. VID and PID are in decimal form.
 */
QList<QPair<int, int>> RazerGenie::getConnectedDevices_lsusb()
{
    // Get list of Razer devices connected to the PC: lsusb | grep '1532:' | cut -d' ' -f6
    QProcess process;
    process.start("bash", QStringList() << "-c" << "lsusb | grep '1532:' | cut -d' ' -f6");
    process.waitForFinished();
    QStringList outputList = QString(process.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);

    QList<QPair<int, int>> returnList;

    // Transform the list ["1234:abcd", "5678:def0"] into a QList with QPairs.
    QStringListIterator i(outputList);
    while(i.hasNext()) {
        QStringList split = i.next().split(":");
        bool ok;
        //TODO: Check if count is 2? Otherwise SIGSEGV probably
        int vid = split[0].toInt(&ok, 16);
        int pid = split[1].toInt(&ok, 16);
        if(!ok) {
            qDebug() << "RazerGenie: Error while parsing the lsusb output.";
            return QList<QPair<int, int>>();
        }
        returnList.append(qMakePair(vid, pid));
    }
    return returnList;
}

void RazerGenie::fillDeviceList()
{
    // Get all connected devices
    QStringList serialnrs = librazer::getConnectedDevices();

    // Iterate through all devices
    foreach (const QString &serial, serialnrs) {
        addDeviceToGui(serial);
    }

    if(serialnrs.size() == 0) {
        // Add placeholder widget
        ui_main.stackedWidget->addWidget(getNoDevicePlaceholder());
    }
}

void RazerGenie::refreshDeviceList()
{
    // LOGIC:
    // - list of current
    // - hash of old
    // go through old
    // if still in new, remove from new list
    // if not in new, remove from both
    // go through new (remaining items) list and add
    QStringList serialnrs = librazer::getConnectedDevices();
    QMutableHashIterator<QString, librazer::Device*> i(devices);
    while (i.hasNext()) {
        i.next();
        if(serialnrs.contains(i.key())) {
            qDebug() << "Keep:";
            qDebug() << i.key();
            serialnrs.removeOne(i.key());
        } else {
            qDebug() << "Remove:";
            qDebug() << i.key();
            serialnrs.removeOne(i.key());
            devices.remove(i.key());
            removeDeviceFromGui(i.key());
        }
    }
    QStringListIterator j(serialnrs);
    while(j.hasNext()) {
        QString serial = j.next();
        qDebug() << "Add:";
        qDebug() << serial;
        addDeviceToGui(serial);
    }
}

void RazerGenie::clearDeviceList()
{
    // Clear devices QHash
    devices.clear();
    // Clear device list
    ui_main.listWidget->clear();
    // Clear stackedwidget
    for(int i = ui_main.stackedWidget->count(); i >= 0; i--) {
        QWidget* widget = ui_main.stackedWidget->widget(i);
        ui_main.stackedWidget->removeWidget(widget);
        widget->deleteLater();
    }
    // Add placeholder widget
    // TODO: Add placeholder widget with crash information and link to bug report?
    ui_main.stackedWidget->addWidget(getNoDevicePlaceholder());
}

void RazerGenie::addDeviceToGui(const QString &serial)
{
    // Create device instance with current serial
    librazer::Device *currentDevice = new librazer::Device(serial);

    // Setup variables for easy access
    QString type = currentDevice->getDeviceType();
    QString name = currentDevice->getDeviceName();

    qDebug() << serial;
    qDebug() << name;

    if(devices.isEmpty()) {
        // Remove placeholder widget if inserted.
        ui_main.stackedWidget->removeWidget(ui_main.stackedWidget->widget(0));
    }

//     qDebug() << "Width" << ui_main.listWidget->width();
//     qDebug() << "Height" << ui_main.listWidget->height();

    // Add new device to the list
    QListWidgetItem *listItem = new QListWidgetItem();
    listItem->setSizeHint(QSize(listItem->sizeHint().width(), 120));
    ui_main.listWidget->addItem(listItem);
    DeviceListWidget *listItemWidget = new DeviceListWidget(ui_main.listWidget, currentDevice);
    ui_main.listWidget->setItemWidget(listItem, listItemWidget);

    // Insert current device pointer with serial lookup into a QHash
    devices.insert(serial, currentDevice);

    // Download image for device
    if(!currentDevice->getPngFilename().isEmpty()) {
        RazerImageDownloader *dl = new RazerImageDownloader(QUrl(currentDevice->getPngUrl()), this);
        connect(dl, &RazerImageDownloader::downloadFinished, listItemWidget, &DeviceListWidget::imageDownloaded);
    } else {
        qDebug() << ".png mapping for device '" + currentDevice->getDeviceName() + "' (PID "+QString::number(currentDevice->getPid())+") missing.";
        listItemWidget->setNoImage();
    }

    // Types known for now: headset, mouse, mug, keyboard, tartarus, core, orbweaver
    qDebug() << type;

    /* Create actual DeviceWidget */
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
    QFont titleFont("Arial", 18, QFont::Bold);

    // Add header with the device name
    QLabel *header = new QLabel(name, widget);
    header->setFont(titleFont);
    verticalLayout->addWidget(header);

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

        QHBoxLayout *lightingHBox = new QHBoxLayout();
        verticalLayout->addWidget(lightingLocationLabel);
        verticalLayout->addLayout(lightingHBox);

        QComboBox *comboBox = new QComboBox;
        QLabel *brightnessLabel = NULL;
        QSlider *brightnessSlider = NULL;

        comboBox->setObjectName(QString::number(currentLocation));
        qDebug() << "CURRENT LOCATION: " << QString::number(currentLocation);
        //TODO More elegant solution instead of the sizePolicy?
        comboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

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
                    qDebug() << "Brightness:" << currentDevice->getBrightness();
                    brightnessSlider->setValue(currentDevice->getBrightness());
                } else {
                    // Set the slider to 100 by default as it's more likely it's 100 than 0...
                    brightnessSlider->setValue(100);
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
                } else {
                    // Set the slider to 100 by default as it's more likely it's 100 than 0...
                    brightnessSlider->setValue(100);
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
                } else {
                    // Set the slider to 100 by default as it's more likely it's 100 than 0...
                    brightnessSlider->setValue(100);
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

        /* 'Set Logo Active' checkbox */
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

        /* 'Set Scroll Active' checkbox */
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
            QHBoxLayout *hboxSlider = new QHBoxLayout();
            QLabel *brightnessSliderValue = new QLabel;
            hboxSlider->addWidget(brightnessSlider);
            hboxSlider->addWidget(brightnessSliderValue);
            verticalLayout->addLayout(hboxSlider);
        }
    }

    /* DPI sliders */
    if(currentDevice->hasCapability("dpi")) {
        // HBoxes
        QHBoxLayout *dpiXHBox = new QHBoxLayout();
        QHBoxLayout *dpiYHBox = new QHBoxLayout();
        QHBoxLayout *dpiHeaderHBox = new QHBoxLayout();

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
        if(currDPI.count() == 2) {
            dpiXSlider->setValue(currDPI[0]/100);
            dpiYSlider->setValue(currDPI[1]/100);
            dpiXText->setText(QString::number(currDPI[0]));
            dpiYText->setText(QString::number(currDPI[1]));
        } else {
            qDebug() << "RazerGenie: Skipping dpi because return value of getDPI() is wrong. Probably the broken fake driver.";
        }

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

    ui_main.stackedWidget->addWidget(widget);
//         qDebug() << "Stacked widget count:" << ui_main.stackedWidget->count();
}

bool RazerGenie::removeDeviceFromGui(const QString &serial)
{
    qDebug() << "Remove device" << serial;
    int index = -1;
    for(int i=0; i<ui_main.listWidget->count(); i++) {
        // get item for index
        QListWidgetItem *item = ui_main.listWidget->item(i);
        // get itemwidget for the item
        DeviceListWidget *widget = dynamic_cast<DeviceListWidget*>(ui_main.listWidget->itemWidget(item));
        // compare serial
        if(widget->device()->serial() == serial) {
            index = i;
            break;
        }
    }
    if(index == -1) {
        return false;
    }
    ui_main.stackedWidget->removeWidget(ui_main.stackedWidget->widget(index));
    delete ui_main.listWidget->takeItem(index);

    // Add placeholder widget if the stackedWidget is empty after removing.
    if(devices.isEmpty()) {
        ui_main.stackedWidget->addWidget(getNoDevicePlaceholder());
    }
    return true;
}

QWidget *RazerGenie::getNoDevicePlaceholder()
{
    if(noDevicePlaceholder != NULL) {
        return noDevicePlaceholder;
    }
    // Generate placeholder widget with text "No device is connected.". Maybe add a usb pid check - at least add link to readme and troubleshooting page. Maybe add support for the future daemon troubleshooting option.

    QList<QPair<int, int>> connectedDevices = getConnectedDevices_lsusb();
    QList<QPair<int, int>> matches;

    // Don't even iterate if there are no devices detected by lsusb.
    if(connectedDevices.count() != 0) {
        QHashIterator<QString, QVariant> i(librazer::getSupportedDevices());
        // Iterate through the supported devices
        while (i.hasNext()) {
            i.next();
            QList<QVariant> list = i.value().toList();
            if(list.count() != 2) {
                qDebug() << "RazerGenie: Error while iterating through supportedDevices";
                qDebug() << list;
                continue;
            }
            int vid = list[0].toInt();
            int pid = list[1].toInt();

            QListIterator<QPair<int, int>> j(connectedDevices);
            while (j.hasNext()) {
                QPair<int, int> x = j.next();
                if(x.first == vid && x.second == pid) {
                    qDebug() << "Found a device match!";
                    matches.append(x);
                }
            }
        }
    }

    noDevicePlaceholder = new QWidget();
    QVBoxLayout *boxLayout = new QVBoxLayout(noDevicePlaceholder);
    boxLayout->setAlignment(Qt::AlignTop);

    QFont headerFont("Arial", 15, QFont::Bold);
    QLabel *headerLabel;
    QLabel *textLabel;
    QPushButton *button1;
    QPushButton *button2;
    if(matches.size() == 0) {
        headerLabel = new QLabel("No device was detected");
        textLabel = new QLabel("The openrazer daemon didn't detect a device that is supported.\nThis could also be caused due to a misconfiguration of this PC.");
        button1 = new QPushButton("Open supported devices");
        connect(button1, &QPushButton::pressed, this, &RazerGenie::openSupportedDevicesUrl);
        button2 = new QPushButton("Report issue");
        connect(button2, &QPushButton::pressed, this, &RazerGenie::openIssueUrl);
    } else {
        headerLabel = new QLabel("The daemon didn't detect a device that is connected");
        textLabel = new QLabel("Linux detected connected devices but the daemon didn't. This could be either due to a permission problem or a kernel module problem.");
        qDebug() << matches;
        button1 = new QPushButton("Open troubleshooting page");
        connect(button1, &QPushButton::pressed, this, &RazerGenie::openTroubleshootingUrl);
        button2 = new QPushButton("Report issue");
        connect(button2, &QPushButton::pressed, this, &RazerGenie::openIssueUrl);
    }
    headerLabel->setFont(headerFont);

    boxLayout->addWidget(headerLabel);
    boxLayout->addWidget(textLabel);
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(button1);
    hbox->addWidget(button2);
    boxLayout->addLayout(hbox);
    return noDevicePlaceholder;
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

int RazerGenie::getWaveDirection(librazer::Device::lightingLocation location)
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
    refreshDeviceList();
}

void RazerGenie::deviceRemoved()
{
    qDebug() << "DEVICE WAS REMOVED!";
    refreshDeviceList();
}

void RazerGenie::openIssueUrl()
{
    QDesktopServices::openUrl(QUrl(newIssueUrl));
}

void RazerGenie::openSupportedDevicesUrl()
{
    QDesktopServices::openUrl(QUrl(supportedDevicesUrl));
}

void RazerGenie::openTroubleshootingUrl()
{
    QDesktopServices::openUrl(QUrl(troubleshootingUrl));
}

void RazerGenie::openWebsiteUrl()
{
    QDesktopServices::openUrl(QUrl(websiteUrl));
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
