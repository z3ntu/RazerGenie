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

#include "razergenie.h"

#include "devicelistwidget.h"
#include "devicewidget.h"
#include "preferences/preferences.h"
#include "razerimagedownloader.h"
#include "util.h"

#include <QDBusServiceWatcher>
#include <QtWidgets>
#include <config.h>

const char *newIssueUrl = "https://github.com/openrazer/openrazer/issues/new/choose";
const char *supportedDevicesUrl = "https://github.com/openrazer/openrazer/blob/master/README.md#device-support";
const char *troubleshootingUrl = "https://github.com/openrazer/openrazer/wiki/Troubleshooting";
const char *websiteUrl = "https://openrazer.github.io/";

RazerGenie::RazerGenie(QWidget *parent)
    : QWidget(parent)
{
    // Set the directory of the application to where the application is located. Needed for the custom editor and relative paths.
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    QString backend = settings.value("backend").toString();
    if (backend == "OpenRazer") {
        manager = new libopenrazer::openrazer::Manager();
    } else if (backend == "razer_test") {
        manager = new libopenrazer::razer_test::Manager();
    } else {
        qWarning() << "Invalid backend value. Using openrazer backend.";
        manager = new libopenrazer::openrazer::Manager();
    }

    // What to do:
    // If disabled, popup to enable : "The daemon service is not auto-started. Press this button to use the full potential of the daemon right after login." => DONE
    // If enabled: Do nothing => DONE
    // If not_installed: "The daemon is not installed (or the version is too old). Please follow the instructions on the website https://openrazer.github.io/"
    // If no_systemd: Check if daemon is not running: "It seems you are not using systemd as your init system. You have to find a way to auto-start the daemon yourself."
    libopenrazer::DaemonStatus daemonStatus = manager->getDaemonStatus();

    // Check if daemon available
    if (!manager->isDaemonRunning()) {
        // Build a UI depending on what the status is.

        if (daemonStatus == libopenrazer::DaemonStatus::NotInstalled) {
            auto *boxLayout = new QVBoxLayout(this);
            QLabel *titleLabel = new QLabel(tr("The OpenRazer daemon is not installed"));
            QLabel *textLabel = new QLabel(tr("The daemon is not installed or the version installed is too old. Please follow the installation instructions on the website!\n\nIf you are running RazerGenie as a flatpak, you will still have to install OpenRazer outside of flatpak from a distribution package."));
            QPushButton *button = new QPushButton(tr("Open OpenRazer website"));
            connect(button, &QPushButton::pressed, this, &RazerGenie::openWebsiteUrl);
            QPushButton *settingsButton = new QPushButton(tr("Open settings"));
            connect(settingsButton, &QPushButton::pressed, this, &RazerGenie::openPreferences);

            boxLayout->setAlignment(Qt::AlignTop);

            QFont titleFont("Arial", 18, QFont::Bold);
            titleLabel->setFont(titleFont);

            boxLayout->addWidget(titleLabel);
            boxLayout->addWidget(textLabel);
            boxLayout->addWidget(button);
            boxLayout->addWidget(settingsButton);
        } else if (daemonStatus == libopenrazer::DaemonStatus::NoSystemd) {
            auto *boxLayout = new QVBoxLayout(this);
            QLabel *titleLabel = new QLabel(tr("The OpenRazer daemon is not available."));
            QLabel *textLabel = new QLabel(tr("The OpenRazer daemon is not started and you are not using systemd as your init system.\nYou have to either start the daemon manually every time you log in or set up another method of autostarting the daemon.\n\nPlease consult the documentation for details."));
            QPushButton *settingsButton = new QPushButton(tr("Open settings"));
            connect(settingsButton, &QPushButton::pressed, this, &RazerGenie::openPreferences);

            boxLayout->setAlignment(Qt::AlignTop);

            QFont titleFont("Arial", 18, QFont::Bold);
            titleLabel->setFont(titleFont);

            boxLayout->addWidget(titleLabel);
            boxLayout->addWidget(textLabel);
            boxLayout->addWidget(settingsButton);
        } else { // Daemon status here can be enabled, unknown (and potentially disabled)
            auto *gridLayout = new QGridLayout(this);
            QLabel *label = new QLabel(tr("The OpenRazer daemon is currently not available. The status output is below."));
            auto *textEdit = new QTextEdit();
            QLabel *issueLabel = new QLabel(tr("If you think, there's a bug, you can report an issue on GitHub:"));
            QPushButton *issueButton = new QPushButton(tr("Report issue"));
            connect(issueButton, &QPushButton::pressed, this, &RazerGenie::openIssueUrl);
            QPushButton *settingsButton = new QPushButton(tr("Open settings"));
            connect(settingsButton, &QPushButton::pressed, this, &RazerGenie::openPreferences);

            textEdit->setReadOnly(true);
            textEdit->setText(manager->getDaemonStatusOutput());

            gridLayout->addWidget(label, 0, 1, 1, 3);
            gridLayout->addWidget(textEdit, 1, 1, 1, 3);
            gridLayout->addWidget(issueLabel, 2, 1);
            gridLayout->addWidget(issueButton, 2, 2);
            gridLayout->addWidget(settingsButton, 2, 3);
        }
        this->resize(1024, 600);
        this->setMinimumSize(QSize(800, 500));
        this->setWindowTitle("RazerGenie");
    } else {
        // Set up the normal UI
        setupUi();

        if (daemonStatus == libopenrazer::DaemonStatus::Disabled) {
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
        // TODO: Use correct DBus bus, based on information from libopenrazer
        QDBusServiceWatcher *watcher = new QDBusServiceWatcher("io.github.openrazer1", QDBusConnection::sessionBus());

        connect(watcher, &QDBusServiceWatcher::serviceRegistered,
                this, &RazerGenie::dbusServiceRegistered);
        connect(watcher, &QDBusServiceWatcher::serviceUnregistered,
                this, &RazerGenie::dbusServiceUnregistered);
    }
}

RazerGenie::~RazerGenie()
{
    QHashIterator<QDBusObjectPath, libopenrazer::Device *> i(devices);
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

    // Connect signals
    connect(ui_main.preferencesButton, &QPushButton::pressed, this, &RazerGenie::openPreferences);
    connect(ui_main.syncCheckBox, &QCheckBox::clicked, this, &RazerGenie::toggleSync);
    ui_main.syncCheckBox->setChecked(manager->getSyncEffects());
    connect(ui_main.screensaverCheckBox, &QCheckBox::clicked, this, &RazerGenie::toggleOffOnScreesaver);
    ui_main.screensaverCheckBox->setChecked(manager->getTurnOffOnScreensaver());

    connect(ui_main.listWidget, &QListWidget::currentRowChanged, ui_main.stackedWidget, &QStackedWidget::setCurrentIndex);

    manager->connectDevicesChanged(this, SLOT(devicesChanged()));
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
    // TODO: Show another placeholder screen with information that the daemon has been stopped?
    util::showError(tr("The D-Bus connection was lost, which probably means that the daemon has crashed."));
}

/**
 * Returns a list of connected devices, which are detected by Linux / lsusb. VID and PID are in decimal form.
 */
QList<QPair<int, int>> RazerGenie::getConnectedDevices_lsusb()
{
    // Get list of Razer devices connected to the PC: lsusb | grep '1532:' | cut -d' ' -f6
    QProcess process;
    process.start("bash", QStringList() << "-c"
                                        << "lsusb | grep '1532:' | cut -d' ' -f6");
    process.waitForFinished();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QStringList outputList = QString(process.readAllStandardOutput()).split("\n", Qt::SkipEmptyParts);
#else
    QStringList outputList = QString(process.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);
#endif

    QList<QPair<int, int>> returnList;

    // Transform the list ["1234:abcd", "5678:def0"] into a QList with QPairs.
    QStringListIterator i(outputList);
    while (i.hasNext()) {
        QStringList split = i.next().split(":");
        bool ok;
        // TODO: Check if count is 2? Otherwise SIGSEGV probably
        int vid = split[0].toInt(&ok, 16);
        int pid = split[1].toInt(&ok, 16);
        if (!ok) {
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

    if (devicePaths.size() == 0) {
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
    QMutableHashIterator<QDBusObjectPath, libopenrazer::Device *> i(devices);
    while (i.hasNext()) {
        i.next();
        if (devicePaths.contains(i.key())) {
            qDebug() << "Keep: " << i.key().path();
            devicePaths.removeOne(i.key());
        } else {
            libopenrazer::Device *dev = i.value();
            qDebug() << "Remove: " << i.key().path();
            devicePaths.removeOne(i.key());
            removeDeviceFromGui(i.key());
            devices.remove(i.key());
            delete dev;
        }
    }
    QListIterator<QDBusObjectPath> j(devicePaths);
    while (j.hasNext()) {
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
    for (int i = ui_main.stackedWidget->count(); i >= 0; i--) {
        QWidget *widget = ui_main.stackedWidget->widget(i);
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
    libopenrazer::Device *currentDevice = manager->getDevice(devicePath);

    // Setup variables for easy access
    QString type = currentDevice->getDeviceType();
    QString name = currentDevice->getDeviceName();

    if (devices.isEmpty()) {
        // Remove placeholder widget if inserted.
        ui_main.stackedWidget->removeWidget(ui_main.stackedWidget->widget(0));
    }

    // Add new device to the list
    auto *listItem = new QListWidgetItem();
    listItem->setSizeHint(QSize(/* any small width */ 1, 120));
    ui_main.listWidget->addItem(listItem);
    auto *listItemWidget = new DeviceListWidget(ui_main.listWidget, currentDevice);
    ui_main.listWidget->setItemWidget(listItem, listItemWidget);

    // Insert current device pointer with serial lookup into a QHash
    devices.insert(devicePath, currentDevice);

    // Download image for device
    if (!currentDevice->getDeviceImageUrl().isEmpty()) {
        RazerImageDownloader *dl = new RazerImageDownloader(QUrl(currentDevice->getDeviceImageUrl()), this);
        connect(dl, &RazerImageDownloader::downloadFinished, listItemWidget, &DeviceListWidget::imageDownloaded);
        connect(dl, &RazerImageDownloader::downloadErrored, listItemWidget, &DeviceListWidget::imageDownloadErrored);
        dl->startDownload();
    } else {
        qWarning() << "Device image for" << currentDevice->getDeviceName() << "is missing.";
        listItemWidget->setNoImage();
    }

    // Create a scroll widget for everything to sit in
    auto scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);

    /* Create actual DeviceWidget */
    auto *widget = new DeviceWidget(name, devicePath, currentDevice);

    // Set the main widget as child of the scrollArea
    scrollArea->setWidget(widget);

    // Add the new widget to the stacked widget
    ui_main.stackedWidget->addWidget(scrollArea);
}

bool RazerGenie::removeDeviceFromGui(const QDBusObjectPath &devicePath)
{
    int index = -1;
    for (int i = 0; i < ui_main.listWidget->count(); i++) {
        // get item for index
        QListWidgetItem *item = ui_main.listWidget->item(i);
        // get itemwidget for the item
        auto *widget = dynamic_cast<DeviceListWidget *>(ui_main.listWidget->itemWidget(item));
        // compare serial
        if (widget->device()->objectPath() == devicePath) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return false;
    }
    ui_main.stackedWidget->removeWidget(ui_main.stackedWidget->widget(index));
    delete ui_main.listWidget->takeItem(index);

    // Add placeholder widget if the stackedWidget is empty after removing.
    if (devices.isEmpty()) {
        ui_main.stackedWidget->addWidget(getNoDevicePlaceholder());
    }
    return true;
}

QWidget *RazerGenie::getNoDevicePlaceholder()
{
    if (noDevicePlaceholder != nullptr) {
        return noDevicePlaceholder;
    }
    // Generate placeholder widget with text "No device is connected.". Maybe add a usb pid check - at least add link to readme and troubleshooting page. Maybe add support for the future daemon troubleshooting option.

    QList<QPair<int, int>> connectedDevices = getConnectedDevices_lsusb();
    QList<QPair<int, int>> matches;

    // Don't even iterate if there are no devices detected by lsusb.
    if (connectedDevices.count() != 0) {
        QHashIterator<QString, QVariant> i(manager->getSupportedDevices());
        // Iterate through the supported devices
        while (i.hasNext()) {
            i.next();
            QList<QVariant> list = i.value().toList();
            if (list.count() != 2) {
                qWarning() << "RazerGenie: Error while iterating through supportedDevices";
                qWarning() << list;
                continue;
            }
            int vid = list[0].toInt();
            int pid = list[1].toInt();

            QListIterator<QPair<int, int>> j(connectedDevices);
            while (j.hasNext()) {
                QPair<int, int> x = j.next();
                if (x.first == vid && x.second == pid) {
                    qDebug() << "Found a device match!";
                    matches.append(x);
                }
            }
        }
    }

    noDevicePlaceholder = new QWidget();
    auto *boxLayout = new QVBoxLayout(noDevicePlaceholder);
    boxLayout->setAlignment(Qt::AlignTop);

    QFont headerFont("Arial", 15, QFont::Bold);
    QLabel *headerLabel;
    QLabel *textLabel;
    QPushButton *button1;
    QPushButton *button2;
    if (matches.size() == 0) {
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
    auto *hbox = new QHBoxLayout();
    hbox->addWidget(button1);
    hbox->addWidget(button2);
    boxLayout->addLayout(hbox);
    return noDevicePlaceholder;
}

void RazerGenie::toggleSync(bool sync)
{
    if (!manager->syncEffects(sync))
        util::showError(tr("Error while syncing devices."));
}

void RazerGenie::toggleOffOnScreesaver(bool on)
{
    if (!manager->setTurnOffOnScreensaver(on))
        util::showError(tr("Error while toggling 'turn off on screensaver'"));
}

void RazerGenie::openPreferences()
{
    auto *prefs = new Preferences(manager, this);
    prefs->setWindowModality(Qt::WindowModal);
    prefs->setAttribute(Qt::WA_DeleteOnClose);
    prefs->show();
}

void RazerGenie::devicesChanged()
{
    qInfo() << "DEVICE HAVE CHANGED!";
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
