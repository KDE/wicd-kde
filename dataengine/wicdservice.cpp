/****************************************************************************
 *  Copyright (c) 2011 Anthony Vital <anthony.vital@gmail.com>              *
 *                                                                          *
 *  This file is part of Wicd Client KDE.                                   *
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
 *  along with Wicd Client KDE.  If not, see <http://www.gnu.org/licenses/>.*
 ****************************************************************************/

#include "wicdservice.h"
#include "wicdjob.h"

#include <KDebug>

WicdService::WicdService(QObject* parent, DBusHandler* dbus)
    : Plasma::Service(parent),
      m_dbus(dbus)
{
    setName("wicd");
}

Plasma::ServiceJob* WicdService::createJob(const QString& operation,
                                             QMap<QString,QVariant>& parameters)
{
    kDebug() << "Job" << operation << "with arguments" << parameters << "requested";
    return new WicdJob(m_dbus, operation, parameters, this);
}
