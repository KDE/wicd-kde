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

#include "networkicon.h"

#include <QPainter>

#include <KIcon>

static const int iconsize = 30;

NetworkIcon::NetworkIcon(QGraphicsItem *parent)
    : Plasma::IconWidget(parent),
      m_encrypted(false),
      m_connected(false)
{
    setOrientation(Qt::Horizontal);
}

NetworkIcon::~NetworkIcon()
{
}

void NetworkIcon::setText(const QString &text) 
{
    Plasma::IconWidget::setText(text);
    setMinimumSize(sizeFromIconSize(iconsize));
    setMaximumSize(sizeFromIconSize(iconsize));
}

void NetworkIcon::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Plasma::IconWidget::paint(painter, option, widget);
    if (m_encrypted) {
        painter->drawPixmap(QRect(0,1.5*iconsize/2.5,iconsize/2.5, iconsize/2.5),
                            KIcon("emblem-locked").pixmap(iconsize/2.5, iconsize/2.5));
    }
    if (m_connected) {
        painter->drawPixmap(QRect(iconsize/2,iconsize/2,iconsize/2, iconsize/2),
                            KIcon("dialog-ok-apply").pixmap(iconsize/2, iconsize/2));
    }
}

void NetworkIcon::setEncrypted(bool encrypted)
{
    m_encrypted = encrypted;
}

void NetworkIcon::setConnected(bool connected)
{
    m_connected = connected;
}
