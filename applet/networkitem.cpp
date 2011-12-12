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
#include "properties/networkpropertiesdialog.h"

#include <KIconLoader>

#include <Plasma/ToolButton>

NetworkItem::NetworkItem(NetworkInfo info, QGraphicsWidget *parent)
    : QGraphicsWidget(parent),
      m_info(info),
      m_isExpanded(false)
{
    //allow item highlighting
    setAcceptHoverEvents(true);
    setCacheMode(DeviceCoordinateCache);
    //force parent layout to shrink vertically
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    m_networkIcon = new NetworkIcon(this);
    m_networkIcon->setConnected(m_info.value("connected").toBool());
    connect(m_networkIcon, SIGNAL(clicked()), this, SLOT(toggleConnection()));

    const int buttonSize = KIconLoader::SizeSmall;
    //config button for all
    m_configButton = new Plasma::IconWidget(this);
    m_configButton->setMaximumSize(m_configButton->sizeFromIconSize(buttonSize));
    m_configButton->setSvg("widgets/configuration-icons", "configure");
    connect(m_configButton, SIGNAL(clicked()), this, SLOT(askProperties()));
    
    //m_moreButton opens either an infodialog or the profilemanager
    m_moreButton = new Plasma::IconWidget(this);
    m_moreButton->setMaximumSize(m_moreButton->sizeFromIconSize(buttonSize));
    m_moreButton->setSvg("widgets/action-overlays", "add-normal");
    connect(m_moreButton, SIGNAL(clicked()), this, SLOT(askMore()));

    m_vLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    m_hLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    m_vLayout->addItem(m_hLayout);
    m_hLayout->addItem(m_networkIcon);
    m_hLayout->setAlignment(m_networkIcon, Qt::AlignVCenter);
    m_hLayout->addStretch();
    m_hLayout->addItem(m_configButton);
    m_hLayout->setAlignment(m_configButton, Qt::AlignVCenter);
    m_hLayout->addItem(m_moreButton);
    m_hLayout->setAlignment(m_moreButton, Qt::AlignVCenter);

    m_infoFade = Plasma::Animator::create(Plasma::Animator::FadeAnimation);
    connect(m_infoFade, SIGNAL(finished()), this, SLOT(animationFinished()));
}

NetworkItem::~NetworkItem()
{
}

void NetworkItem::toggleConnection()
{
    emit toggled(m_info.value("networkId").toInt());
}

void NetworkItem::animationFinished()
{
    m_isExpanded = !m_isExpanded;
    if (!m_isExpanded) {
        m_vLayout->removeItem(moreWidget());
    }
}

void NetworkItem::askProperties()
{
    QPointer<NetworkPropertiesDialog> dialog = new NetworkPropertiesDialog(m_info);
    dialog->open();
}

void NetworkItem::askMore()
{
    if (m_isExpanded) {
        m_moreButton->setSvg("widgets/action-overlays", "add-normal");
        m_infoFade->setProperty("startOpacity", 1.0);
        m_infoFade->setProperty("targetOpacity", 0.0);
        m_infoFade->start();
    } else {
        m_moreButton->setSvg("widgets/action-overlays", "remove-normal");
        m_vLayout->addItem(moreWidget());
        m_infoFade->setProperty("startOpacity", 0.0);
        m_infoFade->setProperty("targetOpacity", 1.0);
        m_infoFade->start();
    }
}
