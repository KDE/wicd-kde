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

#include "networkitem.h" 
#include "networkicon.h"
#include "dbushandler.h"
#include "global.h"
#include "networkpropertiesdialog.h"
#include "profilemanager.h"

#include <QFormLayout>

#include <KIcon>

#include <Plasma/ToolButton>
#include <Plasma/Meter>
#include <Plasma/Theme>

bool NetworkItem::m_showStrength(false);
static const int buttonSize = 16;

NetworkItem::NetworkItem(NetworkInfos info, QGraphicsWidget *parent)
    : QGraphicsWidget(parent),
      m_infoWidget(0),
      m_isExpanded(false)
{
    m_infos = info;
    //allow item highlighting
    setAcceptHoverEvents(true);
    setCacheMode(DeviceCoordinateCache);
    //force parent layout to shrink vertically
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    NetworkIcon *networkIcon = new NetworkIcon(this);
    
    if (m_infos.value("connected").toBool()) {
        networkIcon->setConnected(true);
    }
    connect(networkIcon, SIGNAL(clicked()), this, SLOT(toggleConnection()));
    
    //config button for all
    Plasma::IconWidget *configButton = new Plasma::IconWidget(this);
    configButton->setMaximumSize(configButton->sizeFromIconSize(buttonSize));
    configButton->setSvg("widgets/configuration-icons", "configure");
    connect(configButton, SIGNAL(clicked()), this, SLOT(askProperties()));
    
    //m_variantButton opens either an infodialog or the profilemanager
    m_variantButton = new Plasma::IconWidget(this);
    m_variantButton->setMaximumSize(m_variantButton->sizeFromIconSize(buttonSize));
    
    Plasma::Meter *qualityBar(0); //for wireless connection
    
    if (m_infos.value("networkId").toInt() == -1) {
        //wired
        networkIcon->setText(Wicd::currentprofile);
        networkIcon->setIcon("network-wired");
        m_variantButton->setIcon(KIcon("user-identity"));
        connect(m_variantButton, SIGNAL(clicked()), this, SLOT(askProfileManager()));
    } else {
        //wireless
        networkIcon->setText(m_infos.value("essid").toString());
        networkIcon->setIcon("network-wireless");
        if (m_infos.value("encryption").toBool()) {
            networkIcon->setEncrypted(true);
        }
        m_variantButton->setSvg("widgets/action-overlays", "add-normal");
        connect(m_variantButton, SIGNAL(clicked()), this, SLOT(askInfos()));
        //add signal quality
        qualityBar = new Plasma::Meter(this);
        qualityBar->setMeterType(Plasma::Meter::BarMeterHorizontal);
        qualityBar->setPreferredWidth(100);
        qualityBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        qualityBar->setMinimum(0);
        qualityBar->setMaximum(100);
        qualityBar->setValue(m_infos.value("quality").toInt());

        if (m_showStrength) {
            Plasma::Theme* theme = Plasma::Theme::defaultTheme();
            QFont font = theme->font(Plasma::Theme::DefaultFont);
            font.setPointSize(7.5);
            qualityBar->setLabelFont(0, font);
            qualityBar->setLabelAlignment(0, Qt::AlignVCenter | Qt::AlignLeft);
            bool usedbm = m_infos.value("usedbm").toBool();
            QString signal = usedbm ? m_infos.value("strength").toString()+" dBm" : m_infos.value("quality").toString()+"%";
            qualityBar->setLabel(0, signal);
        } else {
            qualityBar->setMaximumHeight(12);
        }
    }
    
    m_vLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    QGraphicsLinearLayout *lay = new QGraphicsLinearLayout(Qt::Horizontal);
    m_vLayout->addItem(lay);
    lay->addItem(networkIcon);
    lay->setAlignment(networkIcon, Qt::AlignVCenter);
    lay->addStretch();
    if (qualityBar) {
        lay->addItem(qualityBar);
        lay->setAlignment(qualityBar, Qt::AlignVCenter);
    }
    lay->addItem(configButton);
    lay->setAlignment(configButton, Qt::AlignVCenter);
    lay->addItem(m_variantButton);
    lay->setAlignment(m_variantButton, Qt::AlignVCenter);

    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), SLOT(updateColors()));
    updateColors();
}

NetworkItem::~NetworkItem()
{
    if (m_infoWidget)
        m_infoWidget->deleteLater();
}

void NetworkItem::updateColors()
{
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
    setPalette(pal);
}

void NetworkItem::toggleConnection()
{
    emit toggled(m_infos.value("networkId").toInt());
}

void NetworkItem::animationFinished()
{
    m_isExpanded = !m_isExpanded;
    if (!m_isExpanded) {
        m_vLayout->removeItem(infoWidget());
    }
}

void NetworkItem::askProperties()
{
    NetworkPropertiesDialog dialog(m_infos.value("networkId").toInt());
    dialog.setCaption(m_infos.value("essid").toString());
    dialog.exec();
}

void NetworkItem::askInfos()
{
    if (m_isExpanded) {
        m_variantButton->setSvg("widgets/action-overlays", "add-normal");
        m_infoFade->setProperty("startOpacity", 1.0);
        m_infoFade->setProperty("targetOpacity", 0.0);
        m_infoFade->start();
    } else {
        m_variantButton->setSvg("widgets/action-overlays", "remove-normal");
        m_vLayout->addItem(infoWidget());
        m_infoFade->setProperty("startOpacity", 0.0);
        m_infoFade->setProperty("targetOpacity", 1.0);
        m_infoFade->start();
    }
}

void NetworkItem::askProfileManager()
{
    ProfileManager manager;
    manager.exec();
}

QGraphicsProxyWidget* NetworkItem::infoWidget()
{
    if (!m_infoWidget) {
        m_infoWidget = new QGraphicsProxyWidget(this);

        m_infoFade = Plasma::Animator::create(Plasma::Animator::FadeAnimation);
        connect(m_infoFade, SIGNAL(finished()), this, SLOT(animationFinished()));
        m_infoFade->setTargetWidget(m_infoWidget);

        QWidget *widget = new QWidget();
        widget->setAttribute(Qt::WA_NoSystemBackground);
        QFormLayout *formLayout = new QFormLayout(widget);
        formLayout->setLabelAlignment(Qt::AlignLeft);
        widget->setLayout(formLayout);

        int networkId = m_infos.value("networkId").toInt();
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
        formLayout->addRow(new QLabel(i18n("Encryption type:")), new QLabel(encryption));

        QString accessPoint = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "bssid").toString();
        formLayout->addRow(new QLabel(i18n("Access point address:")), new QLabel(accessPoint));

        QString mode = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "mode").toString();
        formLayout->addRow(new QLabel(i18n("Mode:")), new QLabel(mode));

        QString channel = DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, "channel").toString();
        formLayout->addRow(new QLabel(i18n("Channel:")), new QLabel(channel));

        m_infoWidget->setWidget(widget);
    }
    return m_infoWidget;
}
