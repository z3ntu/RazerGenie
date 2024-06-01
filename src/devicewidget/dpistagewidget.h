// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DPISTAGEWIDGET_H
#define DPISTAGEWIDGET_H

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>
#include <libopenrazer.h>

class DpiStageWidget : public QWidget
{
    Q_OBJECT
public:
    DpiStageWidget(int initialStageNumber, int minimumDpi, int maximumDpi, openrazer::DPI currentDpi, bool activeStage, QWidget *parent = nullptr);

    /* Tell the stage that syncing DPI is active - so the Y axis can get hidden */
    void setSyncDpi(bool syncDpi);
    /* Tell the stage that it's being used as single-stage DPI widget */
    void setSingleStage(bool singleStage);
    /* Inform the stage which stage should be active */
    void informStageActive(int activeStage);
    /* Inform the stage that it's the last one that's active and needs to
     * disable the 'Enable' button because it needs to stay active */
    void informLastStage(bool lastStage);
    /* Set the number of this stage - but returns false if it's currently
     * disabled */
    bool setStageNumber(int stageNumber);
    /* Return the currently selected DPI - if the stage is disabled then pass
     * the DPI {0,0} */
    openrazer::DPI getDpi();

signals:
    /* The DPI of this stage have changed - if the stage is disabled then
     * pass the DPI {0,0} */
    void dpiChanged(int stageNumber, openrazer::DPI dpi);
    /* This stage has been activated */
    void stageActivated(int stageNumber);

private:
    QPushButton *dpiStageButton;
    QCheckBox *enableCheckBox;

    QLabel *dpiXLabel;
    QLabel *dpiYLabel;
    QSpinBox *dpiXSpinBox;
    QSpinBox *dpiYSpinBox;
    QSlider *dpiXSlider;
    QSlider *dpiYSlider;

    bool syncDpi;
    int stageNumber;

    void emitDpiChanged();
    void updateEnabled(bool enabled);
};

#endif // DPISTAGEWIDGET_H
