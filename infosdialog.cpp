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

#include "infosdialog.h"
#include "dbushandler.h"

InfosDialog::InfosDialog(int networkId, QWidget *parent)
    : KDialog(parent)
{
    setCaption(i18n("Network informations"));
    setModal(true);
    setButtons(KDialog::Close);

    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    QString signal;
    if (DBusHandler::instance()->callDaemon("GetSignalDisplayType").toInt())
        signal = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "strength").toString()+" dBm";
    else
        signal = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "quality").toString()+"%";
    ui.qualitytext->setText(signal);
    QString encryption;
    if (DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "encryption").toBool())
        encryption = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "encryption_method").toString();
    else
        encryption = i18n("Unsecured");
    ui.encryptiontext->setText(encryption);
    ui.accesspointtext->setText(DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "bssid").toString());
    ui.modetext->setText(DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "mode").toString());
    ui.channeltext->setText(DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "channel").toString());

    setMainWidget(widget);
    resize(0, 0);
}

#include "infosdialog.moc"
