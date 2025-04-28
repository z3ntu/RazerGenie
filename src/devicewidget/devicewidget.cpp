// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "devicewidget.h"

#include "deviceinfodialog.h"
#include "inputremappinginfodialog.h"
#include "lightingwidget.h"
#include "performancewidget.h"
#include "powerwidget.h"

#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QScrollArea>
#include <QStandardPaths>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>

DeviceWidget::DeviceWidget(libopenrazer::Device *device)
    : QWidget()
{
    auto *verticalLayout = new QVBoxLayout(this);

    QFont titleFont("Arial", 18, QFont::Bold);

    /* Header items */
    auto *headerHBox = new QHBoxLayout();

    QLabel *header = new QLabel(device->getDeviceName(), this);
    header->setFont(titleFont);
    headerHBox->addWidget(header);

    if (QStringList({ "keyboard", "keypad", "mouse" }).contains(device->getDeviceType())) {
        QPushButton *remapButton = new QPushButton();
        remapButton->setText(tr("Input remapping"));
        remapButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed));
        remapButton->setIcon(QIcon::fromTheme("applications-games-symbolic"));
        connect(remapButton, &QPushButton::clicked, this, [=]() {
            const QString inputRemapperName = "input-remapper-gtk";
            if (QStandardPaths::findExecutable(inputRemapperName).isEmpty()) {
                auto *info = new InputRemappingInfoDialog(this);
                info->setWindowModality(Qt::WindowModal);
                info->setAttribute(Qt::WA_DeleteOnClose);
                info->show();
            }

            // Disable the button for a bit to avoid double launches since
            // there's no immediate feedback
            remapButton->setEnabled(false);

            QProcess::startDetached(inputRemapperName);

            // Enable the button again after a small delay
            QTimer::singleShot(1000, [=]() {
                remapButton->setEnabled(true);
            });
        });
        headerHBox->addWidget(remapButton);
    }

    QPushButton *infoButton = new QPushButton();
    infoButton->setText(tr("Device Info"));
    infoButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed));
    infoButton->setIcon(QIcon::fromTheme("help-about-symbolic"));
    connect(infoButton, &QPushButton::clicked, this, [=]() {
        auto *info = new DeviceInfoDialog(device, this);
        info->setWindowModality(Qt::WindowModal);
        info->setAttribute(Qt::WA_DeleteOnClose);
        info->show();
    });
    headerHBox->addWidget(infoButton);

    verticalLayout->addLayout(headerHBox);

    /* Tabs */
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
}

DeviceWidget::~DeviceWidget() = default;
