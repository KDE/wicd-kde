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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "trayicon.h"
#include "networkpanel.h"
#include "dbushandler.h"

#include <KMainWindow>
#include <KPushButton>

class MainWindow : public KMainWindow
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.wicd-client-kde")

public:
    MainWindow();
    ~MainWindow();

public slots:
    Q_SCRIPTABLE void reloadConfig();

private slots:
    void activated();
    void updateStatus(Status status);
    void handleConnectionResult(const QString& result);
    void notify(const QString& event, const QString& message);
    void forceUpdateStatus();
    void freeze();
    void unfreeze();
    void cancelConnect() const;
    void showPreferences();
    void createAdhocDialog();
    void findHiddenDialog();

private:
    void setWirelessIcon(int quality);

    TrayIcon *m_trayicon;
    NetworkPanel *m_networkPanel;
    KPushButton *m_abortButton;
    Status m_status;
    bool m_autoscan;
    QHash<QString, QString> m_messageTable;

};

#endif
