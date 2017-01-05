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
#include "librazer/razermethods.h"
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

    ui.versionLabel->setText("Daemon version: " + razermethods::getDaemonVersion());

    fillList();

    //Connect signals
    connect(ui.syncCheckBox, &QCheckBox::clicked, this, &kcm_razerdrivers::toggleSync);
}

kcm_razerdrivers::~kcm_razerdrivers()
{
//    delete ui;
}

void kcm_razerdrivers::fillList()
{
    QStringList serialnrs = razermethods::getConnectedDevices();

    foreach (const QString &serial, serialnrs) {

        std::cout << serial.toStdString() << std::endl;
        razermethods::Device *currentDevice = new razermethods::Device(serial);
        if(!currentDevice->getPngFilename().isEmpty()) {
            RazerImageDownloader *dl = new RazerImageDownloader(serial, QUrl("http://developer.razerzone.com/wp-content/uploads/" + currentDevice->getPngFilename()), this);
            connect(dl, &RazerImageDownloader::downloadFinished, this, &kcm_razerdrivers::imageDownloaded);
        } else {
            showInfo(".png mapping for device '" + currentDevice->getDeviceName() + "' missing.");
        }
        QString type = currentDevice->getDeviceType();
        QString name = currentDevice->getDeviceName();

        devices.insert(serial, currentDevice);
        if(QString::compare(type, "mug") == 0) {
            showInfo("This lucky bastard has a mug... :)");
        }
        if(QString::compare(type, "headset") == 0 || QString::compare(type, "mouse") == 0 || QString::compare(type, "mug") == 0) {
            std::cout << type.toStdString() << std::endl;
            QWidget *widget = new QWidget();
            QVBoxLayout *verticalLayout = new QVBoxLayout(widget);

            QList<razermethods::Device::lightingLocations> locationsTodo;

            if(currentDevice->hasCapability("lighting"))
                locationsTodo.append(razermethods::Device::lighting);
            if(currentDevice->hasCapability("lighting_logo"))
                locationsTodo.append(razermethods::Device::lighting_logo);
            if(currentDevice->hasCapability("lighting_scroll"))
                locationsTodo.append(razermethods::Device::lighting_scroll);

            while(locationsTodo.size() != 0) {
                razermethods::Device::lightingLocations currentLocation = locationsTodo.takeFirst();
                QLabel *text;
                if(currentLocation == razermethods::Device::lighting) {
                    text = new QLabel("Lighting");
                } else if(currentLocation == razermethods::Device::lighting_logo) {
                    text = new QLabel("Lighting Logo");
                } else if(currentLocation == razermethods::Device::lighting_scroll) {
                    text = new QLabel("Lighting Scroll");
                } else {
                    // Houston, we have a problem.
                }
                QHBoxLayout *hbox = new QHBoxLayout();
                verticalLayout->addWidget(text);
                verticalLayout->addLayout(hbox);
                QComboBox *comboBox = new QComboBox;
                QLabel *brightnessLabel;
                /* Declare here that you can 'connect' in the 'if' */
                QSlider *brightnessSlider = new QSlider(Qt::Horizontal, widget);

                // TODO: Maybe split again into lighting, lighting_logo & lighting_scroll, not all every time; we know what we are working with.
                for(int i=0; i<razermethods::allCapabilites.size(); i++) {
                    if(currentDevice->hasCapability(razermethods::allCapabilites[i].getIdentifier())) {
                        comboBox->addItem(razermethods::allCapabilites.at(i).getDisplayString(), QVariant::fromValue(razermethods::allCapabilites.at(i)));
//                         QVariant::fromValue(razermethods::allCapabilites.at(i));
//                         QVariant var;
//                         var.setValue(razermethods::allCapabilites.at(i));
                    }
                }

                if(currentLocation == razermethods::Device::lighting) {
                    // Connect signal
                    connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::standardCombo);

                    brightnessLabel = new QLabel("Brightness");
                    connect(brightnessSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::brightnessChanged);

                } else if(currentLocation == razermethods::Device::lighting_logo) {
                    // Connect signal
                    connect(comboBox, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::logoCombo);

                    brightnessLabel = new QLabel("Brightness Logo");
                    connect(brightnessSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::logoBrightnessChanged);

                } else if(currentLocation == razermethods::Device::lighting_scroll) {
                    // Connect signal
                    connect(comboBox, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::scrollCombo);

                    brightnessLabel = new QLabel("Brightness Scroll");
                    connect(brightnessSlider, &QSlider::valueChanged, this, &kcm_razerdrivers::scrollBrightnessChanged);
                }

                hbox->addWidget(comboBox);

                /* Color buttons TODO: Dual&Triple */
                QPushButton *colorButton = new QPushButton(widget);
                QPalette pal = colorButton->palette();
                // TODO: Set color when set & connect button; dynamic button creation?
                pal.setColor(QPalette::Button, QColor(Qt::green));
                colorButton->setAutoFillBackground(true);
                colorButton->setFlat(true);
                colorButton->setPalette(pal);
                colorButton->setMaximumWidth(70);
                hbox->addWidget(colorButton);

                /* Brightness sliders */
                verticalLayout->addWidget(brightnessLabel);
                QHBoxLayout *hboxSlider = new QHBoxLayout();
                QLabel *brightnessSliderValue = new QLabel;
                hboxSlider->addWidget(brightnessSlider);
                hboxSlider->addWidget(brightnessSliderValue);
                verticalLayout->addLayout(hboxSlider);
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
                QIcon *icon = new QIcon(RazerImageDownloader::getDownloadPath() + currentDevice->getPngFilename());
                item->setIcon(*icon);
            }
            item->setHeader(name);

            ui.kpagewidget->addPage(item);
        } else if(QString::compare(type, "keyboard") == 0) {
            //TODO: Handle
            std::cout << "keyboard" << std::endl;

            //TODO: keyboard
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

void kcm_razerdrivers::toggleSync(bool yes)
{
    if(!razermethods::syncDevices(yes))
        showError("Error while syncing devices.");
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

void kcm_razerdrivers::scrollCombo(const QString &/*text*/)
{

}

void kcm_razerdrivers::logoCombo(const QString &text)
{
    std::cout << text.toStdString() << std::endl;
//     std::cout << ui.kpagewidget->currentPage()->name().toStdString() << std::endl;
    //razermethods::Device *device = devices.value(((RazerPageWidgetItem*)ui.kpagewidget->currentPage())->getSerial());
    //TODO: Set real color
    //device->setLogoStatic(255, 0, 255);
}

void kcm_razerdrivers::standardCombo(int index)
{
    std::cout << index << std::endl;
    QComboBox *sender = qobject_cast<QComboBox*>(QObject::sender());
    razermethods::RazerCapability capability = sender->itemData(index).value<razermethods::RazerCapability>();
    std::cout << capability.getIdentifier().toStdString() << std::endl;
}


void kcm_razerdrivers::brightnessChanged(int value)
{
    std::cout << value << std::endl;
}

void kcm_razerdrivers::scrollBrightnessChanged(int value)
{
    std::cout << value << std::endl;

}

void kcm_razerdrivers::logoBrightnessChanged(int value)
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
