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

#ifndef PREFERENCESPANEL_H
#define PREFERENCESPANEL_H

#include <KTabWidget>
#include "labelentry.h"
#include <QCheckBox>
#include <QComboBox>

#include <QVBoxLayout>
#include <KPushButton>

class PreferencesPanel : public KTabWidget
{
    Q_OBJECT

public:
    PreferencesPanel( QWidget *parent = 0);
    ~PreferencesPanel();

public slots:
    void load();
    void save() const;

private:
    LabelEntry* wirelessinterfaceEdit;
    LabelEntry* wiredinterfaceEdit;
    QCheckBox* alwaysshowwiredBox;
    QCheckBox* alwaysswitchtowiredBox;
    QCheckBox* useglobaldnsBox;
    LabelEntry* dnsdomainEdit;
    LabelEntry* searchdomainEdit;
    LabelEntry* dns1Edit;
    LabelEntry* dns2Edit;
    LabelEntry* dns3Edit;
    QComboBox* wiredautoCombo;
    QCheckBox* autoreconBox;
    QCheckBox* notificationsBox;
    QComboBox* dhcpclientCombo;
    QComboBox* wiredlinkCombo;
    QComboBox* routetableCombo;
    QComboBox* driverCombo;
    QComboBox* backendCombo;
    QCheckBox* debuggingBox;
    QCheckBox* usedbmBox;
    QCheckBox* pingstaticBox;

    QStringList dhcplist;
    QStringList wiredlinklist;
    QStringList routetablelist;

};

class CollapsibleWidget : public QWidget
{
    Q_OBJECT

public:
    CollapsibleWidget( const QString& title, QWidget *parent = 0);
    ~CollapsibleWidget();
    void addWidget(QWidget* widget);
    void addLayout(QLayout* layout);

private slots:
    void expand(bool doexpand);

private:
    KPushButton* m_collapseButton;
    QVBoxLayout* m_bodyLayout;

};

#endif


