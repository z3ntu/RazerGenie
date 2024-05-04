// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "powerwidget.h"

#include "util.h"

#include <QLabel>
#include <QProgressBar>
#include <QSlider>
#include <QVBoxLayout>

PowerWidget::PowerWidget(libopenrazer::Device *device)
    : QWidget()
{
    this->device = device;

    auto *verticalLayout = new QVBoxLayout(this);

    QFont headerFont("Arial", 15, QFont::Bold);

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

    /* Low battery threshold / Enter low power at */
    if (device->hasFeature("low_battery_threshold")) {
        QLabel *lowBatteryThresholdHeader = new QLabel(tr("Enter lower power at"), this);
        lowBatteryThresholdHeader->setFont(headerFont);
        verticalLayout->addWidget(lowBatteryThresholdHeader);

        ushort threshold = 0;
        try {
            threshold = device->getLowBatteryThreshold();
        } catch (const libopenrazer::DBusException &e) {
            qWarning("Failed to get low battery threshold");
        }

        auto *lowBatteryThresholdHBox = new QHBoxLayout();

        auto *lowBatteryThresholdSlider = new QSlider(Qt::Horizontal, this);
        lowBatteryThresholdSlider->setMinimum(1);
        lowBatteryThresholdSlider->setMaximum(100);
        lowBatteryThresholdSlider->setValue(threshold);

        auto *lowBatteryThresholdLabel = new QLabel(this);
        lowBatteryThresholdLabel->setText(QString("%1%").arg(threshold));

        connect(lowBatteryThresholdSlider, &QSlider::valueChanged, this, [=](int threshold) {
            lowBatteryThresholdLabel->setText(QString("%1%").arg(threshold));

            try {
                device->setLowBatteryThreshold(threshold);
            } catch (const libopenrazer::DBusException &e) {
                qWarning("Failed to set low battery threshold");
                util::showError(tr("Failed to set low battery threshold"));
            }
        });

        lowBatteryThresholdHBox->addWidget(lowBatteryThresholdSlider);
        lowBatteryThresholdHBox->addWidget(lowBatteryThresholdLabel);
        verticalLayout->addLayout(lowBatteryThresholdHBox);
    }

    /* Spacer to bottom */
    auto *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(spacer);
}

PowerWidget::~PowerWidget() = default;

bool PowerWidget::isAvailable(libopenrazer::Device *device)
{
    return device->hasFeature("battery") || device->hasFeature("idle_time") || device->hasFeature("low_battery_threshold");
}
