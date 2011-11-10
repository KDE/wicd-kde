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
#include "wirednetworkitem.h"
#include "wirelessnetworkitem.h"

#include <Plasma/DataEngineManager>

NetworkView::NetworkView( QGraphicsItem *parent )
    : QGraphicsWidget( parent ),
      m_currentNetworkItem(0)
{
    setAcceptHoverEvents(true);
    installEventFilter(this);
    m_layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    m_itemBackground = new Plasma::ItemBackground(this);
    m_itemBackground->setTargetItem(0);

    m_controller = engine()->serviceForSource("");
}

NetworkView::~NetworkView()
{
}

void NetworkView::loadNetworks()
{
    //delete old list
    while (m_networkItemList.count() > 0) {
        NetworkItem *oldItem = m_networkItemList.at(0);
        m_layout->removeItem(oldItem);
        m_networkItemList.removeAt(0);
        oldItem->deleteLater();
    }

    //get new data
    Plasma::DataEngine::Data list = engine()->query("networks");
    //Store the data in a QMap with int key
    QMap<int, NetworkInfo> networkMap;
    Plasma::DataEngine::Data::const_iterator i = list.constBegin();
    while (i != list.constEnd()) {
        networkMap.insert(i.key().toInt(), i.value().toHash());
        ++i;
    }

    //populate new list
    QMap<int, NetworkInfo>::const_iterator it = networkMap.constBegin();
    m_currentNetworkItem = 0;
    while (it != networkMap.constEnd()) {
        NetworkItem* item;
        NetworkInfo info = it.value();
        bool isWired = (info.value("networkId").toInt() == -1);
        if (isWired) {
            WiredNetworkItem* wireditem = new WiredNetworkItem(it.value(), this);
            item = wireditem;
        } else {
            WirelessNetworkItem* wirelessitem = new WirelessNetworkItem(it.value(), this);
            item = wirelessitem;
        }
        connect(item, SIGNAL(toggled(int)), this, SLOT(toggleConnection(int)));
        if (info.value("connected").toBool()) {
            m_currentNetworkItem = item;
        }
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
    WirelessNetworkItem::showStrength(show);
}

NetworkItem* NetworkView::currentNetworkItem() const
{
    return m_currentNetworkItem;
}

void NetworkView::toggleConnection(int networkId)
{
    if (m_controller) {
        KConfigGroup op = m_controller->operationDescription("toggleConnection");
        op.writeEntry("networkId", networkId);
        m_controller->startOperationCall(op);
    }
}

void NetworkView::highlightItem(QGraphicsItem* item)
{
    m_itemBackground->setTargetItem(item);
}

Plasma::DataEngine* NetworkView::engine()
{
    Plasma::DataEngine *e = Plasma::DataEngineManager::self()->engine("wicd");
    if (e->isValid()) {
        return e;
    } else {
        return 0;
    }
}

