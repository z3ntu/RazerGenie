// Copyright (C) 2018-2024  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dpistagewidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

DpiStageWidget::DpiStageWidget(int initialStageNumber, int minimumDpi, int maximumDpi, openrazer::DPI currentDpi, bool activeStage, QWidget *parent)
    : QWidget(parent)
{
    this->stageNumber = initialStageNumber;

    auto *dpiStageHBox = new QHBoxLayout(this);

    dpiStageButton = new QPushButton();
    dpiStageButton->setText(QString::number(stageNumber));
    dpiStageButton->setCheckable(true);
    dpiStageButton->setChecked(activeStage);
    dpiStageButton->setMaximumWidth(40);
    dpiStageHBox->addWidget(dpiStageButton);

    connect(dpiStageButton, &QPushButton::clicked, [=](bool checked) {
        /* Disregard unchecking - invalid operation */
        if (!checked) {
            dpiStageButton->setChecked(true);
            return;
        }

        emit stageActivated(stageNumber);
    });

    // HBoxes
    auto *dpiXHBox = new QHBoxLayout();
    auto *dpiYHBox = new QHBoxLayout();

    // Labels
    dpiXLabel = new QLabel("X");
    dpiYLabel = new QLabel("Y");

    // Spinboxes
    dpiXSpinBox = new QSpinBox();
    dpiYSpinBox = new QSpinBox();
    dpiXSpinBox->setEnabled(false);
    dpiYSpinBox->setEnabled(false);

    // Sliders
    dpiXSlider = new QSlider(Qt::Horizontal);
    dpiYSlider = new QSlider(Qt::Horizontal);

    dpiXSpinBox->setMinimum(minimumDpi);
    dpiYSpinBox->setMinimum(minimumDpi);
    dpiXSlider->setMinimum(minimumDpi / 100);
    dpiYSlider->setMinimum(minimumDpi / 100);

    dpiXSpinBox->setMaximum(maximumDpi);
    dpiYSpinBox->setMaximum(maximumDpi);
    dpiXSlider->setMaximum(maximumDpi / 100);
    dpiYSlider->setMaximum(maximumDpi / 100);

    dpiXSpinBox->setValue(currentDpi.dpi_x);
    dpiYSpinBox->setValue(currentDpi.dpi_y);
    dpiXSlider->setValue(currentDpi.dpi_x / 100);
    dpiYSlider->setValue(currentDpi.dpi_y / 100);

    dpiXSlider->setTickInterval(10);
    dpiYSlider->setTickInterval(10);
    dpiXSlider->setTickPosition(QSlider::TickPosition::TicksBelow);
    dpiYSlider->setTickPosition(QSlider::TickPosition::TicksBelow);

    // updateXYVisibility();

    auto *dpiSliderVBox = new QVBoxLayout();
    dpiStageHBox->addLayout(dpiSliderVBox);

    enableCheckBox = new QCheckBox();
    enableCheckBox->setText(tr("Enable"));
    enableCheckBox->setChecked(currentDpi.dpi_x != 0);
    dpiStageHBox->addWidget(enableCheckBox);

    updateEnabled(currentDpi.dpi_x != 0);

    connect(enableCheckBox, &QCheckBox::clicked, this, [=](bool checked) {
        updateEnabled(checked);

        /* Emit changed DPI indicating it's now enabled/disabled */
        emitDpiChanged();
    });

    dpiXHBox->addWidget(dpiXLabel);
    dpiXHBox->addWidget(dpiXSpinBox);
    dpiXHBox->addWidget(dpiXSlider);
    dpiSliderVBox->addLayout(dpiXHBox);

    dpiYHBox->addWidget(dpiYLabel);
    dpiYHBox->addWidget(dpiYSpinBox);
    dpiYHBox->addWidget(dpiYSlider);
    dpiSliderVBox->addLayout(dpiYHBox);

    connect(dpiXSlider, &QSlider::valueChanged, this, [=](int sliderValue) {
        if (syncDpi) {
            dpiYSlider->setValue(sliderValue);
            emitDpiChanged();
        } else {
            emitDpiChanged();
        }
        dpiXSpinBox->setValue(sliderValue * 100);
    });
    connect(dpiYSlider, &QSlider::valueChanged, this, [=](int sliderValue) {
        if (syncDpi) {
            dpiXSlider->setValue(sliderValue);
        } else {
            emitDpiChanged();
        }
        dpiYSpinBox->setValue(sliderValue * 100);
    });
}

void DpiStageWidget::setSyncDpi(bool syncDpi)
{
    this->syncDpi = syncDpi;

    /* Snap the Y value to the current X value when the box is checked */
    int dpiXSliderValue = dpiXSlider->value();
    if (syncDpi && dpiXSliderValue != dpiYSlider->value()) {
        dpiYSlider->setValue(dpiXSliderValue);
        emitDpiChanged();
    }

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

void DpiStageWidget::setSingleStage(bool singleStage)
{
    if (singleStage) {
        dpiStageButton->hide();
        enableCheckBox->hide();
        enableCheckBox->setChecked(true);
        updateEnabled(true);
        emitDpiChanged();
    } else {
        dpiStageButton->show();
        enableCheckBox->show();
    }
}

void DpiStageWidget::informStageActive(int activeStage)
{
    dpiStageButton->setChecked(activeStage == stageNumber && dpiStageButton->isEnabled());
}

void DpiStageWidget::informLastStage(bool lastStage)
{
    /* We need to disable the checkbox if we're still enabled */
    if (lastStage && enableCheckBox->isChecked()) {
        enableCheckBox->setEnabled(false);
    } else {
        enableCheckBox->setEnabled(true);
    }
}

bool DpiStageWidget::setStageNumber(int stageNumber)
{
    if (!enableCheckBox->isChecked())
        return false;

    this->stageNumber = stageNumber;
    dpiStageButton->setText(QString::number(stageNumber));
    return true;
}

openrazer::DPI DpiStageWidget::getDpi()
{
    openrazer::DPI dpi = { 0, 0 };

    /* Only provide the value if the stage is enabled */
    if (enableCheckBox->isChecked())
        dpi = { static_cast<ushort>(dpiXSlider->value() * 100),
                static_cast<ushort>(dpiYSlider->value() * 100) };

    return dpi;
}

void DpiStageWidget::emitDpiChanged()
{
    openrazer::DPI dpi = getDpi();
    emit dpiChanged(stageNumber, dpi);
}

void DpiStageWidget::updateEnabled(bool enabled)
{
    dpiXSlider->setEnabled(enabled);
    dpiYSlider->setEnabled(enabled);
    dpiStageButton->setEnabled(enabled);

    if (enabled) {
        dpiStageButton->setText(QString::number(stageNumber));
    } else {
        dpiStageButton->setText("");
    }
}
