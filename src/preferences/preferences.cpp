// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "preferences.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <config.h>
#include <libopenrazer.h>

Preferences::Preferences(libopenrazer::Manager *manager, QWidget *parent)
    : QDialog(parent), manager(manager)
{
    setWindowTitle(tr("RazerGenie - Preferences"));
    resize(600, 500);
    setMinimumSize(QSize(400, 300));

    QFont titleFont("Arial", 16, QFont::Bold);

    // Layout setup:
    // QDialog -> QVBoxLayout mainLayout -> QScrollArea scrollArea -> QWidget contentWidget -> QFormLayout formLayout

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

    QLabel *aboutLabel = new QLabel(contentWidget);
    aboutLabel->setText(tr("About"));
    aboutLabel->setFont(titleFont);
    aboutLabel->setAlignment(Qt::AlignHCenter);
    formLayout->addRow(aboutLabel);

    QFrame *aboutSeparator = new QFrame(this);
    aboutSeparator->setFrameShape(QFrame::HLine);
    aboutSeparator->setFrameShadow(QFrame::Sunken);
    formLayout->addRow(aboutSeparator);

    QLabel *razergenieVersionLabel = new QLabel(this);
    razergenieVersionLabel->setText(RAZERGENIE_VERSION);
    formLayout->addRow(tr("RazerGenie Version:"), razergenieVersionLabel);

    QLabel *openrazerVersionLabel = new QLabel(this);
    QString daemonVersion = "unknown";
    try {
        daemonVersion = manager->getDaemonVersion();
    } catch (const libopenrazer::DBusException &e) {
        qDebug() << "Failed to get daemon version:" << e.name() << e.message();
    }
    openrazerVersionLabel->setText(daemonVersion);
    formLayout->addRow(tr("OpenRazer Daemon Version:"), openrazerVersionLabel);

    QLabel *generalLabel = new QLabel(this);
    generalLabel->setText(tr("General"));
    generalLabel->setFont(titleFont);
    generalLabel->setAlignment(Qt::AlignHCenter);
    formLayout->addRow(generalLabel);

    QFrame *generalSeparator = new QFrame(this);
    generalSeparator->setFrameShape(QFrame::HLine);
    generalSeparator->setFrameShadow(QFrame::Sunken);
    formLayout->addRow(generalSeparator);

    QCheckBox *downloadCheckBox = new QCheckBox(this);
    downloadCheckBox->setText(tr("Download device images"));
    downloadCheckBox->setChecked(settings.value("downloadImages").toBool());
    connect(downloadCheckBox, &QCheckBox::clicked, this, [=](bool checked) {
        settings.setValue("downloadImages", checked);
    });
    formLayout->addRow(tr("Device images:"), downloadCheckBox);

    QLabel *downloadText = new QLabel(this);
    downloadText->setText(tr("For displaying device images, RazerGenie downloads the image behind "
                             "the URL specified for a device in the OpenRazer daemon source code. "
                             "This will only be done for devices that are connected to the PC and "
                             "only once, as the images are cached locally. For reviewing, what "
                             "information Razer might collect with these connections, please "
                             "consult the <a href=\"https://www.razer.com/legal/privacy-policy\">"
                             "Razer Privacy Policy</a>."));
    downloadText->setOpenExternalLinks(true);
    downloadText->setWordWrap(true);
    formLayout->addRow(nullptr, downloadText);

    QCheckBox *noAutostartCheckBox = new QCheckBox(this);
    noAutostartCheckBox->setText(tr("Ask to auto-start daemon on startup"));
    noAutostartCheckBox->setChecked(settings.value("askAutostartDaemon", true).toBool());
    connect(noAutostartCheckBox, &QCheckBox::clicked, this, [=](bool checked) {
        settings.setValue("askAutostartDaemon", checked);
    });
    formLayout->addRow(tr("Daemon auto-start:"), noAutostartCheckBox);

    QComboBox *backendComboBox = new QComboBox(this);
    backendComboBox->addItem("OpenRazer");
    backendComboBox->addItem("razer_test");
    backendComboBox->setCurrentText(settings.value("backend").toString());
    connect(backendComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        settings.setValue("backend", backendComboBox->itemText(index));
        // Show popup to restart program
        QMessageBox msgBox;
        msgBox.setText(tr("Please restart the application for the switch to take effect."));
        msgBox.addButton(QMessageBox::Ok);
        msgBox.exec();
    });
    formLayout->addRow(tr("Daemon backend:"), backendComboBox);
}

Preferences::~Preferences() = default;
