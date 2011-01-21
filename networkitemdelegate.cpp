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

#include "networkitemdelegate.h"
#include "networkmodel.h"
#include "networkpropertiesdialog.h"
#include "infosdialog.h"
#include "profilemanager.h"
#include "dbushandler.h"
#include "global.h"

#include <QApplication>
#include <QPainter>
#include <QProgressBar>

#include <KIcon>
#include <KPushButton>

bool NetworkItemDelegate::m_useTooltips(false);
bool NetworkItemDelegate::m_showStrength(false);

NetworkItemDelegate::NetworkItemDelegate(QAbstractItemView *itemView, QObject *parent)
    : KWidgetItemDelegate(itemView, parent)
{
}

void NetworkItemDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{

    int networkId = index.model()->data(index, NetworkModel::IdRole).toInt();
    QString essid = index.model()->data(index, NetworkModel::EssidRole).toString();
    bool usedbm = index.model()->data(index, NetworkModel::UseDbmRole).toBool();
    int strength = index.model()->data(index, NetworkModel::StrengthRole).toInt();
    int quality = index.model()->data(index, NetworkModel::QualityRole).toInt();
    bool encryption = index.model()->data(index, NetworkModel::EncryptionRole).toBool();

    // highlight selected item
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->save();

    //draw icon
    KIcon icon;
    if (networkId == -1) {
        //wired
        icon = KIcon("network-wired");
    } else {
        //wireless
        if (quality <= 25)
            icon = KIcon("network-wireless-connected-25");
        else if (quality <= 50)
            icon = KIcon("network-wireless-connected-50");
        else if (quality <= 75)
            icon = KIcon("network-wireless-connected-75");
        else
            icon = KIcon("network-wireless-connected-100");
    }
    QRect iconRect = option.rect.translated(m_spacer, (sizeHint(option, index).height()-m_iconsize)/2);
    painter->drawPixmap(iconRect.topLeft(), icon.pixmap(m_iconsize, m_iconsize));

    if (encryption)  {
        KIcon emblem("emblem-locked");
        painter->drawPixmap(iconRect.translated(0, 1.5*m_iconsize/2.5).topLeft(),
                            emblem.pixmap(m_iconsize/2.5, m_iconsize/2.5));
    }

    // draw text
    int flags = Qt::AlignLeft | Qt::TextWordWrap;
    int verticalShift = 5;

    QFont font = painter->font();
    font.setWeight(QFont::Bold);
    painter->setFont(font);
    painter->drawText(option.rect.translated(m_spacer+m_iconsize+m_spacer, verticalShift),
                      flags,
                      essid);
    if (networkId == -1) {
        font.setWeight(QFont::Normal);
        font.setItalic(true);
        painter->setFont(font);
        painter->drawText(option.rect.translated(m_spacer+m_iconsize+m_spacer, 22),
                          flags,
                          Wicd::currentprofile);
    } else if (m_showStrength) {
	font.setWeight(QFont::Normal);
        font.setItalic(true);
        painter->setFont(font);
      	QString signal;
        if (usedbm)
            signal = QString::number(strength)+" dBm";
	else
            signal = QString::number(quality)+"%";
        painter->drawText( option.rect.translated( m_spacer+m_iconsize+m_spacer+m_progressbarlength+m_spacer, 22 ), signal );
    }
    painter->restore();
}

NetworkItemDelegate::~NetworkItemDelegate()
{
}

QSize NetworkItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return QSize(200, 42);
}

QList<QWidget*> NetworkItemDelegate::createItemWidgets() const
{
    QProgressBar *progressBar = new QProgressBar();
    progressBar->setFixedWidth(m_progressbarlength);
    progressBar->setMaximumHeight(10);
    progressBar->setTextVisible(false);

    QPushButton *connectButton = new KPushButton(KIcon("network-connect"), "");
    connect(connectButton, SIGNAL(clicked()), this, SLOT(toggleConnection()));

    QPushButton *propertiesButton = new KPushButton(KIcon("configure"), "");
    connect(propertiesButton, SIGNAL(clicked()), this, SLOT(editProperties()));

    QPushButton *informationsButton = new KPushButton(KIcon("dialog-information"), "");
    connect(informationsButton, SIGNAL(clicked()), this, SLOT(showInfos()));

    QPushButton *profilemanagerButton = new KPushButton(KIcon("user-identity"), "");
    connect(profilemanagerButton, SIGNAL(clicked()), this, SLOT(showProfileManager()));

    return QList<QWidget*>() << progressBar << connectButton << propertiesButton << informationsButton << profilemanagerButton;
}

