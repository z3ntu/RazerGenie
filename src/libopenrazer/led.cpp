/*
 * Copyright (C) 2016-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
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

#include <QColor>

#include "libopenrazer.h"

#define QCOLOR_TO_UCHAR_RGB(c) QVariant::fromValue(static_cast<uchar>(c.red())), QVariant::fromValue(static_cast<uchar>(c.green())), QVariant::fromValue(static_cast<uchar>(c.blue()))

namespace libopenrazer
{

Led::Led(QDBusObjectPath objectPath)
{
    mObjectPath = objectPath;
}

/*
 * Destructor
 */
Led::~Led()
    = default;

QDBusObjectPath Led::getObjectPath()
{
    return mObjectPath;
}

/*!
 * \fn bool libopenrazer::Led::setNone()
 *
 * Sets the LED to none / off.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setNone()
{
    QDBusReply<bool> reply = ledIface()->call("setNone");
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn bool libopenrazer::Led::setStatic(QColor color)
 *
 * Sets the lighting to static lighting in the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setStatic(QColor color)
{
    QDBusReply<bool> reply = ledIface()->call("setStatic", QCOLOR_TO_UCHAR_RGB(color));
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn bool libopenrazer::Led::setBreathSingle(QColor color)
 *
 * Sets the lighting to the single breath effect with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setBreathing(QColor color)
{
    QDBusReply<bool> reply = ledIface()->call("setBreathing", QCOLOR_TO_UCHAR_RGB(color));
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn bool libopenrazer::Led::setBreathDual(QColor color, QColor color2)
 *
 * Sets the lighting to the dual breath effect with the specified \a color and \a color2.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setBreathingDual(QColor color, QColor color2)
{
    QDBusReply<bool> reply = ledIface()->call("setBreathingDual", QCOLOR_TO_UCHAR_RGB(color), QCOLOR_TO_UCHAR_RGB(color2));
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn bool libopenrazer::Led::setBreathRandom()
 *
 * Sets the lighting wheel to the random breath effect.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setBreathingRandom()
{
    QDBusReply<bool> reply = ledIface()->call("setBreathingRandom");
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn bool libopenrazer::Led::setBreathRandom()
 *
 * Sets the lighting wheel to the random breath effect.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setBlinking(QColor color)
{
    QDBusReply<bool> reply = ledIface()->call("setBlinking", QCOLOR_TO_UCHAR_RGB(color));
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn bool libopenrazer::Led::setSpectrum()
 *
 * Sets the lighting to spectrum mode.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setSpectrum()
{
    QDBusReply<bool> reply = ledIface()->call("setSpectrum");
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn bool libopenrazer::Led::setWave(WaveDirection direction)
 *
 * Sets the lighting effect to wave, in the direction \a direction.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setWave(WaveDirection direction) // FIXME
{
    QDBusReply<bool> reply = ledIface()->call("setWave", direction);
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn bool libopenrazer::Led::setReactive(QColor color, ReactiveSpeed speed)
 *
 * Sets the lighting to reactive mode with the specified \a color and \a speed.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setReactive(QColor color, ReactiveSpeed speed) // FIXME
{
    QDBusReply<bool> reply = ledIface()->call("setReactive", speed, QCOLOR_TO_UCHAR_RGB(color));
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn bool libopenrazer::Led::setBrightness(double brightness)
 *
 * Sets the \a brightness (0-100).
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setBrightness(uchar brightness)
{
    QDBusReply<bool> reply = ledIface()->call("setBrightness", QVariant::fromValue(brightness));
    return handleBoolReply (reply, Q_FUNC_INFO);
}

/*!
 * \fn double libopenrazer::Led::getBrightness()
 *
 * Returns the current brightness (0-100).
 */
uchar Led::getBrightness()
{
    QDBusReply<uchar> reply = ledIface()->call("getBrightness");
    if (reply.isValid()) {
        return reply.value();
    } else {
        printDBusError(reply.error(), Q_FUNC_INFO);
        return 0;
    }
}

}
