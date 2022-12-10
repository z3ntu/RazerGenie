/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Luca Weiss <luca@z3ntu.xyz>
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
 */

#include "dpisliderwidget.h"

#include "util.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QTextEdit>

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
    QLabel *dpiSyncLabel = new QLabel(tr("Lock X/Y"), this);
    auto *dpiSyncCheckbox = new QCheckBox(this);

    dpiHeaderHBox->addWidget(dpiHeader);
    dpiHeaderHBox->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    dpiHeaderHBox->addWidget(dpiSyncLabel);
    dpiHeaderHBox->addWidget(dpiSyncCheckbox);

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

    // Get the current DPI and set the slider&text
    openrazer::RazerDPI currDPI = { 0, 0 };
    try {
        currDPI = device->getDPI();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get dpi");
    }

    dpiXSlider->setValue(currDPI.dpi_x / 100);
    dpiYSlider->setValue(currDPI.dpi_y / 100);
    dpiXText->setText(QString::number(currDPI.dpi_x));
    dpiYText->setText(QString::number(currDPI.dpi_y));

    int maxDPI = 0;
    try {
        maxDPI = device->maxDPI();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get max dpi");
    }
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

    connect(dpiXSlider, &QSlider::valueChanged, this, &DpiSliderWidget::dpiChanged);
    connect(dpiYSlider, &QSlider::valueChanged, this, &DpiSliderWidget::dpiChanged);
    connect(dpiSyncCheckbox, &QCheckBox::clicked, this,
            [=](bool checked) { syncDpi = checked; });

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
            auto *slider = sender->parentWidget()->findChild<QSlider *>("dpiY");
            slider->setValue(orig_value);

            // set DPI
            dpi = { value, value }; // set for both X & Y
        } else {
            // just set the slider (as the rest was done already or will be done)
            auto *slider = sender->parentWidget()->findChild<QSlider *>("dpiX");
            slider->setValue(orig_value);
        }
    } else { /* if DPI should NOT be synced */
        // set DPI (with value from other slider)
        if (sender->objectName() == "dpiX") {
            auto *slider = sender->parentWidget()->findChild<QSlider *>("dpiY");
            dpi = { value, static_cast<ushort>(slider->value() * 100) };
        } else {
            auto *slider = sender->parentWidget()->findChild<QSlider *>("dpiX");
            dpi = { static_cast<ushort>(slider->value() * 100), value };
        }
    }

    // Update textbox with new value
    auto *dpitextbox = sender->parentWidget()->findChild<QTextEdit *>(sender->objectName() + "Text");
    dpitextbox->setText(QString::number(value));

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
