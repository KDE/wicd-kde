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
#include "global.h"

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>

#include <KLocalizedString>


InfosDialog::InfosDialog(int networkId, QWidget *parent)
    : KDialog(parent)
{
    setCaption(i18n("Network informations"));
    setModal(true);
    setButtons(KDialog::Close);

    QWidget *widget = new QWidget(this);
    QFormLayout *formLayout = new QFormLayout(widget);
    formLayout->setLabelAlignment(Qt::AlignLeft);

    if (networkId == -1)
    {
        Status status = DBusHandler::instance()->status();

        if (status.State == WicdState::NOT_CONNECTED)
        {
            formLayout->addRow(new QLabel(i18n("Disconnected")));
        }
        else if (status.State == WicdState::CONNECTING)
        {
            formLayout->addRow(new QLabel(i18n("Connecting")));
        }
        else if (status.State == WicdState::WIRED)
        {
            formLayout->addRow(new QLabel(i18n("Wired")));
            formLayout->addRow(new QLabel(i18n("IP:")), new QLabel(status.Infos.at(0)));
        }
        else if (status.State == WicdState::WIRELESS)
        {
            formLayout->addRow(new QLabel(i18n("Wireless")));
            formLayout->addRow(new QLabel(i18n("SSID:")), new QLabel(status.Infos.at(1)));
            formLayout->addRow(new QLabel(i18n("Speed:")), new QLabel(status.Infos.at(4)));
            formLayout->addRow(new QLabel(i18n("IP:")), new QLabel(status.Infos.at(0)));

            QString quality = status.Infos.at(2);
            QString unit = "%";
            if (quality.toInt() <= -10)
                unit = " dBm";
            formLayout->addRow(new QLabel(i18n("Signal strength:")), new QLabel(quality + unit));
        }
    }
    else
    {
        QString signal;
        if (DBusHandler::instance()->callDaemon("GetSignalDisplayType").toInt())
            signal = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "strength").toString()+" dBm";
        else
            signal = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "quality").toString()+"%";
        formLayout->addRow(new QLabel(i18n("Signal strength:")), new QLabel(signal));

        QString encryption;
        if (DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "encryption").toBool())
            encryption = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "encryption_method").toString();
        else
            encryption = i18n("Unsecured");
        formLayout->addRow(new QLabel(i18n("Encryption:")), new QLabel(encryption));

        QString accessPoint = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "bssid").toString();
        formLayout->addRow(new QLabel(i18n("Access point address:")), new QLabel(accessPoint));

        QString mode = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "mode").toString();
        formLayout->addRow(new QLabel(i18n("Mode:")), new QLabel(mode));

        QString channel = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "channel").toString();
        formLayout->addRow(new QLabel(i18n("Channel:")), new QLabel(channel));
    }

    widget->setLayout(formLayout);
    setMainWidget(widget);
    resize(0, 0);
}

#include "infosdialog.moc"
