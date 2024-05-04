// Copyright (C) 2018-2024  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "deviceinfodialog.h"

#include <QFormLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

DeviceInfoDialog::DeviceInfoDialog(libopenrazer::Device *device, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("RazerGenie - Device info"));
    resize(400, 200);
    setMinimumSize(QSize(400, 200));

    QFont titleFont("Arial", 16, QFont::Bold);

    // Layout setup:
    // QDialog -> QVBoxLayout mainLayout -> QScrollArea scrollArea -> QWidget contentWidget -> QFormLayout formLayout

    QWidget *contentWidget = new QWidget(this);
    contentWidget->setMaximumWidth(300);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidget(contentWidget);
    scrollArea->setAlignment(Qt::AlignHCenter);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(scrollArea);

    QFormLayout *formLayout = new QFormLayout(contentWidget);

    QLabel *aboutLabel = new QLabel(contentWidget);
    aboutLabel->setText(tr("About"));
    aboutLabel->setFont(titleFont);
    aboutLabel->setAlignment(Qt::AlignHCenter);
    formLayout->addRow(aboutLabel);

    QFrame *aboutSeparator = new QFrame(this);
    aboutSeparator->setFrameShape(QFrame::HLine);
    aboutSeparator->setFrameShadow(QFrame::Sunken);
    formLayout->addRow(aboutSeparator);

    /* Serial number */
    QString serial = "error";
    try {
        serial = device->getSerial();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get serial");
    }
    QLabel *serialLabel = new QLabel(this);
    serialLabel->setText(serial);
    serialLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    formLayout->addRow(tr("Serial number:"), serialLabel);

    /* Firmware version */
    QString firmwareVersion = "error";
    try {
        firmwareVersion = device->getFirmwareVersion();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get firmware version");
    }
    QLabel *firmwareVersionLabel = new QLabel(this);
    firmwareVersionLabel->setText(firmwareVersion);
    firmwareVersionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    formLayout->addRow(tr("Firmware version:"), firmwareVersionLabel);
}

DeviceInfoDialog::~DeviceInfoDialog() = default;
