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

#ifndef NETWORKITEM_H
#define NETWORKITEM_H 

#include "types.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>

#include <Plasma/IconWidget>
#include <Plasma/Animation>

class NetworkItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    NetworkItem(NetworkInfos info, QGraphicsWidget *parent);
    ~NetworkItem();
    static void showStrength(bool show) { m_showStrength = show; }
    
private slots:
    void updateColors();
    void toggleConnection();
    void animationFinished();
    void askProperties();
    void askInfos();
    void askProfileManager();
    
private:
    QGraphicsProxyWidget *infoWidget();

    NetworkInfos m_infos;
    QGraphicsLinearLayout *m_vLayout;
    Plasma::IconWidget *m_variantButton;
    QGraphicsProxyWidget *m_infoWidget;
    Plasma::Animation *m_infoFade;
    bool m_isExpanded;

    static bool m_showStrength;
};

#endif
