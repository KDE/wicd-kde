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

#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QGraphicsWidget>

#include <Plasma/DataEngine>
#include <Plasma/CheckBox>
#include <Plasma/ComboBox>
#include <Plasma/Dialog>

class ProfileWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    ProfileWidget(QGraphicsItem * parent = 0, Qt::WindowFlags wFlags = 0);
    Plasma::DataEngine* engine();

signals:
    void profileSelected(QString profile);

private slots:
    void toggleDefault(bool toggle);
    void profileChanged(QString profile);
    void addProfile();
    void removeProfile();

private:
    Plasma::Service *m_wicdService;
    Plasma::CheckBox *m_defaultBox;
    Plasma::ComboBox *m_comboBox;

};

class ProfileDialog : public Plasma::Dialog
{
    Q_OBJECT

public:
    ProfileDialog(QGraphicsWidget *parent);

private slots:
    void accepted();

private:
    ProfileWidget *m_profileWidget;
};

#endif // PROFILEWIDGET_H
