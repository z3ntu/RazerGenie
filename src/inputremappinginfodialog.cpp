// Copyright (C) 2018-2024  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "inputremappinginfodialog.h"

#include <QFormLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

InputRemappingInfoDialog::InputRemappingInfoDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("RazerGenie - Input remapping"));
    resize(500, 300);
    setMinimumSize(QSize(400, 200));

    QFont titleFont("Arial", 16, QFont::Bold);

    QWidget *contentWidget = new QWidget(this);
    contentWidget->setMaximumWidth(550);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidget(contentWidget);
    scrollArea->setAlignment(Qt::AlignHCenter);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(scrollArea);

    QFormLayout *formLayout = new QFormLayout(contentWidget);

    QLabel *titleLabel = new QLabel(contentWidget);
    titleLabel->setText(tr("Input remapping"));
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignHCenter);
    formLayout->addRow(titleLabel);

    QFrame *titleSeparator = new QFrame(this);
    titleSeparator->setFrameShape(QFrame::HLine);
    titleSeparator->setFrameShadow(QFrame::Sunken);
    formLayout->addRow(titleSeparator);

    QLabel *infoText = new QLabel(this);
    infoText->setText(tr("OpenRazer and RazerGenie do not support input "
                         "remapping since it's not Razer-specific functionality."
                         "<br><br>"
                         "We recommend <a href=\"https://github.com/sezanzeb/input-remapper\">"
                         "input-remapper</a> which supports combinations, programmable "
                         "macros and much more with all of your input devices!"));
    infoText->setOpenExternalLinks(true);
    infoText->setWordWrap(true);
    formLayout->addRow(infoText);
}

InputRemappingInfoDialog::~InputRemappingInfoDialog() = default;
