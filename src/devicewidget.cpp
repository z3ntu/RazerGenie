/*
 * Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
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

#include "devicewidget.h"

#include "customeditor/customeditor.h"
#include "ledwidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTextEdit>
#include <QVBoxLayout>

DeviceWidget::DeviceWidget(const QString &name, const QDBusObjectPath &devicePath, libopenrazer::Device *device)
    : QWidget()
{
    this->name = name;
    this->devicePath = devicePath;
    this->device = device;

    auto *verticalLayout = new QVBoxLayout(this);

    // List of locations to iterate through
    QList<libopenrazer::Led *> leds = device->getLeds();

    // Declare header font
    QFont headerFont("Arial", 15, QFont::Bold);
    QFont titleFont("Arial", 18, QFont::Bold);

    // Add header with the device name
    QLabel *header = new QLabel(name, this);
    header->setFont(titleFont);
    verticalLayout->addWidget(header);

    // Lighting header
    if (leds.size() != 0) {
        QLabel *lightingHeader = new QLabel(tr("Lighting"), this);
        lightingHeader->setFont(headerFont);
        verticalLayout->addWidget(lightingHeader);
    }

    // Iterate through lighting locations
    foreach (libopenrazer::Led *led, leds) {
        verticalLayout->addWidget(new LedWidget(this, device, led));
    }

    /* DPI sliders */
    if (device->hasFeature("dpi")) {
        // HBoxes
        auto *dpiXHBox = new QHBoxLayout();
        auto *dpiYHBox = new QHBoxLayout();
        auto *dpiHeaderHBox = new QHBoxLayout();

        // Header
        QLabel *dpiHeader = new QLabel(tr("DPI"), this);
        dpiHeader->setFont(headerFont);
        dpiHeaderHBox->addWidget(dpiHeader);

        verticalLayout->addLayout(dpiHeaderHBox);

        // Labels
        QLabel *dpiXLabel = new QLabel(tr("DPI X"));
        QLabel *dpiYLabel = new QLabel(tr("DPI Y"));

        // Read-only textboxes
        auto *dpiXText = new QTextEdit(this);
        auto *dpiYText = new QTextEdit(this);
        dpiXText->setMaximumWidth(60);
        dpiYText->setMaximumWidth(60);
        dpiXText->setMaximumHeight(30);
        dpiYText->setMaximumHeight(30);
        dpiXText->setObjectName("dpiXText");
        dpiYText->setObjectName("dpiYText");
        dpiXText->setEnabled(false);
        dpiYText->setEnabled(false);

        // Sliders
        auto *dpiXSlider = new QSlider(Qt::Horizontal, this);
        auto *dpiYSlider = new QSlider(Qt::Horizontal, this);
        dpiXSlider->setObjectName("dpiX");
        dpiYSlider->setObjectName("dpiY");

        // Sync checkbox
        QLabel *dpiSyncLabel = new QLabel(tr("Lock X/Y"), this);
        auto *dpiSyncCheckbox = new QCheckBox(this);

        // Get the current DPI and set the slider&text
        razer_test::RazerDPI currDPI = device->getDPI();

        dpiXSlider->setValue(currDPI.dpi_x / 100);
        dpiYSlider->setValue(currDPI.dpi_y / 100);
        dpiXText->setText(QString::number(currDPI.dpi_x));
        dpiYText->setText(QString::number(currDPI.dpi_y));

        int maxDPI = device->maxDPI();
        dpiXSlider->setMaximum(maxDPI / 100);
        dpiYSlider->setMaximum(maxDPI / 100);

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

        connect(dpiXSlider, &QSlider::valueChanged, this, &DeviceWidget::dpiChanged);
        connect(dpiYSlider, &QSlider::valueChanged, this, &DeviceWidget::dpiChanged);
        connect(dpiSyncCheckbox, &QCheckBox::clicked, this, &DeviceWidget::dpiSyncCheckbox);

        verticalLayout->addLayout(dpiXHBox);
        verticalLayout->addLayout(dpiYHBox);
    }

    /* Poll rate */
    if (device->hasFeature("poll_rate")) {
        QLabel *pollRateHeader = new QLabel(tr("Polling rate"), this);
        pollRateHeader->setFont(headerFont);
        verticalLayout->addWidget(pollRateHeader);

        auto *pollComboBox = new QComboBox;
        pollComboBox->addItem("125 Hz", 125);
        pollComboBox->addItem("500 Hz", 500);
        pollComboBox->addItem("1000 Hz", 1000);
        pollComboBox->setCurrentText(QString::number(device->getPollRate()) + " Hz");
        verticalLayout->addWidget(pollComboBox);

        connect(pollComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeviceWidget::pollCombo);
    }

    /* Custom lighting */
    if (device->hasFeature("custom_frame")) {
        auto *button = new QPushButton(this);
        button->setText(tr("Open custom editor"));
        verticalLayout->addWidget(button);
        connect(button, &QPushButton::clicked, this, &DeviceWidget::openCustomEditor);
#ifdef INCLUDE_MATRIX_DISCOVERY
        QPushButton *buttonD = new QPushButton(this);
        buttonD->setText(tr("Launch matrix discovery"));
        verticalLayout->addWidget(buttonD);
        connect(buttonD, &QPushButton::clicked, this, [this] { openCustomEditor(true); });
#endif
    }

    /* Spacer to bottom */
    auto *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(spacer);

    /* Serial and firmware version labels */
    QLabel *serialLabel = new QLabel(tr("Serial number: %1").arg(device->getSerial()));
    verticalLayout->addWidget(serialLabel);

    QLabel *fwVerLabel = new QLabel(tr("Firmware version: %1").arg(device->getFirmwareVersion()));
    verticalLayout->addWidget(fwVerLabel);
}

