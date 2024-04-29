// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "devicewidget.h"

#include "clickeventfilter.h"
#include "customeditor/customeditor.h"
#include "dpicomboboxwidget.h"
#include "dpisliderwidget.h"
#include "ledwidget.h"
#include "util.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
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

    /* Battery */
    if (device->hasFeature("battery")) {
        auto *batteryHeaderHBox = new QHBoxLayout();

        QLabel *batterHeader = new QLabel(tr("Battery"), this);
        batterHeader->setFont(headerFont);

        bool charging = false;
        try {
            charging = device->isCharging();
        } catch (const libopenrazer::DBusException &e) {
            qWarning("Failed to get charging status");
        }

        QLabel *chargingLabel = new QLabel(this);
        if (charging) {
            chargingLabel->setText(tr("Charging"));
        } else {
            chargingLabel->setText(tr("Not Charging"));
        }

        batteryHeaderHBox->addWidget(batterHeader);
        batteryHeaderHBox->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        batteryHeaderHBox->addWidget(chargingLabel);

        verticalLayout->addLayout(batteryHeaderHBox);

        double percent = 0.0;
        try {
            percent = device->getBatteryPercent();
        } catch (const libopenrazer::DBusException &e) {
            qWarning("Failed to get battery charge percentage");
        }

        auto *progressBar = new QProgressBar;
        progressBar->setValue(percent);

        verticalLayout->addWidget(progressBar);
    }

    /* Idle time / Sleep mode after */
    if (device->hasFeature("idle_time")) {
        QLabel *idleTimeHeader = new QLabel(tr("Sleep mode after"), this);
        idleTimeHeader->setFont(headerFont);
        verticalLayout->addWidget(idleTimeHeader);

        ushort idleTimeSec = 0;
        try {
            idleTimeSec = device->getIdleTime();
        } catch (const libopenrazer::DBusException &e) {
            qWarning("Failed to get idle time");
        }

        auto *idleTimeHBox = new QHBoxLayout();

        auto *idleTimeSlider = new QSlider(Qt::Horizontal, this);
        idleTimeSlider->setTickInterval(1);
        idleTimeSlider->setTickPosition(QSlider::TickPosition::TicksBelow);
        idleTimeSlider->setMinimum(1);
        idleTimeSlider->setMaximum(15);
        idleTimeSlider->setPageStep(1);
        idleTimeSlider->setValue(idleTimeSec / 60);

        auto *idleTimeLabel = new QLabel(this);
        idleTimeLabel->setText(tr("%1 minutes").arg(idleTimeSec / 60));

        connect(idleTimeSlider, &QSlider::valueChanged, this, [=](int idleTimeMin) {
            idleTimeLabel->setText(tr("%1 minutes").arg(idleTimeMin));

            try {
                device->setIdleTime(idleTimeMin * 60);
            } catch (const libopenrazer::DBusException &e) {
                qWarning("Failed to set idle time");
                util::showError(tr("Failed to set idle time"));
            }
        });

        idleTimeHBox->addWidget(idleTimeSlider);
        idleTimeHBox->addWidget(idleTimeLabel);
        verticalLayout->addLayout(idleTimeHBox);
    }

    // Lighting header
    if (leds.size() != 0) {
        QLabel *lightingHeader = new QLabel(tr("Lighting"), this);
        lightingHeader->setFont(headerFont);
        verticalLayout->addWidget(lightingHeader);
    }

    // Iterate through lighting locations
    for (libopenrazer::Led *led : leds) {
        verticalLayout->addWidget(new LedWidget(this, led));
    }

    /* DPI sliders */
    if (device->hasFeature("dpi")) {
        if (device->hasFeature("restricted_dpi")) {
            verticalLayout->addWidget(new DpiComboBoxWidget(this, device));
        } else {
            verticalLayout->addWidget(new DpiSliderWidget(this, device));
        }
    }

    /* Poll rate */
    if (device->hasFeature("poll_rate")) {
        QLabel *pollRateHeader = new QLabel(tr("Polling rate"), this);
        pollRateHeader->setFont(headerFont);
        verticalLayout->addWidget(pollRateHeader);

        ushort pollRate = 0;
        try {
            pollRate = device->getPollRate();
        } catch (const libopenrazer::DBusException &e) {
            qWarning("Failed to get poll rate");
        }

        QVector<ushort> supportedPollRates;
        try {
            supportedPollRates = device->getSupportedPollRates();
        } catch (const libopenrazer::DBusException &e) {
            qWarning("Failed to get supported poll rates");
        }

        auto *pollComboBox = new QComboBox;
        for (ushort poll : supportedPollRates) {
            pollComboBox->addItem(QString::number(poll) + " Hz", poll);
        }
        pollComboBox->setCurrentText(QString::number(pollRate) + " Hz");
        verticalLayout->addWidget(pollComboBox);

        connect(pollComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeviceWidget::pollCombo);
    }

    /* Custom lighting */
    if (device->hasFeature("custom_frame")) {
        auto *button = new QPushButton(this);
        button->setText(tr("Open custom editor"));

        // Make Shift-Click open the custom editor with fallback layout
        ClickEventFilter *filter = new ClickEventFilter();
        button->installEventFilter(filter);

        verticalLayout->addWidget(button);

        connect(filter, &ClickEventFilter::shiftClicked,
                [=]() { openCustomEditor(true); });
        connect(button, &QPushButton::clicked,
                [=]() { openCustomEditor(false); });
    }

    /* Spacer to bottom */
    auto *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(spacer);

    /* Serial and firmware version labels */
    QString serial = "error";
    try {
        serial = device->getSerial();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get serial");
    }
    QString firmwareVersion = "error";
    try {
        firmwareVersion = device->getFirmwareVersion();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get firmware version");
    }

    QLabel *serialLabel = new QLabel(tr("Serial number: %1").arg(serial));
    verticalLayout->addWidget(serialLabel);

    QLabel *fwVerLabel = new QLabel(tr("Firmware version: %1").arg(firmwareVersion));
    verticalLayout->addWidget(fwVerLabel);
}

QDBusObjectPath DeviceWidget::getDevicePath()
{
    return devicePath;
}

DeviceWidget::~DeviceWidget() = default;

void DeviceWidget::pollCombo(int /* index */)
{
    auto *sender = qobject_cast<QComboBox *>(QObject::sender());
    try {
        device->setPollRate(sender->currentData().value<ushort>());
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to set polling rate");
        util::showError(tr("Failed to set polling rate"));
    }
}

void DeviceWidget::openCustomEditor(bool forceFallback)
{
    // Set combobox(es) to "Custom Effect"
    auto comboboxes = this->findChildren<QComboBox *>("combobox");
    for (auto combobox : comboboxes) {
        if (combobox->findText("Custom Effect") == -1)
            combobox->addItem("Custom Effect");
        combobox->setCurrentText("Custom Effect");
    }

    auto *cust = new CustomEditor(device, forceFallback);
    cust->setAttribute(Qt::WA_DeleteOnClose);
    cust->show();
}
