// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "devicewidget.h"

#include "lightingwidget.h"
#include "performancewidget.h"
#include "powerwidget.h"

#include <QLabel>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>

DeviceWidget::DeviceWidget(libopenrazer::Device *device)
    : QWidget()
{
    auto *verticalLayout = new QVBoxLayout(this);

    QFont titleFont("Arial", 18, QFont::Bold);

    // Add header with the device name
    QLabel *header = new QLabel(device->getDeviceName(), this);
    header->setFont(titleFont);
    verticalLayout->addWidget(header);

    QTabWidget *tabWidget = new QTabWidget(this);

    /* Lighting tab */
    if (LightingWidget::isAvailable(device)) {
        auto widget = new LightingWidget(device);

        auto scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(widget);

        tabWidget->addTab(scrollArea, tr("Lighting"));
    }

    /* Performance tab */
    if (PerformanceWidget::isAvailable(device)) {
        auto widget = new PerformanceWidget(device);

        auto scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(widget);

        tabWidget->addTab(scrollArea, tr("Performance"));
    }

    /* Power tab */
    if (PowerWidget::isAvailable(device)) {
        auto widget = new PowerWidget(device);

        auto scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(widget);

        tabWidget->addTab(scrollArea, tr("Power"));
    }

    verticalLayout->addWidget(tabWidget);

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

DeviceWidget::~DeviceWidget() = default;
