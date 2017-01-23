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
#include <QtWidgets>

#include <KPluginFactory>
#include <KAboutData>
#include <KI18n/KLocalizedString>

#include <config.h>

#include "kcm_razerdrivers.h"
#include "librazer/librazer.h"
#include "librazer/razercapability.h"
#include "razerimagedownloader.h"
#include "razerpagewidgetitem.h"

K_PLUGIN_FACTORY(RazerDriversKcmFactory, registerPlugin<kcm_razerdrivers>();)

kcm_razerdrivers::kcm_razerdrivers(QWidget* parent, const QVariantList& args) : KCModule(parent, args)
{
    // About dialog
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
    ui.setupUi(this);

    ui.versionLabel->setText("Daemon version: " + librazer::getDaemonVersion());

    fillList();

    //Connect signals
    connect(ui.syncCheckBox, &QCheckBox::clicked, this, &kcm_razerdrivers::toggleSync);
    ui.syncCheckBox->setChecked(librazer::getSyncEffects());
    connect(ui.screensaverCheckBox, &QCheckBox::clicked, this, &kcm_razerdrivers::toggleOffOnScreesaver);
    ui.screensaverCheckBox->setChecked(librazer::getTurnOffOnScreensaver());
}

kcm_razerdrivers::~kcm_razerdrivers()
{
//    delete ui;
}

void kcm_razerdrivers::fillList()
{
    QStringList serialnrs = librazer::getConnectedDevices();

    foreach (const QString &serial, serialnrs) {

        librazer::Device *currentDevice = new librazer::Device(serial);
        if(!currentDevice->getPngFilename().isEmpty()) {
            RazerImageDownloader *dl = new RazerImageDownloader(serial, QUrl(currentDevice->getPngUrl()), this);
            connect(dl, &RazerImageDownloader::downloadFinished, this, &kcm_razerdrivers::imageDownloaded);
        } else {
            showInfo(".png mapping for device '" + currentDevice->getDeviceName() + "' (PID "+QString::number(currentDevice->getPid())+") missing.");
        }
        QString type = currentDevice->getDeviceType();
        QString name = currentDevice->getDeviceName();

        std::cout << serial.toStdString() << std::endl;
        std::cout << name.toStdString() << std::endl;

        devices.insert(serial, currentDevice);
        if(QString::compare(type, "mug") == 0) {
            showInfo("This lucky bastard has a mug... :)");
        }                                                  // TODO: Get rid of keyboard here ? or whole if probably
        if(type=="headset" || type=="mouse" || type=="mug" || type=="keyboard") {
            std::cout << type.toStdString() << std::endl;
            QWidget *widget = new QWidget();
            QVBoxLayout *verticalLayout = new QVBoxLayout(widget);

            QList<librazer::Device::lightingLocations> locationsTodo;

            if(currentDevice->hasCapability("lighting"))
                locationsTodo.append(librazer::Device::lighting);
            if(currentDevice->hasCapability("lighting_logo"))
                locationsTodo.append(librazer::Device::lighting_logo);
            if(currentDevice->hasCapability("lighting_scroll"))
                locationsTodo.append(librazer::Device::lighting_scroll);

            while(locationsTodo.size() != 0) {
                librazer::Device::lightingLocations currentLocation = locationsTodo.takeFirst();
                QLabel *text;
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

                if(currentLocation == librazer::Device::lighting) {
                    for(int i=0; i<librazer::lightingComboBoxCapabilites.size(); i++) {
                        if(currentDevice->hasCapability(librazer::lightingComboBoxCapabilites[i].getIdentifier())) {
                            comboBox->addItem(librazer::lightingComboBoxCapabilites[i].getDisplayString(), QVariant::fromValue(librazer::lightingComboBoxCapabilites[i]));
                        }
                    }

                    // Connect signal
                    connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::standardCombo);

                    if(currentDevice->hasCapability("brightness")) {
                        brightnessLabel = new QLabel("Brightness");
                        brightnessSlider = new QSlider(Qt::Horizontal, widget);
                        if(currentDevice->hasCapability("get_brightness")) {
                            brightnessSlider->setValue(currentDevice->getBrightness());
                        }
                        connect(brightnessSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::brightnessChanged);
                    }

                } else if(currentLocation == librazer::Device::lighting_logo) {
                    for(int i=0; i<librazer::logoComboBoxCapabilites.size(); i++) {
                        if(currentDevice->hasCapability(librazer::logoComboBoxCapabilites[i].getIdentifier())) {
                            comboBox->addItem(librazer::logoComboBoxCapabilites[i].getDisplayString(), QVariant::fromValue(librazer::logoComboBoxCapabilites[i]));
                        }
                    }

                    // Connect signal
                    connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::logoCombo);

                    if(currentDevice->hasCapability("lighting_logo_brightness")) {
                        brightnessLabel = new QLabel("Brightness Logo");
                        brightnessSlider = new QSlider(Qt::Horizontal, widget);
                        if(currentDevice->hasCapability("get_lighting_logo_brightness")) {
                            brightnessSlider->setValue(currentDevice->getLogoBrightness());
                        }
                        connect(brightnessSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::logoBrightnessChanged);
                    }

                } else if(currentLocation == librazer::Device::lighting_scroll) {
                    for(int i=0; i<librazer::scrollComboBoxCapabilites.size(); i++) {
                        if(currentDevice->hasCapability(librazer::scrollComboBoxCapabilites[i].getIdentifier())) {
                            comboBox->addItem(librazer::scrollComboBoxCapabilites[i].getDisplayString(), QVariant::fromValue(librazer::scrollComboBoxCapabilites[i]));
                        }
                    }

                    // Connect signal
                    connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::scrollCombo);

                    if(currentDevice->hasCapability("lighting_scroll_brightness")) {
                        brightnessLabel = new QLabel("Brightness Scroll");
                        brightnessSlider = new QSlider(Qt::Horizontal, widget);
                        if(currentDevice->hasCapability("get_lighting_scroll_brightness")) {
                            brightnessSlider->setValue(currentDevice->getScrollBrightness());
                        }
                        connect(brightnessSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::scrollBrightnessChanged);
                    }
                }

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

            ui.kpagewidget->addPage(item);
        } else if(QString::compare(type, "tartarus") == 0) {
            //TODO: Handle
            std::cout << "tartarus" << std::endl;

            //TODO: tartarus
        } else {
            showError("Unknown device type: " + type + " for serial " + serial + "! Please contact the author.");
        }
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

