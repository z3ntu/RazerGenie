/*
 * Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
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
 *
 */

#include "preferences.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <libopenrazer.h>
#include <config.h>

Preferences::Preferences(QWidget *parent)
    : QDialog(parent)
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
    // FIXME reenable
    // openrazerVersionLabel->setText(tr("OpenRazer Daemon Version: %1").arg(libopenrazer::getDaemonVersion()));

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

    auto *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vbox->addWidget(aboutLabel);
    vbox->addWidget(razergenieVersionLabel);
    vbox->addWidget(openrazerVersionLabel);
    vbox->addWidget(generalLabel);
    vbox->addWidget(downloadText);
    vbox->addWidget(downloadCheckBox);
    vbox->addItem(spacer);

    this->resize(600, 400);
    this->setMinimumSize(QSize(400, 300));
    this->setWindowTitle(tr("RazerGenie - Preferences"));
}

Preferences::~Preferences() = default;
