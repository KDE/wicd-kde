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

#include "trayicon.h"
#include "infosdialog.h"

#include <KMenu>
#include <KIcon>
#include <KAboutApplicationDialog>
#include <KCmdLineArgs>
#include <KLocalizedString>


TrayIcon::TrayIcon( QObject *parent )
    : KStatusNotifierItem( parent )
{
    setObjectName("wicdclientkde.statusnotifieritem");
    setStatus(KStatusNotifierItem::Active);
    setCategory(KStatusNotifierItem::Communications);
    setIcon("network-wired");
    setToolTipTitle("Wicd");

    addActions();
}

void TrayIcon::setIcon(const QString &name)
{
    if (name != iconName()) {
        setIconByName(name);
        setToolTipIconByName(name);
    }
}

void TrayIcon::showAbout() const
{
     KAboutApplicationDialog aboutDialog(KCmdLineArgs::aboutData());
     aboutDialog.exec();
}

void TrayIcon::connectionInfoRequested() const
{
    InfosDialog dialog;
    dialog.exec();
}

void TrayIcon::addActions()
{
    contextMenu()->addAction(KIcon("help-about"), i18n("Connection infos"), this, SLOT(connectionInfoRequested()));
    contextMenu()->addAction(KIcon("network-wireless"), i18n("About"), this, SLOT(showAbout()));
}
