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

#include <QFormLayout>
#include <QLabel>

#include <KLocalizedString>


InfosDialog::InfosDialog(int networkId, QWidget *parent)
    : KDialog(parent)
{
    init();

    QString signal;
    if (DBusHandler::instance()->callDaemon("GetSignalDisplayType").toInt())
        signal = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "strength").toString()+" dBm";
    else
        signal = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "quality").toString()+"%";
    m_layout->addRow(new QLabel(i18n("Signal strength :")), new QLabel(signal));

    QString encryption;
    if (DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "encryption").toBool())
        encryption = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "encryption_method").toString();
    else
        encryption = i18n("Unsecured");
    m_layout->addRow(new QLabel(i18n("Encryption type :")), new QLabel(encryption));

    QString accessPoint = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "bssid").toString();
    m_layout->addRow(new QLabel(i18n("Access point address :")), new QLabel(accessPoint));

    QString mode = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "mode").toString();
    m_layout->addRow(new QLabel(i18n("Mode :")), new QLabel(mode));

    QString channel = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "channel").toString();
    m_layout->addRow(new QLabel(i18n("Channel :")), new QLabel(channel));
}

InfosDialog::InfosDialog(QWidget *parent)
    : KDialog(parent)
{
    init();

    Status status = DBusHandler::instance()->status();

    if (status.State == WicdState::CONNECTING) {
        m_layout->addRow(new QLabel(i18n("State :")), new QLabel(i18n("Connecting")));
    }
    else if (status.State == WicdState::WIRED) {
        m_layout->addRow(new QLabel(i18n("Connection type :")), new QLabel(i18nc("Connection type :", "Wired")));
        m_layout->addRow(new QLabel(i18n("IP :")), new QLabel(status.Infos.at(0)));
    }
    else if (status.State == WicdState::WIRELESS) {
        m_layout->addRow(new QLabel(i18n("Connection type :")), new QLabel(i18nc("Connection type :", "Wireless")));
        m_layout->addRow(new QLabel(i18n("SSID :")), new QLabel(status.Infos.at(1)));
        m_layout->addRow(new QLabel(i18n("Speed :")), new QLabel(status.Infos.at(4)));
        m_layout->addRow(new QLabel(i18n("IP :")), new QLabel(status.Infos.at(0)));

        QString quality = status.Infos.at(2);
        QString unit = "%";
        if (quality.toInt() <= -10)
            unit = " dBm";
        m_layout->addRow(new QLabel(i18n("Signal strength :")), new QLabel(quality + unit));
    }
    else {
        m_layout->addRow(new QLabel(i18n("State :")), new QLabel(i18n("Disconnected")));
    }
}

void InfosDialog::init()
{
    setCaption(i18n("Network informations"));
    setModal(true);
    setButtons(KDialog::Close);

    QWidget *widget = new QWidget(this);
    m_layout = new QFormLayout(widget);
    m_layout->setLabelAlignment(Qt::AlignLeft);

    widget->setLayout(m_layout);
    setMainWidget(widget);
    resize(0, 0);
}

#include "infosdialog.moc"
