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
    } else if (operation == "cancelConnect") {
        m_dbus->callDaemon("CancelConnect");
        m_dbus->callDaemon("SetForcedDisconnect", true);
    } else if (operation == "createAdHocNetwork") {
        m_dbus->callWireless("CreateAdHocNetwork",
                                              parameters()["essid"].toString(),
                                              parameters()["channel"].toString(),
                                              parameters()["ip"].toString(),
                                              "WEP",
                                              parameters()["key"].toString(),
                                              parameters()["wep"].toBool(),
                                              false /*parameters()["ics"].toBool()*/);
    } else if (operation == "findHiddenNetwork") {
        m_dbus->callWireless("SetHiddenNetworkESSID", parameters()["essid"].toString());
        m_dbus->scan();
    } else if (operation == "scan") {
        m_dbus->scan();
    } else if (operation == "rfkill") {
           m_dbus->rfkill();
    } else if (operation == "getWiredProfileList") {
        setResult(m_dbus->callWired("GetWiredProfileList"));
    } else if (operation == "setProfileDefaultProperty") {
        if (parameters()["default"].toBool()) {
            //We are about to set a new default profile
            //so we unset the default option in the current default wired profile.
            m_dbus->callWired("UnsetWiredDefault");
        }
        m_dbus->callWired("SetWiredProperty", "default", parameters()["default"].toBool());
        m_dbus->callWired("SaveWiredNetworkProfile", parameters()["profile"].toString());
    } else if (operation == "setCurrentProfile") {
        m_dbus->setCurrentProfile(parameters()["profile"].toString());
        //returns true if the profile is the default profile
        setResult(m_dbus->callWired("GetWiredProperty", "default").toBool());
    } else if (operation == "createWiredNetworkProfile") {
        m_dbus->callWired("CreateWiredNetworkProfile", parameters()["profile"].toString(), false);
    } else if (operation == "deleteWiredNetworkProfile") {
        m_dbus->callWired("DeleteWiredNetworkProfile", parameters()["profile"].toString());
    } else if (operation == "connectWired") {
        m_dbus->callWired("ConnectWired");
    } else if (operation == "setProfileNotNeeded") {
        m_dbus->callDaemon("SetNeedWiredProfileChooser", false);
        //update the dataengine "manually"
        m_dbus->emitChooserLaunched();
    }
}
