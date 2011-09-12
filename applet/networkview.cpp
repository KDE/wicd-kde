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

NetworkView::NetworkView( QGraphicsWidget *parent  = 0)
    : Plasma::ScrollWidget( parent )
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setOverflowBordersVisible(false); //workaround for lateral borders appearing on first load for no apparent reason
    setMaximumHeight(400);
    setMinimumWidth(350);
    
    m_listview = new QGraphicsWidget(this);
    m_listview->setAcceptHoverEvents(true);
    m_listview->installEventFilter(this);
    m_listviewLayout = new QGraphicsLinearLayout(Qt::Vertical, m_listview);
    
    setOverShoot(false); //doesn't work?
    setWidget(m_listview);
    
    m_itemBackground = new Plasma::ItemBackground(m_listview);
    m_itemBackground->setTargetItem(0);
    
    m_busyWidget = new Plasma::BusyWidget(this);
    m_busyWidget->hide();
}

NetworkView::~NetworkView()
{
}

void NetworkView::setBusy(bool busy)
{
    if (busy) {
        m_listview->hide();
        m_busyWidget->resize(viewportGeometry().size());
        m_busyWidget->show();
    } else {
        m_busyWidget->hide();
        m_listview->show();
    }
}

void NetworkView::loadList(const QMap<int, NetworkInfos> &list)
{
    //delete old list
    while (m_networkItemList.count() > 0) {
        NetworkItem *oldItem = m_networkItemList.at(0);
        m_listviewLayout->removeItem(oldItem);
        m_networkItemList.removeAt(0);
        oldItem->deleteLater();
    }

    //populate new list
    QMap<int, NetworkInfos>::const_iterator it = list.constBegin();
    while (it != list.constEnd()) {
        NetworkItem* item = new NetworkItem(it.value(), this/*m_listview*/);
        item->installEventFilter(this);
        m_networkItemList.append(item);
        m_listviewLayout->addItem(item);
        ++it;
    }
    
    //update scrollwidget
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
            //             case QEvent::GraphicsSceneHoverMove:
            //                 if (m_notifier->poppedUpInternally()) {
            //                     m_notifier->showPopup(DeviceNotifier::LONG_NOTIFICATION_TIMEOUT);
            //                 }
            //                 break;
            //             case QEvent::GraphicsSceneMousePress:
            //                 m_notifier->keepPopupOpen();
            //                 break;
        default:
            break;
        }
    }
    QGraphicsWidget *widget = qobject_cast<QGraphicsWidget*>(obj);
    if (widget == m_listview && event->type() == QEvent::GraphicsSceneHoverLeave) {
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

