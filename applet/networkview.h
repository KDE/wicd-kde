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

#ifndef NETWORKVIEW_H
#define NETWORKVIEW_H

#include "types.h"
#include "networkitem.h"

#include <QGraphicsLinearLayout>

#include <Plasma/DataEngine>
#include <Plasma/ItemBackground>


class NetworkView : public QGraphicsWidget
{
    Q_OBJECT

public:
    NetworkView(QGraphicsItem *parent = 0);
    ~NetworkView();
    void loadNetworks();
    void showSignalStrength(bool show);
    
protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void toggleConnection(int networkId);
    void highlightItem(QGraphicsItem* item);

private:
    Plasma::DataEngine* engine();

    QList<NetworkItem*> m_networkItemList;
    QGraphicsLinearLayout* m_layout;
    Plasma::ItemBackground *m_itemBackground;
    Plasma::Service *m_controller;
};

#endif

