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

#ifndef RAZER_TEST_H
#define RAZER_TEST_H

#include <QDBusArgument>

// TODO PUT INTO RAZER_TEST PUBLIC HEADER FILE!!
namespace razer_test
{

struct RazerDPI {
    ushort dpi_x;
    ushort dpi_y;
};

// Marshall the RazerDPI data into a D-Bus argument
inline QDBusArgument &operator<<(QDBusArgument &argument, const RazerDPI &razerDPI)
{
    argument.beginStructure();
    argument << razerDPI.dpi_x << razerDPI.dpi_y;
    argument.endStructure();
    return argument;
}

// Retrieve the RazerDPI data from the D-Bus argument
inline const QDBusArgument &operator>>(const QDBusArgument &argument, RazerDPI &razerDPI)
{
    argument.beginStructure();
    argument >> razerDPI.dpi_x >> razerDPI.dpi_y;
    argument.endStructure();
    return argument;
}

}
Q_DECLARE_METATYPE(razer_test::RazerDPI)

#endif // RAZER_TEST_H
