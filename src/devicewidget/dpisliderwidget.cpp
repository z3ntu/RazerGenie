// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dpisliderwidget.h"

#include "util.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

DpiSliderWidget::DpiSliderWidget(QWidget *parent, libopenrazer::Device *device)
    : QWidget(parent)
{
    this->device = device;

    // The widget seems to get big spacing in some cases without this size policy
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    QFont headerFont("Arial", 15, QFont::Bold);

    auto *dpiHeaderHBox = new QHBoxLayout();

    // Header
    QLabel *dpiHeader = new QLabel(tr("DPI"));
    dpiHeader->setFont(headerFont);
    dpiHeaderHBox->addWidget(dpiHeader);

    dpiHeaderHBox->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    if (device->hasFeature("dpi_stages")) {
        auto *dpiStagesCheckbox = new QCheckBox();
        dpiStagesCheckbox->setText(tr("Enable stages"));
        dpiStagesCheckbox->setChecked(true); // TODO: determine based on something
        singleStage = false; // TODO: set from checkbox value above
        dpiHeaderHBox->addWidget(dpiStagesCheckbox);

        connect(dpiStagesCheckbox, &QCheckBox::clicked, this, [=](bool checked) {
            /* Show/hide stage 2-5 */
            for (int i = 1; i < dpiStageWidgets.size(); i++) {
                if (checked) {
                    dpiStageWidgets[i]->show();
                } else {
                    dpiStageWidgets[i]->hide();
                }
            }

            dpiStageWidgets[0]->setSingleStage(!checked);
            singleStage = !checked;
        });
    }

    // Sync checkbox
    auto *dpiSyncCheckbox = new QCheckBox();
    dpiSyncCheckbox->setText(tr("Lock X/Y"));
    dpiHeaderHBox->addWidget(dpiSyncCheckbox);
    verticalLayout->addLayout(dpiHeaderHBox);

    connect(dpiSyncCheckbox, &QCheckBox::clicked, this, [=](bool checked) {
        for (DpiStageWidget *stageWidget : dpiStageWidgets) {
            stageWidget->setSyncDpi(checked);
        }
    });

    // DPI stages
    const int minimumDpi = 100;

    int maximumDpi = 0;
    try {
        maximumDpi = device->maxDPI();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get max dpi");
    }

    if (device->hasFeature("dpi_stages")) {
        QPair<uchar, QVector<openrazer::DPI>> stagesPair = { 1, {} };
        try {
            stagesPair = device->getDPIStages();
        } catch (const libopenrazer::DBusException &e) {
            qWarning("Failed to get dpi stages");
        }
        activeStage = stagesPair.first;
        dpiStages = stagesPair.second;

        // Assume user wants DPI synced if all values are currently equal
        bool isSynced = true;
        for (openrazer::DPI dpi : dpiStages) {
            isSynced &= dpi.dpi_x == dpi.dpi_y;
        }
        dpiSyncCheckbox->setChecked(isSynced);

        /* Create widgets for the 5 possible DPI stages */
        for (int stageNumber = 1; stageNumber <= 5; stageNumber++) {
            /* Makes sure we have a DPI stage for every value - 0/0 if not provided */
            if (dpiStages.size() < stageNumber) {
                dpiStages.append({ 0, 0 });
            }
            /* Get DPI for current stage */
            openrazer::DPI dpi = dpiStages[stageNumber - 1];

            auto *stageWidget = new DpiStageWidget(stageNumber, minimumDpi, maximumDpi, dpi, activeStage == stageNumber);
            stageWidget->setSyncDpi(isSynced);

            connect(stageWidget, &DpiStageWidget::stageActivated, this, [=](int stageNumber) {
                activeStage = stageNumber;
                for (DpiStageWidget *widget : dpiStageWidgets) {
                    widget->informStageActive(activeStage);
                }

                device->setDPIStages(activeStage, dpiStages);
            });

            connect(stageWidget, &DpiStageWidget::dpiChanged, this, [=](int stageNumber, openrazer::DPI dpi) {
                handleStageUpdates();

                /* Apply to device */
                if (singleStage) {
                    device->setDPI(dpi);
                } else {
                    /* If the currently active stage was disabled, we need to
                     * find a new one to enable */
                    if (dpi.dpi_x == 0 && dpi.dpi_y == 0 && stageNumber == activeStage) {
                        activeStage = 1;
                        for (DpiStageWidget *widget : dpiStageWidgets) {
                            widget->informStageActive(activeStage);
                        }
                    }

                    device->setDPIStages(activeStage, dpiStages);
                }
            });

            verticalLayout->addWidget(stageWidget);

            dpiStageWidgets.append(stageWidget);
        }

        handleStageUpdates();
    } else {
        openrazer::DPI currentDpi = { 0, 0 };
        try {
            currentDpi = device->getDPI();
        } catch (const libopenrazer::DBusException &e) {
            qWarning("Failed to get dpi");
        }

        // Assume user wants DPI synced if both values are currently equal
        bool isSynced = currentDpi.dpi_x == currentDpi.dpi_y;
        dpiSyncCheckbox->setChecked(isSynced);

        auto *stageWidget = new DpiStageWidget(0, minimumDpi, maximumDpi, currentDpi, false);
        stageWidget->setSingleStage(true);
        stageWidget->setSyncDpi(isSynced);
        connect(stageWidget, &DpiStageWidget::dpiChanged, this, [=](int /*stageNumber*/, openrazer::DPI dpi) {
            device->setDPI(dpi);
        });

        verticalLayout->addWidget(stageWidget);

        dpiStageWidgets.append(stageWidget);
    }
}

void DpiSliderWidget::handleStageUpdates()
{
    /* Re-number the stages to account for disabled stages, re-init dpiStages
     * array based on new values */
    int stageNumber = 1;
    dpiStages.clear();
    for (DpiStageWidget *stageWidget : dpiStageWidgets) {
        openrazer::DPI dpi = stageWidget->getDpi();
        if (dpi.dpi_x != 0 && dpi.dpi_y != 0)
            dpiStages.insert(stageNumber - 1, dpi);

        /* Set the stage number, if it returns false the stage is currently
         * disabled and we'll try the same number on the next stage */
        if (stageWidget->setStageNumber(stageNumber)) {
            stageNumber++;
        }
    }

    /* Disable "Enable" button if last stage active */
    int nrDisabledStages = 0;
    for (openrazer::DPI dpiStage : dpiStages) {
        if (dpiStage.dpi_x == 0 && dpiStage.dpi_y == 0) {
            nrDisabledStages++;
        }
    }
    bool lastStage = nrDisabledStages == dpiStages.size() - 1;
    for (DpiStageWidget *stageWidget : dpiStageWidgets) {
        stageWidget->informLastStage(lastStage);
    }
}
