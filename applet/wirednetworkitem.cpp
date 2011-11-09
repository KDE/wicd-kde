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

#include "wirednetworkitem.h"
#include "global.h"

WiredNetworkItem::WiredNetworkItem(NetworkInfo info, QGraphicsWidget *parent)
    : NetworkItem(info, parent),
      m_profileWidget(0)
{
    //make the combobox pop down over the next networkitem in the list
    setZValue(110);

    Wicd::currentprofile = m_info.value("currentprofile").toString();
    m_networkIcon->setText(m_info.value("essid").toString()+": "+Wicd::currentprofile);
    m_networkIcon->setIcon("network-wired");
}

QGraphicsWidget* WiredNetworkItem::moreWidget()
{
    if (!m_profileWidget) {
        m_profileWidget = new ProfileWidget(this);
        connect(m_profileWidget, SIGNAL(profileSelected(QString)), this, SLOT(profileUpdated(QString)));
        m_infoFade->setTargetWidget(m_profileWidget);
    }
    return m_profileWidget;
}

void WiredNetworkItem::profileUpdated(QString profile)
{
    m_networkIcon->setText(m_info.value("essid").toString()+": "+profile);
}
