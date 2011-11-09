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

#ifndef DBUSHANDLER_H
#define DBUSHANDLER_H

#include <QtDBus>
#include "types.h"

class DBusHandler : public QObject
{
    Q_OBJECT

public:
    static DBusHandler* instance();
    static void destroy();

    QMap<int, NetworkInfo> networksList() const;
    Status status() const;
    void emitChooserLaunched();

    QVariant callDaemon(const QString &query,
                        const QVariant &arg1 = QVariant(),
                        const QVariant &arg2 = QVariant(),
                        const QVariant &arg3 = QVariant(),
                        const QVariant &arg4 = QVariant(),
                        const QVariant &arg5 = QVariant(),
                        const QVariant &arg6 = QVariant(),
                        const QVariant &arg7 = QVariant(),
                        const QVariant &arg8 = QVariant()) const;

    QVariant callWired(const QString &query,
                       const QVariant &arg1 = QVariant(),
                       const QVariant &arg2 = QVariant(),
                       const QVariant &arg3 = QVariant(),
                       const QVariant &arg4 = QVariant(),
                       const QVariant &arg5 = QVariant(),
                       const QVariant &arg6 = QVariant(),
                       const QVariant &arg7 = QVariant(),
                       const QVariant &arg8 = QVariant()) const;

    QVariant callWireless(const QString &query,
                          const QVariant &arg1 = QVariant(),
                          const QVariant &arg2 = QVariant(),
                          const QVariant &arg3 = QVariant(),
                          const QVariant &arg4 = QVariant(),
                          const QVariant &arg5 = QVariant(),
                          const QVariant &arg6 = QVariant(),
                          const QVariant &arg7 = QVariant(),
                          const QVariant &arg8 = QVariant()) const;

public slots:
    void scan() const;
    void disconnect() const;

private slots:
    void statusChanged(uint state, QVariantList info);

signals:
    void statusChange(Status status);
    void connectionResultSend(const QString& result);
    void launchChooser();
    void chooserLaunched();
    void scanStarted();
    void scanEnded();

private:
    DBusHandler();
    ~DBusHandler();

    NetworkInfo wiredProperties() const;
    NetworkInfo wirelessProperties(const int &networkId) const;
    QVariant call(QDBusInterface *interface,
                  const QString &query,
                  const QVariant &arg1 = QVariant(),
                  const QVariant &arg2 = QVariant(),
                  const QVariant &arg3 = QVariant(),
                  const QVariant &arg4 = QVariant(),
                  const QVariant &arg5 = QVariant(),
                  const QVariant &arg6 = QVariant(),
                  const QVariant &arg7 = QVariant(),
                  const QVariant &arg8 = QVariant()) const;

    QDBusInterface *m_daemon;
    QDBusInterface *m_wired;
    QDBusInterface *m_wireless;

    static DBusHandler* s_instance;

};

#endif // DBUSHANDLER_H
