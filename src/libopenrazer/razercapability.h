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

#ifndef RAZERCAPABILITY_H
#define RAZERCAPABILITY_H

#include <QString>
#include <QMetaType>

#include <razer_test.h>

namespace libopenrazer
{
class RazerCapability
{
public:
    RazerCapability();
    RazerCapability(razer_test::RazerEffect identifier, QString displayString, int numColors);
    RazerCapability(razer_test::RazerEffect, QString displayString, bool wave);
    RazerCapability(const RazerCapability &other);
    ~RazerCapability();
    int getNumColors() const;
    razer_test::RazerEffect getIdentifier() const;
    QString getDisplayString() const;
    bool isWave() const;
private:
    int numColors;
    razer_test::RazerEffect identifier;
    QString displayString;
    bool wave;
};
}

Q_DECLARE_METATYPE(libopenrazer::RazerCapability)

#endif // RAZERCAPABILITY_H
