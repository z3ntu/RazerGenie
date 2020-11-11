/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Luca Weiss <luca@z3ntu.xyz>
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
 */

#include "ledwidget.h"

#include "util.h"

#include <QColorDialog>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>

LedWidget::LedWidget(QWidget *parent, libopenrazer::Device *device, libopenrazer::Led *led)
    : QWidget(parent)
{
    this->mLed = led;

    auto *verticalLayout = new QVBoxLayout(this);

    // Set appropriate text
    QLabel *lightingLocationLabel = new QLabel(tr("Lighting %1").arg(libopenrazer::ledIdToStringTable.value(led->getLedId(), "error")));

    auto *lightingHBox = new QHBoxLayout();
    verticalLayout->addWidget(lightingLocationLabel);
    verticalLayout->addLayout(lightingHBox);

    auto *comboBox = new QComboBox;
    QLabel *brightnessLabel = nullptr;
    QSlider *brightnessSlider = nullptr;

    comboBox->setObjectName("combobox");
    comboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    //TODO Battery
    //TODO Sync effects in comboboxes & colorStuff when the sync checkbox is active

    razer_test::RazerEffect currentEffect = razer_test::RazerEffect::Static;
    try {
        currentEffect = led->getCurrentEffect();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get current effect");
    }
    QVector<razer_test::RGB> currentColors;
    try {
        currentColors = led->getCurrentColors();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get current colors");
    }

    // Add items from capabilities
    for (auto ledFx : libopenrazer::ledFxList) {
        if (led->hasFx(ledFx.getIdentifier())) {
            comboBox->addItem(ledFx.getDisplayString(), QVariant::fromValue(ledFx));
            // Set selection to current effect
            if (ledFx.getIdentifier() == currentEffect) {
                comboBox->setCurrentIndex(comboBox->count() - 1);
            }
        }
    }

    // Connect signal from combobox
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LedWidget::fxComboboxChanged);

    // Brightness slider
    if (led->hasFx("brightness")) {
        brightnessLabel = new QLabel(tr("Brightness"));
        brightnessSlider = new QSlider(Qt::Horizontal, this);
        brightnessSlider->setMaximum(255);
        uchar brightness;
        try {
            brightness = led->getBrightness();
        } catch (const libopenrazer::DBusException &e) {
            qWarning("Failed to get brightness");
            brightness = 100;
        }
        brightnessSlider->setValue(brightness);
        connect(brightnessSlider, &QSlider::valueChanged, this, &LedWidget::brightnessSliderChanged);
    }

    // Only add combobox if a capability was actually added
    if (comboBox->count() != 0) {
        lightingHBox->addWidget(comboBox);

        /* Color buttons */
        for (int i = 1; i <= 3; i++) {
            auto *colorButton = new QPushButton(this);
            QPalette pal = colorButton->palette();
            if (i - 1 < currentColors.count()) {
                razer_test::RGB color = currentColors.at(i - 1);
                pal.setColor(QPalette::Button, { color.r, color.g, color.b });
            } else {
                pal.setColor(QPalette::Button, QColor(Qt::green));
            }

            colorButton->setAutoFillBackground(true);
            colorButton->setFlat(true);
            colorButton->setPalette(pal);
            colorButton->setMaximumWidth(70);
            colorButton->setObjectName("colorbutton" + QString::number(i));
            lightingHBox->addWidget(colorButton);

            libopenrazer::RazerCapability capability = comboBox->currentData().value<libopenrazer::RazerCapability>();
            if (capability.getNumColors() < i)
                colorButton->hide();
            connect(colorButton, &QPushButton::clicked, this, &LedWidget::colorButtonClicked);
        }

        /* Wave left/right radio buttons */
        for (int i = 1; i <= 2; i++) {
            QString name;
            if (i == 1)
                name = tr("Left");
            else
                name = tr("Right");
            auto *radio = new QRadioButton(name, this);
            radio->setObjectName("radiobutton" + QString::number(i));
            if (i == 1) // set the 'left' checkbox to activated
                radio->setChecked(true);
            // hide by default
            radio->hide();
            lightingHBox->addWidget(radio);
            connect(radio, &QRadioButton::toggled, this, &LedWidget::waveRadioButtonChanged);
        }
    }

    /* Brightness sliders */
    if (brightnessLabel != nullptr && brightnessSlider != nullptr) { // only if brightness capability exists
        verticalLayout->addWidget(brightnessLabel);
        auto *hboxSlider = new QHBoxLayout();
        QLabel *brightnessSliderValue = new QLabel;
        hboxSlider->addWidget(brightnessSlider);
        hboxSlider->addWidget(brightnessSliderValue);
        verticalLayout->addLayout(hboxSlider);
    }
}

