// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dpicomboboxwidget.h"

#include "util.h"

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

DpiComboBoxWidget::DpiComboBoxWidget(QWidget *parent, libopenrazer::Device *device)
    : QWidget(parent)
{
    this->device = device;

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    // Declare header font
    QFont headerFont("Arial", 15, QFont::Bold);

    QLabel *dpiHeader = new QLabel(tr("DPI"), this);
    dpiHeader->setFont(headerFont);
    verticalLayout->addWidget(dpiHeader);

    QComboBox *dpiComboBox = new QComboBox;
    QVector<ushort> allowedDPI = device->getAllowedDPI();
    for (ushort dpi : allowedDPI) {
        dpiComboBox->addItem(QString("%1 DPI").arg(dpi), dpi);
    }

    openrazer::RazerDPI currDPI = { 0, 0 };
    try {
        currDPI = device->getDPI();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get dpi");
    }

    dpiComboBox->setCurrentText(QString("%1 DPI").arg(currDPI.dpi_x));
    verticalLayout->addWidget(dpiComboBox);

    connect(dpiComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DpiComboBoxWidget::dpiChanged);
}

void DpiComboBoxWidget::dpiChanged(int /* index */)
{
    auto *sender = qobject_cast<QComboBox *>(QObject::sender());
    try {
        device->setDPI({ sender->currentData().value<ushort>(), 0 });
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to set DPI");
        util::showError(tr("Failed to set DPI"));
    }
}