void NetworkItemDelegate::updateItemWidgets (const QList<QWidget*> widgets,
                                             const QStyleOptionViewItem &option,
                                             const QPersistentModelIndex &index) const
{
    Q_UNUSED(option)

    //progress bar
    QProgressBar *progressbar = qobject_cast<QProgressBar*>(widgets[0]);
    progressbar->setValue(index.model()->data(index, NetworkModel::QualityRole).toInt());
    progressbar->move(m_spacer+m_iconsize+m_spacer, 25);

    //connect button
    QPushButton *buttonconnect = qobject_cast<QPushButton*>(widgets[1]);
    bool isConnected = index.model()->data(index, NetworkModel::IsConnectedRole).toBool();
    if (isConnected) {
        buttonconnect->setIcon(KIcon("network-disconnect"));
    } else {
        buttonconnect->setIcon(KIcon("network-connect"));
    }
    //properties button
    QPushButton *propertiesbutton = qobject_cast<QPushButton*>(widgets[2]);
    //informations button
    QPushButton *informationsbutton = qobject_cast<QPushButton*>(widgets[3]);
    //informations button
    QPushButton *profilemanagerbutton = qobject_cast<QPushButton*>(widgets[4]);

    if (m_useTooltips) {
        buttonconnect->setToolTip( isConnected ? i18n("Disconnect") : i18n("Connect"));
        propertiesbutton->setToolTip(i18n("Properties"));
        informationsbutton->setToolTip(i18n("Information"));
        profilemanagerbutton->setToolTip(i18n("Manage profiles"));
    } else {
        buttonconnect->setToolTip("");
        propertiesbutton->setToolTip("");
        informationsbutton->setToolTip("");
        profilemanagerbutton->setToolTip("");
    }

    int verticalShift = (sizeHint(option, index).height()-buttonconnect->height())/2+1;
    buttonconnect->move(option.rect.translated(-buttonconnect->width()-m_spacer
                                               -propertiesbutton->width()-m_spacer
                                               -informationsbutton->width()-m_spacer, 0).topRight().x(), verticalShift);
    propertiesbutton->move(buttonconnect->pos().x()+buttonconnect->width()+m_spacer, verticalShift);
    informationsbutton->move(propertiesbutton->pos().x()+propertiesbutton->width()+m_spacer, verticalShift);
    profilemanagerbutton->move(propertiesbutton->pos().x()+propertiesbutton->width()+m_spacer, verticalShift);

    if (index.model()->data(index, NetworkModel::IdRole).toInt() == -1) {
        progressbar->hide();
        informationsbutton->hide();
    } else {
        profilemanagerbutton->hide();
    }
}

void NetworkItemDelegate::toggleConnection()
{
    const QPersistentModelIndex index = focusedIndex();
    if (!index.isValid()) {
        //the focus was not on the list view, *here* the index is invalid
        return;
    }

    bool networkConnected = index.model()->data(index, NetworkModel::IsConnectedRole).toBool();
    int networkId = index.model()->data(index, NetworkModel::IdRole).toInt();

    if (networkConnected) {
        if (networkId == -1) {
            DBusHandler::instance()->callWired("DisconnectWired");
        } else {
            DBusHandler::instance()->callWireless("DisconnectWireless");
        }
        DBusHandler::instance()->callDaemon("SetForcedDisconnect", true);
    } else {
        if (networkId == -1) {
            DBusHandler::instance()->callWired("ConnectWired");
        } else {
            DBusHandler::instance()->callWireless("ConnectWireless", networkId);
        }
    }
}

void NetworkItemDelegate::editProperties()
{
    NetworkPropertiesDialog dialog(focusedIndex().model()->data(focusedIndex(), NetworkModel::IdRole).toInt());
    dialog.setCaption(focusedIndex().model()->data(focusedIndex(), NetworkModel::EssidRole).toString());
    dialog.exec();
}

void NetworkItemDelegate::showInfos()
{
    InfosDialog dialog(focusedIndex().model()->data(focusedIndex(), NetworkModel::IdRole).toInt());
    dialog.exec();
}

void NetworkItemDelegate::showProfileManager()
{
    ProfileManager manager;
    manager.exec();
}