void LedWidget::colorButtonClicked()
{
    auto *sender = qobject_cast<QPushButton *>(QObject::sender());

    QPalette pal(sender->palette());

    QColor oldColor = pal.color(QPalette::Button);

    QColor color = QColorDialog::getColor(oldColor);
    if (color.isValid()) {
        pal.setColor(QPalette::Button, color);
        sender->setPalette(pal);
    } else {
        qInfo("User cancelled the dialog.");
    }
    applyEffect();
}

void LedWidget::fxComboboxChanged(int index)
{
    auto *sender = qobject_cast<QComboBox *>(QObject::sender());
    libopenrazer::RazerCapability capability = sender->itemData(index).value<libopenrazer::RazerCapability>();

    // Remove "Custom Effect" entry when you switch away from it - only gets added by the Custom Editor button
    if (sender->itemText(index) != "Custom Effect")
        sender->removeItem(sender->findText("Custom Effect"));

    // Show/hide the color buttons
    if (capability.getNumColors() == 0) { // hide all
        for (int i = 1; i <= 3; i++)
            findChild<QPushButton *>("colorbutton" + QString::number(i))->hide();
    } else {
        for (int i = 1; i <= 3; i++) {
            if (capability.getNumColors() < i)
                findChild<QPushButton *>("colorbutton" + QString::number(i))->hide();
            else
                findChild<QPushButton *>("colorbutton" + QString::number(i))->show();
        }
    }

    // Show/hide the wave radiobuttons
    if (capability.isWave() == 0) {
        findChild<QRadioButton *>("radiobutton1")->hide();
        findChild<QRadioButton *>("radiobutton2")->hide();
    } else {
        findChild<QRadioButton *>("radiobutton1")->show();
        findChild<QRadioButton *>("radiobutton2")->show();
    }

    applyEffectStandardLoc(capability.getIdentifier());
}

QColor LedWidget::getColorForButton(int num)
{
    QPalette pal = findChild<QPushButton *>("colorbutton" + QString::number(num))->palette();
    return pal.color(QPalette::Button);
}

razer_test::WaveDirection LedWidget::getWaveDirection()
{
    return findChild<QRadioButton *>("radiobutton1")->isChecked() ? razer_test::WaveDirection::RIGHT_TO_LEFT : razer_test::WaveDirection::LEFT_TO_RIGHT;
}

void LedWidget::brightnessSliderChanged(int value)
{
    try {
        mLed->setBrightness(value);
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to change brightness");
        util::showError(tr("Failed to change brightness"));
    }
}

void LedWidget::applyEffectStandardLoc(razer_test::RazerEffect fxStr)
{
    try {
        if (fxStr == razer_test::RazerEffect::Off) {
            mLed->setOff();
        } else if (fxStr == razer_test::RazerEffect::On) {
            mLed->setOn();
        } else if (fxStr == razer_test::RazerEffect::Static) {
            QColor c = getColorForButton(1);
            mLed->setStatic(c);
        } else if (fxStr == razer_test::RazerEffect::Breathing) {
            QColor c = getColorForButton(1);
            mLed->setBreathing(c);
        } else if (fxStr == razer_test::RazerEffect::BreathingDual) {
            QColor c1 = getColorForButton(1);
            QColor c2 = getColorForButton(2);
            mLed->setBreathingDual(c1, c2);
        } else if (fxStr == razer_test::RazerEffect::BreathingRandom) {
            mLed->setBreathingRandom();
        } else if (fxStr == razer_test::RazerEffect::Blinking) {
            QColor c = getColorForButton(1);
            mLed->setBlinking(c);
        } else if (fxStr == razer_test::RazerEffect::Spectrum) {
            mLed->setSpectrum();
        } else if (fxStr == razer_test::RazerEffect::Wave) {
            mLed->setWave(getWaveDirection());
        } else if (fxStr == razer_test::RazerEffect::Reactive) {
            QColor c = getColorForButton(1);
            mLed->setReactive(c, razer_test::ReactiveSpeed::_500MS); // TODO Configure speed?
        } else {
            // qWarning() << fxStr << " is not implemented yet!"; // FIXME
            qWarning("(insert fxstring here) is not implemented yet!");
        }
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to change effect");
        util::showError(tr("Failed to change effect"));
    }
}

void LedWidget::applyEffect()
{
    auto *combobox = findChild<QComboBox *>("combobox");

    libopenrazer::RazerCapability capability = combobox->itemData(combobox->currentIndex()).value<libopenrazer::RazerCapability>();

    applyEffectStandardLoc(capability.getIdentifier());
}

void LedWidget::waveRadioButtonChanged(bool enabled)
{
    if (enabled)
        applyEffect();
}

libopenrazer::Led *LedWidget::led()
{
    return mLed;
}
