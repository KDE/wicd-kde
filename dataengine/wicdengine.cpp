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
#include "wicdservice.h"

WicdEngine::WicdEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent)
{
    Q_UNUSED(args)

    setMinimumPollingInterval(1000);

    connect(DBusHandler::instance(), SIGNAL(statusChange(Status)), this, SLOT(updateStatus(Status)));
    connect(DBusHandler::instance(), SIGNAL(scanStarted()), this, SLOT(scanStarted()));
    connect(DBusHandler::instance(), SIGNAL(scanEnded()), this, SLOT(scanEnded()));
    connect(DBusHandler::instance(), SIGNAL(launchChooser()), this, SLOT(profileNeeded()));
    connect(DBusHandler::instance(), SIGNAL(chooserLaunched()), this, SLOT(profileNotNeeded()));
    connect(DBusHandler::instance(), SIGNAL(connectionResultSend(QString)), this, SLOT(resultReceived(QString)));
}

void WicdEngine::init()
{
    m_scanning = false;
    m_profileNeeded = false;

    //we need a current profile
    QString profile = DBusHandler::instance()->callWired("GetDefaultWiredNetwork").toString();
    if (profile.isEmpty())
        profile = DBusHandler::instance()->callWired("GetWiredProfileList").toStringList().at(0);
    DBusHandler::instance()->setCurrentProfile(profile);

    //check if the profile manager is needed
    if (DBusHandler::instance()->callDaemon("GetNeedWiredProfileChooser").toBool()) {
        profileNeeded();
    }

    //force first status update
    forceUpdateStatus();
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
    sources<<"status";
    sources<<"daemon";
    return sources;
}

bool WicdEngine::sourceRequestEvent(const QString &source)
{
    if (source == "networks") {
        updateSourceEvent(source);
        return true;
    }
    if (source == "status") {
        updateSourceEvent(source);
        return true;
    }
    if (source == "daemon") {
        updateSourceEvent(source);
        return true;
    }
    return false;
}

bool WicdEngine::updateSourceEvent(const QString &source)
{
    if (source == "networks") {
        //reset
        removeAllData(source);
        //populate new data
        QMap<int, NetworkInfo> list = DBusHandler::instance()->networksList();
        QMap<int, NetworkInfo>::const_iterator it = list.constBegin();
        while (it != list.constEnd()) {
            setData(source, QString::number(it.key()), it.value());
            ++it;
        }
        return true;
    }
    if (source == "status") {
        setData(source, "state", m_status.State);
        setData(source, "info", m_status.Info);
        setData(source, "message", m_message);
        setData(source, "interface", m_interface);
        return true;
    }
    if (source == "daemon") {
        setData(source, "profileNeeded", m_profileNeeded);
        setData(source, "scanning", m_scanning);
        setData(source, "connectionResult", m_connectionResult);
        //to simulate a "signal-like" behaviour
        m_connectionResult = "";
        return true;
    }
    return false;
}

void WicdEngine::updateStatus(Status status)
{
    m_interface = DBusHandler::instance()->callDaemon("GetCurrentInterface").toString();
    if (status.State == WicdState::CONNECTING) {
        bool wired = (status.Info.at(0)=="wired");
        if (wired) {
            m_message = DBusHandler::instance()->callWired("CheckWiredConnectingMessage").toString();
        } else {
            m_message = DBusHandler::instance()->callWireless("CheckWirelessConnectingMessage").toString();
        }
        QTimer::singleShot(500, this, SLOT(forceUpdateStatus()));
    }
    m_status = status;
    updateSourceEvent("status");
}

void WicdEngine::forceUpdateStatus()
{
    updateStatus(DBusHandler::instance()->status());
}

void WicdEngine::profileNeeded()
{
    m_profileNeeded = true;
    updateSourceEvent("daemon");
}

void WicdEngine::profileNotNeeded()
{
    m_profileNeeded = false;
    updateSourceEvent("daemon");
}

void WicdEngine::scanStarted()
{
    m_scanning = true;
    updateSourceEvent("daemon");
}

void WicdEngine::scanEnded()
{
    m_scanning = false;
    updateSourceEvent("daemon");
}

void WicdEngine::resultReceived(const QString& result)
{
    m_connectionResult = result;
    updateSourceEvent("daemon");
}

#include "wicdengine.moc"
