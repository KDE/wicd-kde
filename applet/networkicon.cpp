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
#include <KIconLoader>

NetworkIcon::NetworkIcon(QGraphicsItem *parent)
    : Plasma::IconWidget(parent),
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
    setMinimumSize(sizeFromIconSize(KIconLoader::SizeMedium));
    setMaximumSize(sizeFromIconSize(KIconLoader::SizeMedium));
}

void NetworkIcon::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (m_connected) {
        QFont cFont = font();
        cFont.setBold(true);
        setFont(cFont);
    }

    Plasma::IconWidget::paint(painter, option, widget);

    const int emblemsize = KIconLoader::SizeSmall;
    if (m_connected) {
        painter->drawPixmap(QRect(emblemsize, size().height()-emblemsize, emblemsize, emblemsize),
                            KIcon("network-workgroup").pixmap(emblemsize, emblemsize));
    }

    if (!m_overlayIcon.isNull()) {
        painter->drawPixmap(0, size().height()-emblemsize, emblemsize, emblemsize,
                            m_overlayIcon.pixmap(emblemsize, emblemsize));
    }
}

void NetworkIcon::setOverlayIcon(KIcon icon)
{
    m_overlayIcon = icon;
}

void NetworkIcon::setConnected(bool connected)
{
    m_connected = connected;
}
