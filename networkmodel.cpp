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

#include "networkmodel.h"

NetworkModel::NetworkModel( QObject *parent )
    : QAbstractListModel( parent )
{
}

NetworkModel::~NetworkModel()
{
}

void NetworkModel::loadData(const QMap<int, NetworkInfos> &data)
{
    clear();
    m_networks = data;
    beginInsertRows(QModelIndex(), 0, m_networks.size());
    endInsertRows();
}

void NetworkModel::clear()
{
    reset();
    m_networks.clear();
}

int NetworkModel::rowCount(const QModelIndex &) const
{
    return m_networks.size();
}

QVariant NetworkModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_networks.size()) {
        return QVariant();
    }

    QMap<int, NetworkInfos>::const_iterator it = m_networks.constBegin();
    for (int i = 0; i < index.row(); ++i) {
        ++it;
    }

    switch (role) {
    case IdRole:
        return (*it).value("networkId");
    case EssidRole:
        return (*it).value("essid");
    case QualityRole:
        return (*it).value("quality");
    case EncryptionRole:
        return (*it).value("encryption");
    case IsConnectedRole:
        return (*it).value("connected");
    default:
        return QVariant();
    }
}
