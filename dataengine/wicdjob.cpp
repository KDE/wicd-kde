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

#include "wicdjob.h"

WicdJob::WicdJob(DBusHandler* dbus,
                 const QString& operation,
                 QMap<QString,QVariant>& parameters,
                 QObject* parent)
    : Plasma::ServiceJob(dbus->objectName(), operation, parameters, parent),
      m_dbus(dbus)
{
}

void WicdJob::start()
{
    const QString operation(operationName());
    if (operation == "toggleConnection") {
        int networkId = parameters()["networkId"].toInt();
        bool networkConnected = false;
        Status status = m_dbus->status();
        if ( (status.State == WicdState::WIRED) && (networkId ==-1) ) {
            networkConnected = true;
        } else if ( status.State == WicdState::WIRELESS ) {
            int wirelessId = m_dbus->callWireless("GetCurrentNetworkID").toInt();
            if ( wirelessId ==  networkId) {
                networkConnected = true;
            }
        }

        if (networkConnected) {
            if (networkId == -1) {
                m_dbus->callWired("DisconnectWired");
            } else {
                m_dbus->callWireless("DisconnectWireless");
            }
            m_dbus->callDaemon("SetForcedDisconnect", true);
        } else {
            if (networkId == -1) {
                m_dbus->callWired("ConnectWired");
            } else {
                m_dbus->callWireless("ConnectWireless", networkId);
            }
        }
    }
}