void kcm_razerdrivers::standardColorButton()
{
    std::cout << "color dialog" << std::endl;

    QColor color = QColorDialog::getColor(Qt::white);
    std::cout << color.name().toStdString() << std::endl;
}

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

void kcm_razerdrivers::scrollCombo(int index)
{

}

void kcm_razerdrivers::logoCombo(int index)
{
    //std::cout << text.toStdString() << std::endl;
//     std::cout << ui.kpagewidget->currentPage()->name().toStdString() << std::endl;
    //librazer::Device *device = devices.value(((RazerPageWidgetItem*)ui.kpagewidget->currentPage())->getSerial());
    //TODO: Set real color
    //device->setLogoStatic(255, 0, 255);
}

void kcm_razerdrivers::standardCombo(int index)
{
    std::cout << "Standard Combo called" << std::endl;
    std::cout << index << std::endl;
    QComboBox *sender = qobject_cast<QComboBox*>(QObject::sender());
    librazer::RazerCapability capability = sender->itemData(index).value<librazer::RazerCapability>();
    QString identifier = capability.getIdentifier();
    std::cout << identifier.toStdString() << std::endl;
    std::cout << capability.getDisplayString().toStdString() << std::endl;
    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui.kpagewidget->currentPage());
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

    if(identifier == "lighting_breath_single") {
        dev->setBreathSingle(0, 255, 0); // TODO Color
    } else if(identifier == "lighting_breath_dual") {
        dev->setBreathDual(0, 255, 0, 255, 255, 0); // TODO Color
    } else if(identifier == "lighting_breath_triple") {
        //dev->setBreathTriple(0, 255, 0); // TODO Color TODO Implement method
    } else if(identifier == "lighting_breath_random") {
        dev->setBreathRandom();
    } else if(identifier == "lighting_wave") {
        dev->setWave(librazer::WAVE_RIGHT); // TODO: 1/2 or 0/1 for left/right TODO Left/right button
    } else if(identifier == "lighting_reactive") {
        dev->setReactive(0, 255, 0, 2); // TODO Color
    } else if(identifier == "lighting_none") {
        dev->setNone();
    } else if(identifier == "lighting_spectrum") {
        dev->setSpectrum();
    } else if(identifier == "lighting_static") {
        dev->setStatic(255, 0, 255); // TODO Color
    } else if(identifier == "lighting_ripple") {
        //dev->setRipple TODO Implement method
    } else if(identifier == "lighting_ripple_random") {
        //dev->setRippleRandom(); TODO Implement method
    } else if(identifier == "lighting_pulsate") {
        //dev->setPulsate(); TODO Implement method
    } else {
        std::cout << identifier.toStdString() << " is not implemented yet!" << std::endl;
    }
}

void kcm_razerdrivers::brightnessChanged(int value)
{
    std::cout << value << std::endl;

    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui.kpagewidget->currentPage());
    librazer::Device *dev = devices.value(item->getSerial());
    dev->setBrightness(value);
}

void kcm_razerdrivers::scrollBrightnessChanged(int value)
{
    std::cout << value << std::endl;

    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui.kpagewidget->currentPage());
    librazer::Device *dev = devices.value(item->getSerial());
    dev->setScrollBrightness(value);
}

void kcm_razerdrivers::logoBrightnessChanged(int value)
{
    std::cout << value << std::endl;

    RazerPageWidgetItem *item = dynamic_cast<RazerPageWidgetItem*>(ui.kpagewidget->currentPage());
    librazer::Device *dev = devices.value(item->getSerial());
    dev->setLogoBrightness(value);
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
