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
#include "networkicon.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

#include <Plasma/IconWidget>
#include <Plasma/Animation>

class NetworkItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    NetworkItem(NetworkInfo info, QGraphicsWidget *parent);
    ~NetworkItem();
    void collapse();

protected:
    virtual QGraphicsWidget *moreWidget() = 0;
    
private slots:
    void toggleConnection();
    void animationFinished();
    void askProperties();
    void askMore();
    
signals:
    void toggled(int id);

protected:
    NetworkInfo m_info;
    NetworkIcon *m_networkIcon;
    QGraphicsLinearLayout *m_hLayout;
    Plasma::Animation *m_infoFade;

private:
    Plasma::IconWidget *m_configButton;
    Plasma::IconWidget *m_moreButton;
    QGraphicsLinearLayout *m_vLayout;
    bool m_isExpanded;

};

#endif
