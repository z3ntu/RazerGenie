// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ledwidget.h"

#include "util.h"

#include <QApplication>
#include <QColorDialog>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <stdexcept>

LedWidget::LedWidget(QWidget *parent, libopenrazer::Led *led)
    : QWidget(parent)
{
    this->mLed = led;

    auto *verticalLayout = new QVBoxLayout(this);

    // Set appropriate text
    QString lightingLocation = qApp->translate("libopenrazer", libopenrazer::ledIdToStringTable.value(led->getLedId(), "error"));
    QLabel *lightingLocationLabel = new QLabel(tr("Effect %1").arg(lightingLocation));

    auto *lightingHBox = new QHBoxLayout();
    verticalLayout->addWidget(lightingLocationLabel);
    verticalLayout->addLayout(lightingHBox);

    auto *comboBox = new QComboBox;

    comboBox->setObjectName("combobox");
    comboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    // TODO Sync effects in comboboxes & colorStuff when the sync checkbox is active

    openrazer::Effect currentEffect = openrazer::Effect::Static;
    try {
        currentEffect = led->getCurrentEffect();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get current effect");
    }
    QVector<openrazer::RGB> currentColors;
    try {
        currentColors = led->getCurrentColors();
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to get current colors");
    }

    // Add items from capabilities
    for (auto ledFx : libopenrazer::ledFxList) {
        if (led->hasFx(ledFx.getIdentifier())) {
            comboBox->addItem(qApp->translate("libopenrazer", ledFx.getDisplayString()), QVariant::fromValue(ledFx));
            // Set selection to current effect
            if (ledFx.getIdentifier() == currentEffect) {
                comboBox->setCurrentIndex(comboBox->count() - 1);
            }
        }
    }

    // Connect signal from combobox
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LedWidget::fxComboboxChanged);

    // Only add combobox if a capability was actually added
    if (comboBox->count() != 0) {
        lightingHBox->addWidget(comboBox);

        /* Color buttons */
        for (int i = 1; i <= 3; i++) {
            auto *colorButton = new QPushButton(this);
            QPalette pal = colorButton->palette();
            if (i - 1 < currentColors.count()) {
                openrazer::RGB color = currentColors.at(i - 1);
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

            libopenrazer::Capability capability = comboBox->currentData().value<libopenrazer::Capability>();
            if (capability.getNumColors() < i)
                colorButton->hide();
            connect(colorButton, &QPushButton::clicked, this, &LedWidget::colorButtonClicked);
        }

        /* Wave & wheel radio buttons */
        for (int i = 1; i <= 2; i++) {
            QString name;
            // TODO: Use Clockwise & Counterclockwise labels for Wheel
            // TODO: Also use Clockwise & Counterclockwise for mousepads
            if (i == 1)
                name = tr("Left");
            else
                name = tr("Right");
            auto *radio = new QRadioButton(name, this);
            radio->setObjectName("radiobutton" + QString::number(i));
            if (i == 1) // set the 'left' checkbox to activated
                radio->setChecked(true);
            // Hide radio button when we don't need it
            if (currentEffect != openrazer::Effect::Wave && currentEffect != openrazer::Effect::Wheel) {
                radio->hide();
            }
            lightingHBox->addWidget(radio);
            connect(radio, &QRadioButton::toggled, this, [=](bool enabled) {
                if (enabled)
                    applyEffect();
            });
        }
    } else {
        // Otherwise delete comboBox again
        delete comboBox;
        comboBox = nullptr;
    }

    /* Brightness slider */
    if (led->hasBrightness()) {
        auto *brightnessLabel = new QLabel(tr("Brightness"));

        auto *brightnessSlider = new QSlider(Qt::Horizontal, this);
        brightnessSlider->setMaximum(255);

        auto *brightnessSliderValue = new QLabel;

        uchar brightness;
        try {
            brightness = led->getBrightness();
        } catch (const libopenrazer::DBusException &e) {
            qWarning("Failed to get brightness");
            brightness = 100;
        }

        brightnessSlider->setValue(brightness);
        brightnessSliderValue->setText(QString("%1%").arg(brightness * 100 / 255));

        connect(brightnessSlider, &QSlider::valueChanged, this, [=](int value) {
            brightnessSliderValue->setText(QString("%1%").arg(value * 100 / 255));

            try {
                mLed->setBrightness(value);
            } catch (const libopenrazer::DBusException &e) {
                qWarning("Failed to change brightness");
                util::showError(tr("Failed to change brightness"));
            }
        });

        verticalLayout->addWidget(brightnessLabel);

        auto *hboxSlider = new QHBoxLayout();
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
    libopenrazer::Capability capability;

    /* In theory we could remove half of this special handling because
     * .value<>() will give us a default Capability anyways if it's
     * missing. But to be explicit let's do it like this. */
    bool isCustomEffect = sender->itemText(index) == "Custom Effect";
    if (!isCustomEffect) {
        QVariant itemData = sender->itemData(index);
        if (!itemData.canConvert<libopenrazer::Capability>())
            throw new std::runtime_error("Expected to be able to convert itemData into Capability");
        capability = itemData.value<libopenrazer::Capability>();
    } else {
        /* We're fine with getting an empty Capability as we do want to
         * reset all the extra buttons etc. We just don't want to actually do
         * more than UI work with this though. */
        capability = libopenrazer::Capability();
    }

    // Remove "Custom Effect" entry when you switch away from it - only gets added by the Custom Editor button
    if (!isCustomEffect)
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
    if (capability.getIdentifier() != openrazer::Effect::Wave && capability.getIdentifier() != openrazer::Effect::Wheel) {
        findChild<QRadioButton *>("radiobutton1")->hide();
        findChild<QRadioButton *>("radiobutton2")->hide();
    } else {
        findChild<QRadioButton *>("radiobutton1")->show();
        findChild<QRadioButton *>("radiobutton2")->show();
    }

    /* Actually go apply the effect in all cases, except for Custom Effect
     * because there we handle this in the CustomEditor class */
    if (!isCustomEffect)
        applyEffectStandardLoc(capability.getIdentifier());
}

openrazer::RGB LedWidget::getColorForButton(int num)
{
    QPalette pal = findChild<QPushButton *>("colorbutton" + QString::number(num))->palette();
    QColor color = pal.color(QPalette::Button);
    return QCOLOR_TO_RGB(color);
}

openrazer::WaveDirection LedWidget::getWaveDirection()
{
    return findChild<QRadioButton *>("radiobutton1")->isChecked() ? openrazer::WaveDirection::RIGHT_TO_LEFT : openrazer::WaveDirection::LEFT_TO_RIGHT;
}

openrazer::WheelDirection LedWidget::getWheelDirection()
{
    return findChild<QRadioButton *>("radiobutton1")->isChecked()
            ? openrazer::WheelDirection::CLOCKWISE
            : openrazer::WheelDirection::COUNTER_CLOCKWISE;
}

void LedWidget::applyEffectStandardLoc(openrazer::Effect effect)
{
    try {
        switch (effect) {
        case openrazer::Effect::Off: {
            mLed->setOff();
            break;
        }
        case openrazer::Effect::On: {
            mLed->setOn();
            break;
        }
        case openrazer::Effect::Static: {
            openrazer::RGB c = getColorForButton(1);
            mLed->setStatic(c);
            break;
        }
        case openrazer::Effect::Breathing: {
            openrazer::RGB c = getColorForButton(1);
            mLed->setBreathing(c);
            break;
        }
        case openrazer::Effect::BreathingDual: {
            openrazer::RGB c1 = getColorForButton(1);
            openrazer::RGB c2 = getColorForButton(2);
            mLed->setBreathingDual(c1, c2);
            break;
        }
        case openrazer::Effect::BreathingRandom: {
            mLed->setBreathingRandom();
            break;
        }
        case openrazer::Effect::BreathingMono: {
            mLed->setBreathingMono();
            break;
        }
        case openrazer::Effect::Blinking: {
            openrazer::RGB c = getColorForButton(1);
            mLed->setBlinking(c);
            break;
        }
        case openrazer::Effect::Spectrum: {
            mLed->setSpectrum();
            break;
        }
        case openrazer::Effect::Wave: {
            mLed->setWave(getWaveDirection());
            break;
        }
        case openrazer::Effect::Wheel: {
            mLed->setWheel(getWheelDirection());
            break;
        }
        case openrazer::Effect::Reactive: {
            openrazer::RGB c = getColorForButton(1);
            mLed->setReactive(c, openrazer::ReactiveSpeed::_500MS); // TODO Configure speed?
            break;
        }
        case openrazer::Effect::Ripple: {
            openrazer::RGB c = getColorForButton(1);
            mLed->setRipple(c);
            break;
        }
        case openrazer::Effect::RippleRandom: {
            mLed->setRippleRandom();
            break;
        }
        default:
            throw new std::invalid_argument("Effect not handled: " + QVariant::fromValue(effect).toString().toStdString());
        }
    } catch (const libopenrazer::DBusException &e) {
        qWarning("Failed to change effect");
        util::showError(tr("Failed to change effect"));
    }
}

void LedWidget::applyEffect()
{
    auto *combobox = findChild<QComboBox *>("combobox");

    libopenrazer::Capability capability = combobox->itemData(combobox->currentIndex()).value<libopenrazer::Capability>();

    applyEffectStandardLoc(capability.getIdentifier());
}

libopenrazer::Led *LedWidget::led()
{
    return mLed;
}
