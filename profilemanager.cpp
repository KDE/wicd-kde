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

#include "profilemanager.h"
#include "dbushandler.h"
#include "global.h"

#include <KInputDialog>

#include <KLocalizedString>

ProfileManager::ProfileManager( QWidget *parent )
    : KDialog( parent )
{
    setCaption(i18n("Manage Profiles"));
    setButtons(KDialog::Ok);

    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    ui.addButton->setIcon(KIcon("list-add"));
    ui.removeButton->setIcon(KIcon("list-remove"));

    QStringList profileList = DBusHandler::instance()->callWired("GetWiredProfileList").toStringList();
    ui.comboBox->addItems(profileList);
    int currentProfileIndex = profileList.indexOf(Wicd::currentprofile);
    ui.comboBox->setCurrentIndex(currentProfileIndex);
    profileChanged(Wicd::currentprofile);

    connect(ui.defaultBox, SIGNAL(toggled(bool)),this, SLOT(toggleDefault(bool)));
    connect(ui.comboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(profileChanged(QString)));
    connect(ui.addButton, SIGNAL(clicked()),this, SLOT(addProfile()));
    connect(ui.removeButton, SIGNAL(clicked()),this, SLOT(removeProfile()));

    setMainWidget(widget);
    resize(0, 0);
}

void ProfileManager::toggleDefault(bool toggle)
{
    if (toggle) {
        DBusHandler::instance()->callWired("UnsetWiredDefault");
    }
    DBusHandler::instance()->callWired("SetWiredProperty", "default", toggle);
    DBusHandler::instance()->callWired("SaveWiredNetworkProfile", ui.comboBox->currentText());
}

void ProfileManager::profileChanged(QString profile)
{
    DBusHandler::instance()->callWired("ReadWiredNetworkProfile", profile);
    Wicd::currentprofile = profile;
    ui.defaultBox->setChecked(DBusHandler::instance()->callWired("GetWiredProperty", "default").toBool());
}

void ProfileManager::addProfile()
{
    bool ok;
    QString newprofile = KInputDialog::getText(i18n("Add a profile"),
                                               i18n("New profile name:"),
                                               QString(), &ok, this);
    if ((!ok) || newprofile.isEmpty())
        return;

    QStringList profileList = DBusHandler::instance()->callWired("GetWiredProfileList").toStringList();
    if (profileList.contains(newprofile))//the profile already exists
        return;
    
    DBusHandler::instance()->callWired("CreateWiredNetworkProfile", newprofile, false);
    ui.comboBox->insertItem(0, newprofile);
    ui.comboBox->setCurrentIndex(0);
    Wicd::currentprofile = newprofile;
}

void ProfileManager::removeProfile()
{
    QString profile = ui.comboBox->currentText();
    DBusHandler::instance()->callWired("DeleteWiredNetworkProfile", profile);
    ui.comboBox->removeItem(ui.comboBox->currentIndex());
    ui.comboBox->setCurrentIndex(0);
    Wicd::currentprofile = ui.comboBox->currentText();
}

#include "profilemanager.moc"
