// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dpisliderwidget.h"

#include "util.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

DpiSliderWidget::DpiSliderWidget(QWidget *parent, libopenrazer::Device *device)
    : QWidget(parent)
{
    this->device = device;

    // The widget seems to get big spacing in some cases without this size policy
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    // Declare header font
    QFont headerFont("Arial", 15, QFont::Bold);

    // HBoxes
    auto *dpiXHBox = new QHBoxLayout();
    auto *dpiYHBox = new QHBoxLayout();
    auto *dpiHeaderHBox = new QHBoxLayout();

    // Header
    QLabel *dpiHeader = new QLabel(tr("DPI"), this);
    dpiHeader->setFont(headerFont);

    // Sync checkbox
    auto *dpiSyncCheckbox = new QCheckBox(this);
    dpiSyncCheckbox->setText(tr("Lock X/Y"));

    dpiHeaderHBox->addWidget(dpiHeader);
    dpiHeaderHBox->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    dpiHeaderHBox->addWidget(dpiSyncCheckbox);

    // Labels
    dpiXLabel = new QLabel("X");
    dpiYLabel = new QLabel("Y");

    // Read-only spinboxes
    dpiXSpinBox = new QSpinBox(this);
    dpiYSpinBox = new QSpinBox(this);
    dpiXSpinBox->setObjectName("dpiXSpinBox");
    dpiYSpinBox->setObjectName("dpiYSpinBox");
    dpiXSpinBox->setEnabled(false);
    dpiYSpinBox->setEnabled(false);

    // Sliders
    dpiXSlider = new QSlider(Qt::Horizontal, this);
    dpiYSlider = new QSlider(Qt::Horizontal, this);
    dpiXSlider->setObjectName("dpiX");
    dpiYSlider->setObjectName("dpiY");

    // Get the current DPI and set the slider&text
    openrazer::RazerDPI currDPI = { 0, 0 };
    try {
        currDPI = device->getDPI();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get dpi");
    }

    const int minimumDPI = 100;
    dpiXSpinBox->setMinimum(minimumDPI);
    dpiYSpinBox->setMinimum(minimumDPI);
    dpiXSlider->setMinimum(minimumDPI / 100);
    dpiYSlider->setMinimum(minimumDPI / 100);

    int maxDPI = 0;
    try {
        maxDPI = device->maxDPI();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get max dpi");
    }
    dpiXSpinBox->setMaximum(maxDPI);
    dpiYSpinBox->setMaximum(maxDPI);
    dpiXSlider->setMaximum(maxDPI / 100);
    dpiYSlider->setMaximum(maxDPI / 100);

    dpiXSlider->setTickInterval(10);
    dpiYSlider->setTickInterval(10);
    dpiXSlider->setTickPosition(QSlider::TickPosition::TicksBelow);
    dpiYSlider->setTickPosition(QSlider::TickPosition::TicksBelow);

    dpiXSpinBox->setValue(currDPI.dpi_x);
    dpiYSpinBox->setValue(currDPI.dpi_y);
    dpiXSlider->setValue(currDPI.dpi_x / 100);
    dpiYSlider->setValue(currDPI.dpi_y / 100);

    dpiSyncCheckbox->setChecked(syncDpi);
    updateXYVisibility();

    dpiXHBox->addWidget(dpiXLabel);
    dpiXHBox->addWidget(dpiXSpinBox);
    dpiXHBox->addWidget(dpiXSlider);

    dpiYHBox->addWidget(dpiYLabel);
    dpiYHBox->addWidget(dpiYSpinBox);
    dpiYHBox->addWidget(dpiYSlider);

    connect(dpiXSlider, &QSlider::valueChanged, this, &DpiSliderWidget::dpiChanged);
    connect(dpiYSlider, &QSlider::valueChanged, this, &DpiSliderWidget::dpiChanged);
    connect(dpiSyncCheckbox, &QCheckBox::clicked, this, [=](bool checked) {
        syncDpi = checked;
        updateXYVisibility();

        /* Snap the Y value to the current X value when the box is checked */
        if (checked) {
            dpiYSlider->setValue(dpiXSlider->value());
            /* Set DPI on the device manually, the dpiChanged function doesn't
             * do that for just setting Y with syncDpi=true */
            ushort dpi = dpiXSlider->value() * 100;
            device->setDPI({ dpi, dpi });
        }
    });

    verticalLayout->addLayout(dpiHeaderHBox);
    verticalLayout->addLayout(dpiXHBox);
    verticalLayout->addLayout(dpiYHBox);
}

void DpiSliderWidget::dpiChanged(int orig_value)
{
    ushort value = orig_value * 100;

    auto *sender = qobject_cast<QSlider *>(QObject::sender());

    openrazer::RazerDPI dpi = { 0, 0 };

    // if DPI should be synced
    if (syncDpi) {
        if (sender->objectName() == "dpiX") {
            // set the other slider
            dpiYSlider->setValue(orig_value);

            // set DPI
            dpi = { value, value }; // set for both X & Y
        } else {
            // just set the slider (as the rest was done already or will be done)
            dpiXSlider->setValue(orig_value);
        }
    } else { /* if DPI should NOT be synced */
        // set DPI (with value from other slider)
        if (sender->objectName() == "dpiX") {
            dpi = { value, static_cast<ushort>(dpiYSlider->value() * 100) };
        } else {
            dpi = { static_cast<ushort>(dpiXSlider->value() * 100), value };
        }
    }

    // Update spinbox with new value
    if (sender->objectName() == "dpiX") {
        dpiXSpinBox->setValue(value);
    } else {
        dpiYSpinBox->setValue(value);
    }

    // Check if we need to actually set the DPI
    if (dpi.dpi_x == 0 && dpi.dpi_y == 0) {
        return;
    }

    try {
        device->setDPI(dpi);
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to set dpi");
        util::showError(tr("Failed to set dpi"));
    }
}

void DpiSliderWidget::updateXYVisibility()
{
    if (syncDpi) {
        dpiXLabel->hide();

        dpiYLabel->hide();
        dpiYSpinBox->hide();
        dpiYSlider->hide();
    } else {
        dpiXLabel->show();

        dpiYLabel->show();
        dpiYSpinBox->show();
        dpiYSlider->show();
    }
}
