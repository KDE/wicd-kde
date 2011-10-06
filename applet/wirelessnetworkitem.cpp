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

#include "wirelessnetworkitem.h"

#include <QWidget>
#include <QLabel>
#include <QFormLayout>

#include <Plasma/Meter>
#include <Plasma/Theme>

bool WirelessNetworkItem::m_showStrength(false);

WirelessNetworkItem::WirelessNetworkItem(NetworkInfo info, QGraphicsWidget *parent)
    : NetworkItem(info, parent),
      m_infoWidget(0)
{
    m_networkIcon->setText(m_info.value("essid").toString());
    m_networkIcon->setIcon("network-wireless");
    if (m_info.value("encryption").toBool()) {
        m_networkIcon->setEncrypted(true);
    }

    //add signal quality
    Plasma::Meter *qualityBar = new Plasma::Meter(this);
    qualityBar->setMeterType(Plasma::Meter::BarMeterHorizontal);
    qualityBar->setPreferredWidth(100);
    qualityBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    qualityBar->setMinimum(0);
    qualityBar->setMaximum(100);
    qualityBar->setValue(m_info.value("quality").toInt());

    if (m_showStrength) {
        Plasma::Theme* theme = Plasma::Theme::defaultTheme();
        QFont font = theme->font(Plasma::Theme::DefaultFont);
        font.setPointSize(7.5);
        qualityBar->setLabelFont(0, font);
        qualityBar->setLabelAlignment(0, Qt::AlignVCenter | Qt::AlignLeft);
        bool usedbm = m_info.value("usedbm").toBool();
        QString signal = usedbm ? m_info.value("strength").toString()+" dBm" : m_info.value("quality").toString()+'%';
        qualityBar->setLabel(0, signal);
    } else {
        qualityBar->setMaximumHeight(12);
    }

    m_hLayout->insertItem(2, qualityBar);
    m_hLayout->setAlignment(qualityBar, Qt::AlignVCenter);

    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), SLOT(updateColors()));
    updateColors();
}

void WirelessNetworkItem::updateColors()
{
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
    setPalette(pal);
}

QGraphicsWidget* WirelessNetworkItem::moreWidget()
{
    if (!m_infoWidget) {
        m_infoWidget = new QGraphicsProxyWidget(this);
        m_infoFade->setTargetWidget(m_infoWidget);

        QWidget *widget = new QWidget();
        widget->setAttribute(Qt::WA_NoSystemBackground);
        QFormLayout *formLayout = new QFormLayout(widget);
        formLayout->setLabelAlignment(Qt::AlignLeft);
        widget->setLayout(formLayout);

        QString signal;
        if (m_info.value("usedbm").toBool())
            signal = m_info.value("strength").toString()+" dBm";
        else
            signal = m_info.value("quality").toString()+'%';
        formLayout->addRow(new QLabel(i18n("Signal strength:")), new QLabel(signal));

        QString encryption;
        if (m_info.value("encryption").toBool())
            encryption = m_info.value("encryptionType").toString();
        else
            encryption = i18n("Unsecured");
        formLayout->addRow(new QLabel(i18n("Encryption type:")), new QLabel(encryption));

        QString accessPoint = m_info.value("bssid").toString();
        formLayout->addRow(new QLabel(i18n("Access point address:")), new QLabel(accessPoint));

        QString mode = m_info.value("mode").toString();
        formLayout->addRow(new QLabel(i18n("Mode:")), new QLabel(mode));

        QString channel = m_info.value("channel").toString();
        formLayout->addRow(new QLabel(i18n("Channel:")), new QLabel(channel));

        m_infoWidget->setWidget(widget);
    }
    return m_infoWidget;
}
