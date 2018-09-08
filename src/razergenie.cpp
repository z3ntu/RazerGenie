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

#include <iostream>
#include <QtDBus/QDBusConnection>
#include <QDBusServiceWatcher>
#include <QtWidgets>

#include <config.h>

#include "razergenie.h"
#include "libopenrazer/libopenrazer.h"
#include "libopenrazer/razercapability.h"
#include "customeditor/customeditor.h"
#include "preferences/preferences.h"
#include "razerimagedownloader.h"
#include "razerdevicewidget.h"
#include "devicelistwidget.h"
#include "util.h"

#define newIssueUrl "https://github.com/openrazer/openrazer/issues/new"
#define supportedDevicesUrl "https://github.com/openrazer/openrazer/blob/master/README.md#device-support"
#define troubleshootingUrl "https://github.com/openrazer/openrazer/wiki/Troubleshooting"
#define websiteUrl "https://openrazer.github.io/"

RazerGenie::RazerGenie(QWidget *parent) : QWidget(parent)
{
    // Set the directory of the application to where the application is located. Needed for the custom editor and relative paths.
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    manager = new libopenrazer::Manager();

    // What to do:
    // If disabled, popup to enable : "The daemon service is not auto-started. Press this button to use the full potential of the daemon right after login." => DONE
    // If enabled: Do nothing => DONE
    // If not_installed: "The daemon is not installed (or the version is too old). Please follow the instructions on the website https://openrazer.github.io/"
    // If no_systemd: Check if daemon is not running: "It seems you are not using systemd as your init system. You have to find a way to auto-start the daemon yourself."
    libopenrazer::DaemonStatus daemonStatus = manager->getDaemonStatus();

    // Check if daemon available
    if(!manager->isDaemonRunning()) {
        // Build a UI depending on what the status is.

        if(daemonStatus == libopenrazer::DaemonStatus::NotInstalled) {
            QVBoxLayout *boxLayout = new QVBoxLayout(this);
            QLabel *titleLabel = new QLabel(tr("The OpenRazer daemon is not installed"));
            QLabel *textLabel = new QLabel(tr("The daemon is not installed or the version installed is too old. Please follow the installation instructions on the website!\n\nIf you are running RazerGenie as a flatpak, you will still have to install OpenRazer outside of flatpak from a distribution package."));
            QPushButton *button = new QPushButton(tr("Open OpenRazer website"));
            connect(button, &QPushButton::pressed, this, &RazerGenie::openWebsiteUrl);

            boxLayout->setAlignment(Qt::AlignTop);

            QFont titleFont("Arial", 18, QFont::Bold);
            titleLabel->setFont(titleFont);

            boxLayout->addWidget(titleLabel);
            boxLayout->addWidget(textLabel);
            boxLayout->addWidget(button);
        } else if(daemonStatus == libopenrazer::DaemonStatus::NoSystemd) {
            QVBoxLayout *boxLayout = new QVBoxLayout(this);
            QLabel *titleLabel = new QLabel(tr("The OpenRazer daemon is not available."));
            QLabel *textLabel = new QLabel(tr("The OpenRazer daemon is not started and you are not using systemd as your init system.\nYou have to either start the daemon manually every time you log in or set up another method of autostarting the daemon.\n\nManually starting would be running \"openrazer-daemon\" in a terminal."));

            boxLayout->setAlignment(Qt::AlignTop);

            QFont titleFont("Arial", 18, QFont::Bold);
            titleLabel->setFont(titleFont);

            boxLayout->addWidget(titleLabel);
            boxLayout->addWidget(textLabel);
        } else { // Daemon status here can be enabled, unknown (and potentially disabled)
            QGridLayout *gridLayout = new QGridLayout(this);
            QLabel *label = new QLabel(tr("The OpenRazer daemon is currently not available. The status output is below."));
            QTextEdit *textEdit = new QTextEdit();
            QLabel *issueLabel = new QLabel(tr("If you think, there's a bug, you can report an issue on GitHub:"));
            QPushButton *issueButton = new QPushButton(tr("Report issue"));

            textEdit->setReadOnly(true);
            textEdit->setText(manager->getDaemonStatusOutput());

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

        if(daemonStatus == libopenrazer::DaemonStatus::Disabled) {
            QMessageBox msgBox;
            msgBox.setText(tr("The OpenRazer daemon is not set to auto-start. Click \"Enable\" to use the full potential of the daemon right after login."));
            QPushButton *enableButton = msgBox.addButton(tr("Enable"), QMessageBox::ActionRole);
            msgBox.addButton(QMessageBox::Ignore);
            // Show message box
            msgBox.exec();

            if (msgBox.clickedButton() == enableButton) {
                manager->enableDaemon();
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
    QHashIterator<QDBusObjectPath, libopenrazer::Device*> i(devices);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
}

void RazerGenie::setupUi()
{
    ui_main.setupUi(this);

    ui_main.versionLabel->setText(tr("Daemon version: %1").arg(manager->getDaemonVersion()));

    fillDeviceList();

    //Connect signals
    connect(ui_main.preferencesButton, &QPushButton::pressed, this, &RazerGenie::openPreferences);
    connect(ui_main.syncCheckBox, &QCheckBox::clicked, this, &RazerGenie::toggleSync);
    ui_main.syncCheckBox->setChecked(manager->getSyncEffects());
    connect(ui_main.screensaverCheckBox, &QCheckBox::clicked, this, &RazerGenie::toggleOffOnScreesaver);
    ui_main.screensaverCheckBox->setChecked(manager->getTurnOffOnScreensaver());

    connect(ui_main.listWidget, &QListWidget::currentRowChanged, ui_main.stackedWidget, &QStackedWidget::setCurrentIndex);

//     libopenrazer::connectDeviceAdded(this, SLOT(deviceAdded()));
//     libopenrazer::connectDeviceRemoved(this, SLOT(deviceRemoved()));
}

void RazerGenie::dbusServiceRegistered(const QString &serviceName)
{
    qInfo() << "Registered! " << serviceName;
    fillDeviceList();
    util::showInfo(tr("The D-Bus connection was re-established."));
}

void RazerGenie::dbusServiceUnregistered(const QString &serviceName)
{
    qInfo() << "Unregistered! " << serviceName;
    clearDeviceList();
    //TODO: Show another placeholder screen with information that the daemon has been stopped?
    util::showError(tr("The D-Bus connection was lost, which probably means that the daemon has crashed."));
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
            qWarning() << "RazerGenie: Error while parsing the lsusb output.";
            return QList<QPair<int, int>>();
        }
        returnList.append(qMakePair(vid, pid));
    }
    return returnList;
}

void RazerGenie::fillDeviceList()
{
    // Get all connected devices
    QList<QDBusObjectPath> devicePaths = manager->getDevices();

    // Iterate through all devices
    foreach (const QDBusObjectPath &devicePath, devicePaths) {
        addDeviceToGui(devicePath);
    }

    if(devicePaths.size() == 0) {
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
    QList<QDBusObjectPath> devicePaths = manager->getDevices();
    QMutableHashIterator<QDBusObjectPath, libopenrazer::Device*> i(devices);
    while (i.hasNext()) {
        i.next();
        if(devicePaths.contains(i.key())) {
            qDebug() << "Keep: " << i.key().path();
            devicePaths.removeOne(i.key());
        } else {
            libopenrazer::Device* dev = i.value();
            qDebug() << "Remove: " << i.key().path();
            devicePaths.removeOne(i.key());
            removeDeviceFromGui(i.key());
            devices.remove(i.key());
            delete dev;
        }
    }
    QListIterator<QDBusObjectPath> j(devicePaths);
    while(j.hasNext()) {
        QDBusObjectPath devicePath = j.next();
        qDebug() << "Add: " << devicePath.path();
        addDeviceToGui(devicePath);
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

void RazerGenie::addDeviceToGui(const QDBusObjectPath &devicePath)
{
    // Create device instance with current serial
    libopenrazer::Device *currentDevice = new libopenrazer::Device(devicePath);

    // Setup variables for easy access
    QString type = currentDevice->getDeviceType();
    QString name = currentDevice->getDeviceName();

//     qDebug() << devicePath;
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
    devices.insert(devicePath, currentDevice);

    // Download image for device
    if(!currentDevice->getPngFilename().isEmpty()) {
        RazerImageDownloader *dl = new RazerImageDownloader(QUrl(currentDevice->getPngUrl()), this);
        connect(dl, &RazerImageDownloader::downloadFinished, listItemWidget, &DeviceListWidget::imageDownloaded);
        connect(dl, &RazerImageDownloader::downloadErrored, listItemWidget, &DeviceListWidget::imageDownloadErrored);
        dl->startDownload();
    } else {
        qWarning() << ".png mapping for device " << currentDevice->getDeviceName() << "missing.";
        listItemWidget->setNoImage();
    }

    // Types known for now: headset, mouse, mug, keyboard, tartarus, core, orbweaver
    qDebug() << type;

    /* Create actual DeviceWidget */
    RazerDeviceWidget *widget = new RazerDeviceWidget(name, devicePath);

    QVBoxLayout *verticalLayout = new QVBoxLayout(widget);

    // List of locations to iterate through
    QList<libopenrazer::Led *> leds;
    foreach(const QDBusObjectPath &ledPath, currentDevice->getLeds()) {
        libopenrazer::Led *led = new libopenrazer::Led(ledPath);
        leds.append(led);
    }

    // Declare header font
    QFont headerFont("Arial", 15, QFont::Bold);
    QFont titleFont("Arial", 18, QFont::Bold);

    // Add header with the device name
    QLabel *header = new QLabel(name, widget);
    header->setFont(titleFont);
    verticalLayout->addWidget(header);

    // Lighting header
    if(leds.size() != 0) {
        QLabel *lightingHeader = new QLabel(tr("Lighting"), widget);
        lightingHeader->setFont(headerFont);
        verticalLayout->addWidget(lightingHeader);
    }

    // Iterate through lighting locations
    foreach(libopenrazer::Led *led, leds) {

        QLabel *lightingLocationLabel;

        // Set appropriate text
        lightingLocationLabel = new QLabel(tr("Lighting")); // TODO: Adjust based on LED

        QHBoxLayout *lightingHBox = new QHBoxLayout();
        verticalLayout->addWidget(lightingLocationLabel);
        verticalLayout->addLayout(lightingHBox);

        QComboBox *comboBox = new QComboBox;
        QLabel *brightnessLabel = NULL;
        QSlider *brightnessSlider = NULL;

//         comboBox->setObjectName(QString::number(currentLocation)); // FIXME
//         qDebug() << "CURRENT LOCATION: " << QString::number(currentLocation);
        //TODO More elegant solution instead of the sizePolicy?
        comboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

        //TODO Battery
        //TODO Sync effects in comboboxes & colorStuff when the sync checkbox is active

        // Add items from capabilities
        for(int i=0; i<libopenrazer::lightingComboBoxCapabilites.size(); i++) {
            if(currentDevice->hasCapability(libopenrazer::lightingComboBoxCapabilites[i].getIdentifier())) {
                comboBox->addItem(libopenrazer::lightingComboBoxCapabilites[i].getDisplayString(), QVariant::fromValue(libopenrazer::lightingComboBoxCapabilites[i]));
            }
        }

        // Connect signal from combobox
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RazerGenie::standardCombo);

        // Brightness slider
        if(currentDevice->hasCapability("brightness")) {
            brightnessLabel = new QLabel(tr("Brightness"));
            brightnessSlider = new QSlider(Qt::Horizontal, widget);
            qDebug() << "Brightness:" << led->getBrightness();
            brightnessSlider->setValue(led->getBrightness());
            connect(brightnessSlider, &QSlider::valueChanged, this, &RazerGenie::brightnessChanged);
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
//                 colorButton->setObjectName(QString::number(currentLocation) + "_colorbutton" + QString::number(i)); // FIXME
                lightingHBox->addWidget(colorButton);

                libopenrazer::RazerCapability capability = comboBox->currentData().value<libopenrazer::RazerCapability>();
                if(capability.getNumColors() < i)
                    colorButton->hide();
                connect(colorButton, &QPushButton::clicked, this, &RazerGenie::colorButtonClicked);
            }

            /* Wave left/right radio buttons */
            for(int i=1; i<=2; i++) {
                QString name;
                if(i==1)
                    name = tr("Left");
                else
                    name = tr("Right");
                QRadioButton *radio = new QRadioButton(name, widget);
//                 radio->setObjectName(QString::number(currentLocation) + "_radiobutton" + QString::number(i)); // FIXME
                if(i==1) // set the 'left' checkbox to activated
                    radio->setChecked(true);
                // hide by default
                radio->hide();
                lightingHBox->addWidget(radio);
                connect(radio, &QRadioButton::toggled, this, &RazerGenie::waveRadioButtonStandard);
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
        QLabel *dpiHeader = new QLabel(tr("DPI"), widget);
        dpiHeader->setFont(headerFont);
        dpiHeaderHBox->addWidget(dpiHeader);

        verticalLayout->addLayout(dpiHeaderHBox);

        // Labels
        QLabel *dpiXLabel = new QLabel(tr("DPI X"));
        QLabel *dpiYLabel = new QLabel(tr("DPI Y"));

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
        QLabel *dpiSyncLabel = new QLabel(tr("Lock X/Y"), widget);
        QCheckBox *dpiSyncCheckbox = new QCheckBox(widget);

        // Get the current DPI and set the slider&text
        razer_test::RazerDPI currDPI = currentDevice->getDPI();
        qDebug() << "currDPI:" << currDPI.dpi_x << currDPI.dpi_y;
        dpiXSlider->setValue(currDPI.dpi_x/100);
        dpiYSlider->setValue(currDPI.dpi_y/100);
        dpiXText->setText(QString::number(currDPI.dpi_x));
        dpiYText->setText(QString::number(currDPI.dpi_y));

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
        QLabel *pollRateHeader = new QLabel(tr("Polling rate"), widget);
        pollRateHeader->setFont(headerFont);
        verticalLayout->addWidget(pollRateHeader);

        QComboBox *pollComboBox = new QComboBox;
        pollComboBox->addItem("125 Hz", libopenrazer::POLL_125HZ);
        pollComboBox->addItem("500 Hz", libopenrazer::POLL_500HZ);
        pollComboBox->addItem("1000 Hz", libopenrazer::POLL_1000HZ);
        pollComboBox->setCurrentText(QString::number(currentDevice->getPollRate()) + " Hz");
        verticalLayout->addWidget(pollComboBox);

        connect(pollComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RazerGenie::pollCombo);
    }

    /* Custom lighting */
    if(currentDevice->hasCapability("lighting_led_matrix")) {
        QPushButton *button = new QPushButton(widget);
        button->setText(tr("Open custom editor"));
        verticalLayout->addWidget(button);
        connect(button, &QPushButton::clicked, this, &RazerGenie::openCustomEditor);
#ifdef INCLUDE_MATRIX_DISCOVERY
        QPushButton *buttonD = new QPushButton(widget);
        buttonD->setText(tr("Launch matrix discovery"));
        verticalLayout->addWidget(buttonD);
        connect(buttonD, &QPushButton::clicked, this, &RazerGenie::openMatrixDiscovery);
#endif
    }

    /* Spacer to bottom */
    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(spacer);

    /* Serial and firmware version labels */
    QLabel *serialLabel = new QLabel(tr("Serial number: %1").arg(currentDevice->getSerial()));
    verticalLayout->addWidget(serialLabel);

    QLabel *fwVerLabel = new QLabel(tr("Firmware version: %1").arg(currentDevice->getFirmwareVersion()));
    verticalLayout->addWidget(fwVerLabel);

    ui_main.stackedWidget->addWidget(widget);
//         qDebug() << "Stacked widget count:" << ui_main.stackedWidget->count();
}

bool RazerGenie::removeDeviceFromGui(const QDBusObjectPath &devicePath)
{
    int index = -1;
    for(int i=0; i<ui_main.listWidget->count(); i++) {
        // get item for index
        QListWidgetItem *item = ui_main.listWidget->item(i);
        // get itemwidget for the item
        DeviceListWidget *widget = dynamic_cast<DeviceListWidget*>(ui_main.listWidget->itemWidget(item));
        // compare serial
        if(widget->device()->objectPath() == devicePath) {
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
        QHashIterator<QString, QVariant> i(manager->getSupportedDevices());
        // Iterate through the supported devices
        while (i.hasNext()) {
            i.next();
            QList<QVariant> list = i.value().toList();
            if(list.count() != 2) {
                qWarning() << "RazerGenie: Error while iterating through supportedDevices";
                qWarning() << list;
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
        headerLabel = new QLabel(tr("No device was detected"));
        textLabel = new QLabel(tr("The OpenRazer daemon didn't detect a device that is supported.\nThis could also be caused due to a misconfiguration of this PC."));
        button1 = new QPushButton(tr("Open supported devices"));
        connect(button1, &QPushButton::pressed, this, &RazerGenie::openSupportedDevicesUrl);
        button2 = new QPushButton(tr("Report issue"));
        connect(button2, &QPushButton::pressed, this, &RazerGenie::openIssueUrl);
    } else {
        headerLabel = new QLabel(tr("The daemon didn't detect a device that is connected"));
        textLabel = new QLabel(tr("Linux detected connected devices but the daemon didn't. This could be either due to a permission problem or a kernel module problem."));
        qDebug() << matches;
        button1 = new QPushButton(tr("Open troubleshooting page"));
        connect(button1, &QPushButton::pressed, this, &RazerGenie::openTroubleshootingUrl);
        button2 = new QPushButton(tr("Report issue"));
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
    if(!manager->syncEffects(sync))
        util::showError(tr("Error while syncing devices."));
}

void RazerGenie::toggleOffOnScreesaver(bool on)
{
    if(!manager->setTurnOffOnScreensaver(on))
        util::showError(tr("Error while toggling 'turn off on screensaver'"));
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
        qInfo() << "User cancelled the dialog.";
    }
    // objectName is location(int)_colorbuttonNR(1-3)
    // TODO: We shouldn't assume the world to be perfect!
//     applyEffect(static_cast<libopenrazer::Device::LightingLocation>(sender->objectName().split("_")[0].toInt())); // FIXME
}

QPair<libopenrazer::Device*, QString> RazerGenie::commonCombo(int index)
{
    QComboBox *sender = qobject_cast<QComboBox*>(QObject::sender());
    libopenrazer::RazerCapability capability = sender->itemData(index).value<libopenrazer::RazerCapability>();
    QString identifier = capability.getIdentifier();

    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    libopenrazer::Device *dev = devices.value(item->getDevicePath());

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
    QPair<libopenrazer::Device*, QString> tuple = commonCombo(index);
    libopenrazer::Device *dev = tuple.first;
    QString identifier = tuple.second;

    qDebug() << tuple;

    applyEffectStandardLoc(identifier, dev);
}

QColor RazerGenie::getColorForButton(int num, uchar ledId)
{
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    QPalette pal = item->findChild<QPushButton*>(QString::number(ledId) + "_colorbutton" + QString::number(num))->palette();
    return pal.color(QPalette::Button);
}

libopenrazer::WaveDirection RazerGenie::getWaveDirection(uchar ledId)
{
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());

    return item->findChild<QRadioButton*>(QString::number(ledId) + "_radiobutton1")->isChecked() ? libopenrazer::WAVE_LEFT : libopenrazer::WAVE_RIGHT;
}

void RazerGenie::brightnessChanged(int value)
{
    qDebug() << value;

    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    libopenrazer::Device *dev = devices.value(item->getDevicePath());
//     dev->setBrightness(value); // FIXME
}

void RazerGenie::dpiChanged(int orig_value)
{
    ushort value = orig_value * 100;

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
            libopenrazer::Device *dev = devices.value(item->getDevicePath());
            // set DPI
            dev->setDPI({value, value}); // set for both X & Y
        } else {
            // just set the slider (as the rest was done already or will be done)
            QSlider *slider = sender->parentWidget()->findChild<QSlider*>("dpiX");
            slider->setValue(orig_value);
        }
    } /* if DPI should NOT be synced */ else {
        // get device pointer
        RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
        libopenrazer::Device *dev = devices.value(item->getDevicePath());

        // set DPI (with value from other slider)
        if(sender->objectName() == "dpiX") {
            QSlider *slider = sender->parentWidget()->findChild<QSlider*>("dpiY");
            dev->setDPI({value, static_cast<ushort>(slider->value()*100)});
        } else {
            QSlider *slider = sender->parentWidget()->findChild<QSlider*>("dpiX");
            dev->setDPI({static_cast<ushort>(slider->value()*100), value});
        }
    }
    // Update textbox with new value
    QTextEdit *dpitextbox = sender->parentWidget()->findChild<QTextEdit*>(sender->objectName() + "Text");
    dpitextbox->setText(QString::number(value));
}

void RazerGenie::applyEffectStandardLoc(QString identifier, libopenrazer::Device *device)
{
//     libopenrazer::Device::LightingLocation zone = libopenrazer::Device::Lighting;
    uchar zone = 1; // actually ledId FIXME

    if(identifier == "lighting_breath_single") {
        QColor c = getColorForButton(1, zone);
//         device->setBreathingSingle(c);
    } else if(identifier == "lighting_breath_dual") {
        QColor c1 = getColorForButton(1, zone);
        QColor c2 = getColorForButton(2, zone);
//         device->setBreathingDual(c1, c2);
    } else if(identifier == "lighting_breath_triple") {
        QColor c1 = getColorForButton(1, zone);
        QColor c2 = getColorForButton(2, zone);
        QColor c3 = getColorForButton(3, zone);
//         device->setBreathingTriple(c1, c2, c3);
    } else if(identifier == "lighting_breath_random") {
//         device->setBreathingRandom();
    } else if(identifier == "lighting_wave") {
//         device->setWave(getWaveDirection(zone));
    } else if(identifier == "lighting_reactive") {
        QColor c = getColorForButton(1, zone);
//         device->setReactive(c, libopenrazer::REACTIVE_500MS); // TODO Configure speed?
    } else if(identifier == "lighting_none") {
//         device->setNone();
    } else if(identifier == "lighting_spectrum") {
//         device->setSpectrum();
    } else if(identifier == "lighting_static") {
        QColor c = getColorForButton(1, zone);
//         device->setStatic(c);
    } else if(identifier == "lighting_ripple") {
        QColor c = getColorForButton(1, zone);
//         device->setRipple(c, libopenrazer::RIPPLE_REFRESH_RATE); //TODO Configure refreshrate?
    } else if(identifier == "lighting_ripple_random") {
//         device->setRippleRandomColor(libopenrazer::RIPPLE_REFRESH_RATE); //TODO Configure refreshrate?
    } else if(identifier == "lighting_static_bw2013") {
//         device->setStatic_bw2013();
    } else if(identifier == "lighting_pulsate") {
//         device->setPulsate();
    } else {
        qWarning() << identifier << " is not implemented yet!";
    }
}

void RazerGenie::applyEffect(uchar ledId)
{
    qDebug() << "applyEffect()";
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    QComboBox *combobox = item->findChild<QComboBox*>(QString::number(ledId));

    libopenrazer::RazerCapability capability = combobox->itemData(combobox->currentIndex()).value<libopenrazer::RazerCapability>();
    QString identifier = capability.getIdentifier();

    libopenrazer::Device *dev = devices.value(item->getDevicePath());

    applyEffectStandardLoc(identifier, dev);
}

void RazerGenie::waveRadioButtonStandard(bool enabled)
{
    if(enabled)
//         applyEffect(libopenrazer::Device::Lighting); // FIXME
        qDebug();
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
    libopenrazer::Device *dev = devices.value(item->getDevicePath());

    QComboBox *sender = qobject_cast<QComboBox*>(QObject::sender());
    dev->setPollRate(sender->currentData().value<libopenrazer::PollRate>());
}

void RazerGenie::openCustomEditor()
{
    // get device pointer
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    libopenrazer::Device *dev = devices.value(item->getDevicePath());

//     CustomEditor *cust = new CustomEditor(dev);
//     cust->setAttribute(Qt::WA_DeleteOnClose);
//     cust->show();
}

#ifdef INCLUDE_MATRIX_DISCOVERY
void RazerGenie::openMatrixDiscovery()
{
    // get device pointer
    RazerDeviceWidget *item = dynamic_cast<RazerDeviceWidget*>(ui_main.stackedWidget->currentWidget());
    libopenrazer::Device *dev = devices.value(item->getDevicePath());

//     CustomEditor *cust = new CustomEditor(dev, true);
//     cust->setAttribute(Qt::WA_DeleteOnClose);
//     cust->show();
}
#endif

void RazerGenie::openPreferences()
{
    Preferences *prefs = new Preferences();
    prefs->setAttribute(Qt::WA_DeleteOnClose);
    prefs->show();
}

void RazerGenie::deviceAdded()
{
    qInfo() << "DEVICE WAS ADDED!";
    refreshDeviceList();
}

void RazerGenie::deviceRemoved()
{
    qInfo() << "DEVICE WAS REMOVED!";
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
