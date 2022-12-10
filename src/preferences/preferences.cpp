// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "preferences.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <config.h>
#include <libopenrazer.h>

Preferences::Preferences(libopenrazer::Manager *manager, QWidget *parent)
    : QDialog(parent), manager(manager)
{
    setWindowTitle(tr("RazerGenie - Preferences"));

    QFont titleFont("Arial", 18, QFont::Bold);

    auto *vbox = new QVBoxLayout(this);

    QLabel *aboutLabel = new QLabel(this);
    aboutLabel->setText(tr("About:"));
    aboutLabel->setFont(titleFont);

    QLabel *razergenieVersionLabel = new QLabel(this);
    razergenieVersionLabel->setText(tr("RazerGenie Version: %1").arg(RAZERGENIE_VERSION));

    QLabel *openrazerVersionLabel = new QLabel(this);
    QString daemonVersion = "unknown";
    try {
        daemonVersion = manager->getDaemonVersion();
    } catch (const libopenrazer::DBusException &e) {
        qDebug() << "Failed to get daemon version:" << e.name() << e.message();
    }
    openrazerVersionLabel->setText(tr("OpenRazer Daemon Version: %1").arg(daemonVersion));

    QLabel *generalLabel = new QLabel(this);
    generalLabel->setText(tr("General:"));
    generalLabel->setFont(titleFont);

    QLabel *downloadText = new QLabel(this);
    downloadText->setText(tr("For displaying device images, RazerGenie downloads the image behind the URL specified for a device in the OpenRazer daemon source code. This will only be done for devices that are connected to the PC and only once, as the images are cached locally. For reviewing, what information Razer might collect with these connections, please consult the Razer Privacy Policy (https://www.razer.com/legal/privacy-policy)."));
    downloadText->setWordWrap(true);

    auto *downloadCheckBox = new QCheckBox(this);
    downloadCheckBox->setText(tr("Download device images"));
    downloadCheckBox->setChecked(settings.value("downloadImages").toBool());
    connect(downloadCheckBox, &QCheckBox::clicked, this, [=](bool checked) {
        settings.setValue("downloadImages", checked);
    });

    auto backendHbox = new QHBoxLayout();

    auto backendLabel = new QLabel(this);
    backendLabel->setText(tr("Daemon backend to use:"));

    auto *backendComboBox = new QComboBox(this);
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

    backendHbox->addWidget(backendLabel);
    backendHbox->addWidget(backendComboBox);

    auto *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vbox->addWidget(aboutLabel);
    vbox->addWidget(razergenieVersionLabel);
    vbox->addWidget(openrazerVersionLabel);
    vbox->addWidget(generalLabel);
    vbox->addWidget(downloadText);
    vbox->addWidget(downloadCheckBox);
    vbox->addLayout(backendHbox);
    vbox->addItem(spacer);

    this->resize(600, 400);
    this->setMinimumSize(QSize(400, 300));
    this->setWindowTitle(tr("RazerGenie - Preferences"));
}

Preferences::~Preferences() = default;
