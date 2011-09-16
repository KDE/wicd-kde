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

#include "networkview.h"

NetworkView::NetworkView( QGraphicsItem *parent )
    : QGraphicsWidget( parent )
{
    setAcceptHoverEvents(true);
    installEventFilter(this);
    m_layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    
    m_itemBackground = new Plasma::ItemBackground(this);
    m_itemBackground->setTargetItem(0);
}

NetworkView::~NetworkView()
{
}

void NetworkView::loadList(const QMap<int, NetworkInfos> &list)
{
    //delete old list
    while (m_networkItemList.count() > 0) {
        NetworkItem *oldItem = m_networkItemList.at(0);
        m_layout->removeItem(oldItem);
        m_networkItemList.removeAt(0);
        oldItem->deleteLater();
    }

    //populate new list
    QMap<int, NetworkInfos>::const_iterator it = list.constBegin();
    while (it != list.constEnd()) {
        NetworkItem* item = new NetworkItem(it.value(), this);
        item->installEventFilter(this);
        m_networkItemList.append(item);
        m_layout->addItem(item);
        ++it;
    }
    updateGeometry();
}

bool NetworkView::eventFilter(QObject* obj, QEvent *event)
{
    NetworkItem *item = qobject_cast<NetworkItem*>(obj);
    if (item) {
        switch (event->type()) {
        case QEvent::GraphicsSceneHoverEnter:
            highlightItem(item);
            break;
        default:
            break;
        }
    }
    QGraphicsWidget *w = qobject_cast<QGraphicsWidget*>(obj);
    if (w == this && event->type() == QEvent::GraphicsSceneHoverLeave) {
        highlightItem(0);
    }

    return false;
}

void NetworkView::showSignalStrength(bool show)
{
    NetworkItem::showStrength(show);
}

void NetworkView::highlightItem(QGraphicsItem* item)
{
    m_itemBackground->setTargetItem(item);
}

