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

#ifndef WICDENGINE_H
#define WICDENGINE_H

#include "dbushandler.h"

#include <Plasma/DataEngine>

class WicdEngine : public Plasma::DataEngine
{
    Q_OBJECT

public:
    WicdEngine(QObject* parent, const QVariantList& args);

    Plasma::Service *serviceForSource(const QString &source);
    void init();
    QStringList sources() const;

protected:
    bool sourceRequestEvent(const QString &source);
    bool updateSourceEvent(const QString& source);

private slots:
    void updateStatus(Status status);
    void forceUpdateStatus();
    void profileNeeded();
    void profileNotNeeded();
    void scanStarted();
    void scanEnded();
    void resultReceived(const QString& result);
    void daemonStarted();
    void daemonClosed();

private:
    Status m_status;
    QString m_message;
    QString m_interface;
    bool m_profileNeeded;
    bool m_scanning;
    bool m_daemonRunning;
    QString m_error;
    //for translations
    QHash<QString, QString> m_messageTable;
};

K_EXPORT_PLASMA_DATAENGINE(wicd, WicdEngine)

#endif // WICDENGINE_H
