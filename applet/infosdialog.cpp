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

#include "infosdialog.h"

#include <QLabel>

#include <KLocalizedString>

#include <Plasma/Theme>

InfosDialog::InfosDialog(Status status, QWidget *parent)
    : Plasma::Dialog(parent, Qt::Popup)
{
    init();

    if (status.State == WicdState::CONNECTING) {
        m_layout->addRow(i18n("State:"), new QLabel(i18n("Connecting")));
    }
    else if (status.State == WicdState::WIRED) {
        m_layout->addRow(i18n("Connection type:"), new QLabel(i18nc("Connection type", "Wired")));
        m_layout->addRow(i18n("IP:"), new QLabel(status.Infos.at(0)));
    }
    else if (status.State == WicdState::WIRELESS) {
        m_layout->addRow(i18n("Connection type:"), new QLabel(i18nc("Connection type", "Wireless")));
        m_layout->addRow(i18n("ESSID:"), new QLabel(status.Infos.at(1)));
        m_layout->addRow(i18n("Speed:"), new QLabel(status.Infos.at(4)));
        m_layout->addRow(i18n("IP:"), new QLabel(status.Infos.at(0)));

        QString quality = status.Infos.at(2);
        QString unit = "%";
        if (quality.toInt() <= -10)
            unit = " dBm";
        m_layout->addRow(i18n("Signal strength:"), new QLabel(quality + unit));
    }
    else {
        m_layout->addRow(i18n("State:"), new QLabel(i18n("Disconnected")));
    }
}

InfosDialog::~InfosDialog()
{
}

void InfosDialog::init()
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_layout = new QFormLayout(this);
    m_layout->setLabelAlignment(Qt::AlignLeft);
    setLayout(m_layout);

    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), SLOT(updateColors()));
    updateColors();
}

void InfosDialog::updateColors()
{
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
    setPalette(pal);
}

#include "infosdialog.moc"
