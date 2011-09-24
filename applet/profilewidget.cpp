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

#include "profilewidget.h"
#include "dbushandler.h"
#include "global.h"

#include <QGraphicsLinearLayout>

#include <KComboBox>
#include <KInputDialog>
#include <KLocalizedString>

#include <Plasma/PushButton>

ProfileWidget::ProfileWidget(QGraphicsItem * parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent, wFlags)
{
    QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout(Qt::Vertical);

    m_defaultBox = new Plasma::CheckBox(this);
    m_defaultBox->setText(i18n("Use as default profile"));
    vLayout->addItem(m_defaultBox);

    QGraphicsLinearLayout *hLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    m_comboBox = new Plasma::ComboBox(this);
    m_comboBox->nativeWidget()->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    hLayout->addItem(m_comboBox);
    Plasma::PushButton *addButton = new Plasma::PushButton(this);
    addButton->setIcon(KIcon("list-add"));
    hLayout->addItem(addButton);
    Plasma::PushButton *removeButton = new Plasma::PushButton(this);
    removeButton->setIcon(KIcon("list-remove"));
    hLayout->addItem(removeButton);
    hLayout->addStretch();
    vLayout->addItem(hLayout);

    setLayout(vLayout);

    QStringList profileList = DBusHandler::instance()->callWired("GetWiredProfileList").toStringList();
    m_comboBox->nativeWidget()->addItems(profileList);

    connect(m_defaultBox, SIGNAL(toggled(bool)),this, SLOT(toggleDefault(bool)));
    connect(m_comboBox, SIGNAL(textChanged(QString)),this, SLOT(profileChanged(QString)));
    connect(addButton, SIGNAL(clicked()),this, SLOT(addProfile()));
    connect(removeButton, SIGNAL(clicked()),this, SLOT(removeProfile()));

    int currentProfileIndex = profileList.indexOf(Wicd::currentprofile);
    m_comboBox->setCurrentIndex(currentProfileIndex);
}

void ProfileWidget::toggleDefault(bool toggle)
{
    if (toggle) {
        DBusHandler::instance()->callWired("UnsetWiredDefault");
    }
    DBusHandler::instance()->callWired("SetWiredProperty", "default", toggle);
    DBusHandler::instance()->callWired("SaveWiredNetworkProfile", m_comboBox->text());
}

void ProfileWidget::profileChanged(QString profile)
{
    DBusHandler::instance()->callWired("ReadWiredNetworkProfile", profile);
    m_defaultBox->setChecked(DBusHandler::instance()->callWired("GetWiredProperty", "default").toBool());
    emit profileSelected(profile);
    Wicd::currentprofile = profile;
}

void ProfileWidget::addProfile()
{
    bool ok;
    QString newprofile = KInputDialog::getText(i18n("Add a profile"),
                                               i18n("New profile name:"),
                                               QString(), &ok, 0);
    if ((!ok) || newprofile.isEmpty())
        return;

    QStringList profileList = DBusHandler::instance()->callWired("GetWiredProfileList").toStringList();
    if (profileList.contains(newprofile))//the profile already exists
        return;

    DBusHandler::instance()->callWired("CreateWiredNetworkProfile", newprofile, false);
    m_comboBox->nativeWidget()->insertItem(0, newprofile);
    m_comboBox->setCurrentIndex(0);
}

void ProfileWidget::removeProfile()
{
    QString profile = m_comboBox->text();
    DBusHandler::instance()->callWired("DeleteWiredNetworkProfile", profile);
    m_comboBox->nativeWidget()->removeItem(m_comboBox->currentIndex());
    m_comboBox->setCurrentIndex(0);
}

ProfileDialog::ProfileDialog(QGraphicsWidget *parent)
    : Plasma::Dialog(0, Qt::Popup)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QGraphicsWidget *mainWidget = new QGraphicsWidget(parent);
    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mainWidget->setLayout(mainLayout);
    ProfileWidget *profileWidget = new ProfileWidget(mainWidget);
    mainLayout->addItem(profileWidget);
    Plasma::PushButton *okButton = new Plasma::PushButton(mainWidget);
    okButton->setIcon(KIcon("dialog-ok"));
    okButton->setText(i18n("Ok"));
    QGraphicsLinearLayout *bottomLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    bottomLayout->addStretch();
    bottomLayout->addItem(okButton);
    mainLayout->addItem(bottomLayout);
    setGraphicsWidget(mainWidget);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accepted()));
}

void ProfileDialog::accepted()
{
    DBusHandler::instance()->callWired("ReadWiredNetworkProfile", Wicd::currentprofile);
    DBusHandler::instance()->callWired("ConnectWired");
}
