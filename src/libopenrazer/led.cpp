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

#include <QDBusReply>
#include <QColor>

#include "libopenrazer.h"

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
{
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
    QDBusReply<QDBusVariant> reply = ledIface()->call("setNone");
    return reply.isValid();
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
    QDBusReply<QDBusVariant> reply = ledIface()->call("setStatic", color.red(), color.green(), color.blue());
    return reply.isValid();
}

/*!
 * \fn bool libopenrazer::Led::setBreathSingle(QColor color)
 *
 * Sets the lighting to the single breath effect with the specified \a color.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setBreathingSingle(QColor color)
{
    QDBusReply<QDBusVariant> reply = ledIface()->call("setBreathingSingle", color.red(), color.green(), color.blue());
    return reply.isValid();
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
    QDBusReply<QDBusVariant> reply = ledIface()->call("setBreathingDual", color.red(), color.green(), color.blue(), color2.red(), color2.green(), color2.blue());
    return reply.isValid();
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
    QDBusReply<QDBusVariant> reply = ledIface()->call("setBreathingRandom");
    return reply.isValid();
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
    QDBusReply<QDBusVariant> reply = ledIface()->call("setBlinking", color.red(), color.green(), color.blue());
    return reply.isValid();
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
    QDBusReply<QDBusVariant> reply = ledIface()->call("setSpectrum");
    return reply.isValid();
}

/*!
 * \fn bool libopenrazer::Led::setWave(WaveDirection direction)
 *
 * Sets the lighting effect to wave, in the direction \a direction.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setWave(WaveDirection direction)
{
    QDBusReply<QDBusVariant> reply = ledIface()->call("setWave", direction);
    return reply.isValid();
}

/*!
 * \fn bool libopenrazer::Led::setReactive(QColor color, ReactiveSpeed speed)
 *
 * Sets the lighting to reactive mode with the specified \a color and \a speed.
 *
 * Returns if the D-Bus call was successful.
 */
bool Led::setReactive(QColor color, ReactiveSpeed speed)
{
    QDBusReply<QDBusVariant> reply = ledIface()->call("setReactive", speed, color.red(), color.green(), color.blue());
    return reply.isValid();
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
    QDBusReply<QDBusVariant> reply = ledIface()->call("setBrightness", brightness);
    return reply.isValid();
}

/*!
 * \fn double libopenrazer::Led::getBrightness()
 *
 * Returns the current brightness (0-100).
 */
uchar Led::getBrightness()
{
    QDBusReply<QDBusVariant> reply = ledIface()->call("getBrightness");
    if (reply.isValid())
        return reply.value().variant().value<uchar>();
    else
        return 0;
}

}
