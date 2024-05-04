// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "performancewidget.h"

#include "dpicomboboxwidget.h"
#include "dpisliderwidget.h"
#include "util.h"

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

PerformanceWidget::PerformanceWidget(libopenrazer::Device *device)
    : QWidget()
{
    this->device = device;

    auto *verticalLayout = new QVBoxLayout(this);

    QFont headerFont("Arial", 15, QFont::Bold);

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

        connect(pollComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PerformanceWidget::pollCombo);
    }

    /* Spacer to bottom */
    auto *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(spacer);
}

PerformanceWidget::~PerformanceWidget() = default;

bool PerformanceWidget::isAvailable(libopenrazer::Device *device)
{
    return device->hasFeature("dpi") || device->hasFeature("poll_rate");
}

void PerformanceWidget::pollCombo(int /* index */)
{
    auto *sender = qobject_cast<QComboBox *>(QObject::sender());
    try {
        device->setPollRate(sender->currentData().value<ushort>());
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to set polling rate");
        util::showError(tr("Failed to set polling rate"));
    }
}
