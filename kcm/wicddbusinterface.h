/****************************************************************************
 *  Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>                    *
 *                                                                          *
 *  This file is part of the KDE project.                                   *
 *                                                                          *
 *  Wicd Client KDE is free software: you can redistribute it and/or modify *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation, either version 3 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  Wicd Client KDE is distributed in the hope that it will be useful,      *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.    *
 ****************************************************************************/

#ifndef WICDDBUSINTERFACE_H
#define WICDDBUSINTERFACE_H

#include <QtDBus/QDBusInterface>

class WicdDbusInterface
{
public:

    static WicdDbusInterface * instance();

    WicdDbusInterface();
    virtual ~WicdDbusInterface();

    QDBusInterface &daemon() const;
    QDBusInterface &wireless() const;
    QDBusInterface &wired() const;

private:
    class Private;
    Private *d;
};

#endif /* WICDDBUSINTERFACE_H */
