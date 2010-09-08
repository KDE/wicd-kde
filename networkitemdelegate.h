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

#ifndef NETWORKITEM_H
#define NETWORKITEM_H

#include <KWidgetItemDelegate>

class NetworkItemDelegate : public KWidgetItemDelegate
{
    Q_OBJECT
public:
    NetworkItemDelegate(QAbstractItemView *itemView, QObject *parent = 0);
    ~NetworkItemDelegate();

    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
    virtual  QList<QWidget*> createItemWidgets() const;
    virtual void updateItemWidgets (const QList<QWidget*> widgets,
                                    const QStyleOptionViewItem &option,
                                    const QPersistentModelIndex &index) const;

public slots:
    void toggleConnection();

private slots:
    void editProperties();
    void showInfos();
    void showProfileManager();

private:
    static const int m_iconsize = 30;
    static const int m_spacer = 4;
};

#endif

