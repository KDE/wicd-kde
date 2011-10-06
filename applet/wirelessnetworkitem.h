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

#ifndef WIRELESSNETWORKITEM_H
#define WIRELESSNETWORKITEM_H

#include "networkitem.h"

#include <QGraphicsProxyWidget>

class WirelessNetworkItem : public NetworkItem
{
    Q_OBJECT

public:
    WirelessNetworkItem(NetworkInfo info, QGraphicsWidget *parent);
    static void showStrength(bool show) { m_showStrength = show; }

protected:
    QGraphicsWidget *moreWidget();

private slots:
    void updateColors();

private:
    QGraphicsProxyWidget *m_infoWidget;

    static bool m_showStrength;
};

#endif // WIRELESSNETWORKITEM_H
