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

#include "networkpanel.h"
#include "networkitemdelegate.h"

NetworkPanel::NetworkPanel( QWidget *parent  = 0)
    : QListView( parent )
{
    m_networkModel = new NetworkModel(this);

    setModel(m_networkModel);
    setItemDelegate(new NetworkItemDelegate(this));
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(toggleConnection(const QModelIndex &)));
}

NetworkPanel::~NetworkPanel()
{
}

void NetworkPanel::setBusy(bool busy)
{
    setEnabled(!busy);
}

void NetworkPanel::loadList(const QMap<int, NetworkInfos> &list)
{
    m_networkModel->loadData(list);
}

void NetworkPanel::showTooltips(bool show)
{
    NetworkItemDelegate::useTooltips(show);
}

void NetworkPanel::showSignalStrength(bool show)
{
    NetworkItemDelegate::showStrength(show);
}

void NetworkPanel::toggleConnection(const QModelIndex & index)
{
    NetworkItemDelegate *item = static_cast<NetworkItemDelegate*>(itemDelegate(index));
    item->toggleConnection();
}
