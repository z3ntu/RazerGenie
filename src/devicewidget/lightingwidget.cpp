// Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lightingwidget.h"

#include "clickeventfilter.h"
#include "customeditor/customeditor.h"
#include "ledwidget.h"

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

LightingWidget::LightingWidget(libopenrazer::Device *device)
    : QWidget()
{
    this->device = device;

    auto *verticalLayout = new QVBoxLayout(this);

    QFont headerFont("Arial", 15, QFont::Bold);

    QLabel *lightingHeader = new QLabel(tr("Lighting"), this);
    lightingHeader->setFont(headerFont);
    verticalLayout->addWidget(lightingHeader);

    /* Create LedWidget for all LEDs */
    for (libopenrazer::Led *led : device->getLeds()) {
        verticalLayout->addWidget(new LedWidget(this, led));
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
}

LightingWidget::~LightingWidget() = default;

bool LightingWidget::isAvailable(libopenrazer::Device *device)
{
    return !device->getLeds().isEmpty() || device->hasFeature("custom_frame");
}

void LightingWidget::openCustomEditor(bool forceFallback)
{
    /* Set combobox(es) to "Custom Effect" */
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