QDBusObjectPath DeviceWidget::getDevicePath()
{
    return devicePath;
}

DeviceWidget::~DeviceWidget() = default;

void DeviceWidget::dpiChanged(int orig_value)
{
    ushort value = orig_value * 100;

    auto *sender = qobject_cast<QSlider *>(QObject::sender());

    // if DPI should be synced
    if (syncDpi) {
        if (sender->objectName() == "dpiX") {
            // set the other slider
            auto *slider = sender->parentWidget()->findChild<QSlider *>("dpiY");
            slider->setValue(orig_value);

            // set DPI
            device->setDPI({ value, value }); // set for both X & Y
        } else {
            // just set the slider (as the rest was done already or will be done)
            auto *slider = sender->parentWidget()->findChild<QSlider *>("dpiX");
            slider->setValue(orig_value);
        }
    } else { /* if DPI should NOT be synced */
        // set DPI (with value from other slider)
        if (sender->objectName() == "dpiX") {
            auto *slider = sender->parentWidget()->findChild<QSlider *>("dpiY");
            device->setDPI({ value, static_cast<ushort>(slider->value() * 100) });
        } else {
            auto *slider = sender->parentWidget()->findChild<QSlider *>("dpiX");
            device->setDPI({ static_cast<ushort>(slider->value() * 100), value });
        }
    }
    // Update textbox with new value
    auto *dpitextbox = sender->parentWidget()->findChild<QTextEdit *>(sender->objectName() + "Text");
    dpitextbox->setText(QString::number(value));
}

void DeviceWidget::dpiSyncCheckbox(bool checked)
{
    // TODO Sync DPI right here? Or just at next change (current behaviour)?
    syncDpi = checked;
}

void DeviceWidget::pollCombo(int /* index */)
{
    auto *sender = qobject_cast<QComboBox *>(QObject::sender());
    device->setPollRate(sender->currentData().value<ushort>());
}

void DeviceWidget::openCustomEditor(bool openMatrixDiscovery)
{
    // Set combobox(es) to "Custom Effect"
    auto comboboxes = this->findChildren<QComboBox *>("combobox");
    for (auto combobox : comboboxes) {
        if (combobox->findText("Custom Effect") == -1)
            combobox->addItem("Custom Effect");
        combobox->setCurrentText("Custom Effect");
    }

    auto *cust = new CustomEditor(device, openMatrixDiscovery);
    cust->setAttribute(Qt::WA_DeleteOnClose);
    cust->show();
}
