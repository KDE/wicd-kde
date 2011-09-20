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

#include "wicdengine.h"
#include "dbushandler.h"
#include "wicdservice.h"

WicdEngine::WicdEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent)
{
    Q_UNUSED(args)

    setMinimumPollingInterval(1000);
}

Plasma::Service *WicdEngine::serviceForSource(const QString &source)
{
    Q_UNUSED(source)
    return new WicdService(this, DBusHandler::instance());
}

QStringList WicdEngine::sources() const
{
    QStringList sources;
    sources<<"networks";
    return sources;
}

bool WicdEngine::sourceRequestEvent(const QString &source)
{
    if (source == "networks") {
        updateSourceEvent(source);
        return true;
    }
    return false;
}

bool WicdEngine::updateSourceEvent(const QString &source)
{
    //reset
    setData(source, DataEngine::Data());
    //populate new data
    QMap<int, NetworkInfos> list = DBusHandler::instance()->networksList();
    QMap<int, NetworkInfos>::const_iterator it = list.constBegin();
    while (it != list.constEnd()) {
        setData(source, QString::number(it.key()), it.value());
        ++it;
    }
    return true;
}

#include "wicdengine.moc"
