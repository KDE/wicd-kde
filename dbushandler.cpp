/****************************************************************************
 *  Copyright (c) 2010 Anthony Vital <anthony.vital@gmail.com>              *
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

#include "dbushandler.h"
#include "global.h"
#include <KLocalizedString>

DBusHandler* DBusHandler::s_instance = 0;

DBusHandler* DBusHandler::instance()
{
    return s_instance ? s_instance : new DBusHandler();
}

void DBusHandler::destroy()
{
    if (s_instance)
    {
        s_instance->deleteLater();
        s_instance = 0;
    }
}

DBusHandler::DBusHandler()
{
    qDBusRegisterMetaType<Status>();

    m_daemon = new QDBusInterface("org.wicd.daemon", "/org/wicd/daemon", "org.wicd.daemon", QDBusConnection::systemBus());
    m_wired = new QDBusInterface("org.wicd.daemon", "/org/wicd/daemon/wired", "org.wicd.daemon.wired", QDBusConnection::systemBus());
    m_wireless = new QDBusInterface("org.wicd.daemon", "/org/wicd/daemon/wireless", "org.wicd.daemon.wireless", QDBusConnection::systemBus());

    if (callDaemon("GetNeedWiredProfileChooser").toBool()) {
        callDaemon("SetNeedWiredProfileChooser", false);
        emit launchChooser();
    }

    QDBusConnection::systemBus().connect("org.wicd.daemon",
                                         "/org/wicd/daemon",
                                         "org.wicd.daemon",
                                         "StatusChanged",
                                         this,
                                         SLOT(statusChanged(uint, QVariantList))
                                         );
    QDBusConnection::systemBus().connect("org.wicd.daemon",
                                         "/org/wicd/daemon",
                                         "org.wicd.daemon",
                                         "ConnectResultsSent",
                                         this,
                                         SIGNAL(connectionResultSend(QString))
                                         );
    QDBusConnection::systemBus().connect("org.wicd.daemon",
                                         "/org/wicd/daemon",
                                         "org.wicd.daemon",
                                         "LaunchChooser",
                                         this,
                                         SIGNAL(launchChooser())
                                         );
    QDBusConnection::systemBus().connect("org.wicd.daemon",
                                         "/org/wicd/daemon/wireless",
                                         "org.wicd.daemon.wireless",
                                         "SendStartScanSignal",
                                         this,
                                         SIGNAL(scanStarted())
                                         );
    QDBusConnection::systemBus().connect("org.wicd.daemon",
                                         "/org/wicd/daemon/wireless",
                                         "org.wicd.daemon.wireless",
                                         "SendEndScanSignal",
                                         this,
                                         SIGNAL(scanEnded())
                                         );

    s_instance = this;
}

DBusHandler::~DBusHandler()
{
    delete m_daemon;
    delete m_wired;
    delete m_wireless;
}

Status DBusHandler::status() const
{
    QDBusReply<Status> reply = m_daemon->call("GetConnectionStatus");
    return reply;
}

void DBusHandler::scan() const
{
    if (status().State!=WicdState::CONNECTING)
        m_wireless->call("Scan");
}

void DBusHandler::disconnect() const
{
    m_daemon->call("Disconnect");
    m_daemon->call("SetForcedDisconnect", true);
}

QMap<int, NetworkInfos> DBusHandler::networksList() const
{
    QMap<int, NetworkInfos> list;
    Status p_status = status();
    //look for wired connection
    if (call(m_daemon, "GetAlwaysShowWiredInterface").toBool() || call(m_wired, "CheckPluggedIn").toBool()) {
        //a cable is plugged in
        NetworkInfos wirednetwork = wiredProperties();
        if (p_status.State == WicdState::WIRED) {
            wirednetwork.insert("connected", true);
        } else {
            wirednetwork.insert("connected", false);
        }
        list.insert(-1, wirednetwork);
        //we need a current profile
        //TODO: Move to init and prevent empty profile name in profile manager?
        if (Wicd::currentprofile.isEmpty()) {
            QString profile = call(m_wired, "GetDefaultWiredNetwork").toString();
            if (profile.isEmpty())
                profile = call(m_wired, "GetWiredProfileList").toStringList().at(0);
            Wicd::currentprofile = profile;
            call(m_wired, "ReadWiredNetworkProfile", profile);
        }
    }
    //wireless
    int nbNetworks = call(m_wireless, "GetNumberOfNetworks").toInt();
    for (int id = 0; id < nbNetworks; ++id) {
        list.insert(id, wirelessProperties(id));
    }
    if (p_status.State == WicdState::WIRELESS) {
        list[call(m_wireless, "GetCurrentNetworkID").toInt()].insert("connected", true);
    }
    return list;
}

NetworkInfos DBusHandler::wiredProperties() const
{
    NetworkInfos properties;
    properties.insert("networkId", -1);
    properties.insert("essid", i18n("Wired network"));
    return properties;
}

NetworkInfos DBusHandler::wirelessProperties(const int &networkId) const
{
    NetworkInfos properties;
    properties.insert("networkId", networkId);
    properties.insert("essid", call(m_wireless, "GetWirelessProperty", networkId, "essid").toString());
    properties.insert("usedbm", call(m_daemon, "GetSignalDisplayType").toBool());
    properties.insert("strength", call(m_wireless, "GetWirelessProperty", networkId, "strength").toInt());
    properties.insert("quality", call(m_wireless, "GetWirelessProperty", networkId, "quality").toInt());
    properties.insert("encryption", call(m_wireless, "GetWirelessProperty", networkId, "encryption").toBool());
    properties.insert("connected", false);
    return properties;
}

void DBusHandler::statusChanged(uint state, QVariantList infos)
{
    Status status;
    status.State = state;
    switch (state) {
    case WicdState::NOT_CONNECTED:
        status.Infos.append("");
        break;
    case WicdState::CONNECTING:
        status.Infos.append(infos.at(0).toString());//"wired" or "wireless"
        if (infos.at(0).toString() == "wireless")
            status.Infos.append(infos.at(1).toString());//None if wired, essid if wireless
        break;
    case WicdState::WIRED:
        status.Infos.append(infos.at(0).toString());//IP Adresss
        break;
    case WicdState::WIRELESS:
        status.Infos.append(infos.at(0).toString());//IP Adresss
        status.Infos.append(infos.at(1).toString());//essid
        status.Infos.append(infos.at(2).toString());//signal strength
        status.Infos.append(infos.at(3).toString());//internal networkid
        status.Infos.append(infos.at(4).toString());//bitrate
        break;
    case WicdState::SUSPENDED:
        status.Infos.append("");
        break;
    default:
        break;
    }
    emit statusChange(status);
}

QVariant DBusHandler::callDaemon(const QString &query,
                                 const QVariant &arg1,
                                 const QVariant &arg2,
                                 const QVariant &arg3,
                                 const QVariant &arg4,
                                 const QVariant &arg5,
                                 const QVariant &arg6,
                                 const QVariant &arg7,
                                 const QVariant &arg8) const
{
    return call(m_daemon, query, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

QVariant DBusHandler::callWired(const QString &query,
                                const QVariant &arg1,
                                const QVariant &arg2,
                                const QVariant &arg3,
                                const QVariant &arg4,
                                const QVariant &arg5,
                                const QVariant &arg6,
                                const QVariant &arg7,
                                const QVariant &arg8) const
{
    return call(m_wired, query, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

QVariant DBusHandler::callWireless(const QString &query,
                                   const QVariant &arg1,
                                   const QVariant &arg2,
                                   const QVariant &arg3,
                                   const QVariant &arg4,
                                   const QVariant &arg5,
                                   const QVariant &arg6,
                                   const QVariant &arg7,
                                   const QVariant &arg8) const
{
    return call(m_wireless, query, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

QVariant DBusHandler::call(QDBusInterface *interface,
                           const QString &query,
                           const QVariant &arg1,
                           const QVariant &arg2,
                           const QVariant &arg3,
                           const QVariant &arg4,
                           const QVariant &arg5,
                           const QVariant &arg6,
                           const QVariant &arg7,
                           const QVariant &arg8) const
{
    QDBusMessage reply = interface->call(query, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    if (reply.arguments().length() > 1) {
        return QVariant(reply.arguments());
    } else if (reply.arguments().length() > 0) {
        return reply.arguments().at(0);
    } else {
        return QVariant();
    }
}

const QDBusArgument & operator<<(QDBusArgument &arg, const Status &status) {
    arg.beginStructure();
    arg << status.State << status.Infos;
    arg.endStructure();
    return arg;
}

const QDBusArgument & operator>>(const QDBusArgument &arg, Status &status) {
    arg.beginStructure();
    arg >> status.State >> status.Infos;
    arg.endStructure();
    return arg;
}
