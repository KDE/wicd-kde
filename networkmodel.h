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

#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include <QAbstractListModel>
#include "types.h"

class NetworkModel : public QAbstractListModel
{
public:
    enum {
        EssidRole = Qt::UserRole,
        QualityRole = Qt::UserRole + 1,
        EncryptionRole = Qt::UserRole + 2,
        IdRole = Qt::UserRole + 4,
        IsConnectedRole = Qt::UserRole + 5,
        UseDbmRole = Qt::UserRole + 6,
        StrengthRole = Qt::UserRole + 7
    };

    NetworkModel(QObject *parent = 0);
    virtual ~NetworkModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void loadData(const QMap<int, NetworkInfos> &data);
    void clear();

private:
    QMap<int, NetworkInfos> m_networks;
};

#endif
