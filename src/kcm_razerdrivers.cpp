/*
 * Copyright (C) 2016  Luca Weiss <luca (at) z3ntu (dot) xyz>
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
#include <QMessageBox>
#include <QColorDialog>
#include <QtDBus/QDBusConnection>

#include <KPluginFactory>
#include <KAboutData>
#include <KI18n/KLocalizedString>

#include <config.h>

#include "kcm_razerdrivers.h"
#include "razermethods.h"
#include "razerimagedownloader.h"
#include "devicedelegate.h"

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
        "Copyright (C) 2016 Luca Weiss",
        QString(),
        "https://github.com/z3ntu/kcm_razerdrivers",
        "https://github.com/z3ntu/kcm_razerdrivers/issues");
    about->addAuthor("Luca Weiss", "Main Developer", "luca@z3ntu.xyz");
    setAboutData(about);

    ui.setupUi(this);

    ui.versionLabel->setText("Driver version: " + razermethods::getDriverVersion());

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

    manager = new QNetworkAccessManager(this);

    foreach (const QString &serial, serialnrs) {

        std::cout << serial.toStdString() << std::endl;
        razermethods::Device *currentDevice = new razermethods::Device(serial);

        RazerImageDownloader *dl = new RazerImageDownloader(serial, QUrl("http://assets.razerzone.com/eeimages/products/17531/deathadder_chroma_gallery_2.png"), manager, this);
        connect(dl, &RazerImageDownloader::downloadFinished, this, &kcm_razerdrivers::imageDownloaded);
        downloaderList.append(dl);

        /* a */
        QPixmap *image = new QPixmap();
        bool success = image->load("/home/luca/Downloads/deathadder_chroma_gallery_2.png");
        std::cout << "Success: " << success << std::endl;

        QString type = currentDevice->getDeviceType();
        QString name = currentDevice->getDeviceName();

        devices.insert(serial, currentDevice);

        if(QString::compare(type, "mouse") == 0) {
            std::cout << "mouse" << std::endl;

            QWidget *mouseWidget = new QWidget();
            ui_mouse.setupUi(mouseWidget);

            connect(ui_mouse.logoColorButton, &QPushButton::clicked, this, &kcm_razerdrivers::logoColorButton);
            connect(ui_mouse.scrollColorButton, &QPushButton::clicked, this, &kcm_razerdrivers::scrollColorButton);
            // See http://doc.qt.io/qt-5/qcombobox.html#currentIndexChanged-1 for syntax
            connect(ui_mouse.scrollCombo, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::scrollCombo);
            connect(ui_mouse.logoCombo, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &kcm_razerdrivers::logoCombo);

            //for(int i=0; i<kcm_razerdrivers::Effects.size(); i++) {
            //ui_mouse.logoCombo->addItem();
            //}

            // TODO: Extend KPageWidgetItem to hold serial nr (to get in 'connect'ed methods)
            KPageWidgetItem *item = new KPageWidgetItem(mouseWidget, serial);
            QIcon *icon = new QIcon("/home/luca/Downloads/deathadder_chroma_gallery_2.png");
            item->setIcon(*icon);

            item->setHeader(name);

            ui.kpagewidget->addPage(item);

            //TODO: Create my own QAbstractItemDelegate

            //DeviceDelegate *delegate = new DeviceDelegate();
            //ui.kpagewidget->setItemDelegate(delegate);
        } else if(QString::compare(type, "keyboard") == 0) {
            //TODO: Handle
            std::cout << "keyboard" << std::endl;

            //TODO: keyboard
        } else if(QString::compare(type, "tartarus") == 0) {
            //TODO: Handle
            std::cout << "tartarus" << std::endl;

            //TODO: tartarus
        } else if(QString::compare(type, "headset") == 0) {
            std::cout << "headset" << std::endl;
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

void kcm_razerdrivers::scrollCombo(const QString &text)
{

}

void kcm_razerdrivers::logoCombo(const QString &text)
{
    std::cout << text.toStdString() << std::endl;
    std::cout << ui.kpagewidget->currentPage()->name().toStdString() << std::endl;
    razermethods::Device *device = devices.value(ui.kpagewidget->currentPage()->name());
    //TODO: Set real color
    device->setLogoStatic(255, 0, 255);
}

void kcm_razerdrivers::showError(QString error)
{
    QMessageBox messageBox;
    messageBox.critical(0, "Error!", error);
    messageBox.setFixedSize(500, 200);
}

#include "kcm_razerdrivers.moc"
