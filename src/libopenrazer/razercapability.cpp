/*
 * Copyright (C) 2017-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
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

#include "razercapability.h"

namespace libopenrazer
{
RazerCapability::RazerCapability()
{
    this->identifier = "";
    this->displayString = "";
    this->numColors = 0;
    this->wave = false;
}
RazerCapability::RazerCapability(QString identifier, QString displayString, int numColors)
{
    this->identifier = identifier;
    this->displayString = displayString;
    this->numColors = numColors;
    this->wave = false;
}

RazerCapability::RazerCapability(QString identifier, QString displayString, bool isWave) : RazerCapability(identifier, displayString, 0)
{
    this->wave = isWave;
}

RazerCapability::RazerCapability(const RazerCapability &other)
{
    this->identifier = other.identifier;
    this->displayString = other.displayString;
    this->numColors = other.numColors;
    this->wave = other.wave;
}

RazerCapability::~RazerCapability()
{
}

int RazerCapability::getNumColors() const
{
    return numColors;
}

QString RazerCapability::getIdentifier() const
{
    return identifier;
}
QString RazerCapability::getDisplayString() const
{
    return displayString;
}
bool RazerCapability::isWave() const
{
    return wave;
}

}
