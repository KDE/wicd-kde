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

#ifndef NETWORKPANEL_H
#define NETWORKPANEL_H

#include "networkmodel.h"

#include <QListView>

class NetworkPanel : public QListView
{
    Q_OBJECT

public:
    NetworkPanel( QWidget *parent );
    ~NetworkPanel();
    void setBusy(bool busy);
    void loadList(const QMap<int, NetworkInfos> &list);
    void showTooltips(bool show);
    void showSignalStrength(bool show);

private slots:
    void toggleConnection(const QModelIndex &index);

private:
    NetworkModel* m_networkModel;

};

#endif

