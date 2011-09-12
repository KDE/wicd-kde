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

#include "networkitem.h" 
#include "networkicon.h"
#include "dbushandler.h"
#include "global.h"
#include "networkpropertiesdialog.h"
#include "profilemanager.h"

#include <QGraphicsLinearLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsView>

#include <KIcon>

#include <Plasma/ToolButton>
#include <Plasma/Meter>
#include <Plasma/Theme>

bool NetworkItem::m_showStrength(false);
static const int buttonSize = 16;

NetworkItem::NetworkItem(NetworkInfos info, QGraphicsWidget *parent)
    : QGraphicsWidget(parent),
      m_dialog(0)
{
    m_infos = info;
    //allow item highlighting
    setAcceptHoverEvents(true);
    setCacheMode(DeviceCoordinateCache);
    
    NetworkIcon *networkIcon = new NetworkIcon(this);
    
    if (m_infos.value("connected").toBool()) {
        networkIcon->setConnected(true);
    }
    connect(networkIcon, SIGNAL(clicked()), this, SLOT(toggleConnection()));
    
    //config button for all
    Plasma::IconWidget *configButton = new Plasma::IconWidget(this);
    configButton->setMaximumSize(configButton->sizeFromIconSize(buttonSize));
    configButton->setIcon(KIcon("configure"));
    connect(configButton, SIGNAL(clicked()), this, SLOT(askProperties()));
    
    //variantButton opens either an infodialog or the profilemanager
    Plasma::IconWidget *variantButton = new Plasma::IconWidget(this);
    variantButton->setMaximumSize(variantButton->sizeFromIconSize(buttonSize));
    
    Plasma::Meter *qualityBar(0); //for wireless connection
    
    if (m_infos.value("networkId").toInt() == -1) {
        //wired
        networkIcon->setText(Wicd::currentprofile);
        networkIcon->setIcon("network-wired");
        variantButton->setIcon(KIcon("user-identity"));
        connect(variantButton, SIGNAL(clicked()), this, SLOT(askProfileManager()));
    } else {
        //wireless
        networkIcon->setText(m_infos.value("essid").toString());
        networkIcon->setIcon("network-wireless");
        if (m_infos.value("encryption").toBool()) {
            networkIcon->setEncrypted(true);
        }
        variantButton->setIcon(KIcon("dialog-information"));
        connect(variantButton, SIGNAL(clicked()), this, SLOT(askInfos()));
        //add signal quality
        qualityBar = new Plasma::Meter(this);
        qualityBar->setMeterType(Plasma::Meter::BarMeterHorizontal);
        qualityBar->setPreferredWidth(100);
        qualityBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        qualityBar->setMinimum(0);
        qualityBar->setMaximum(100);
        qualityBar->setValue(m_infos.value("quality").toInt());

        if (m_showStrength) {
            Plasma::Theme* theme = Plasma::Theme::defaultTheme();
            QFont font = theme->font(Plasma::Theme::DefaultFont);
            font.setPointSize(7.5);
            qualityBar->setLabelFont(0, font);
            qualityBar->setLabelAlignment(0, Qt::AlignVCenter | Qt::AlignLeft);
            bool usedbm = m_infos.value("usedbm").toBool();
            QString signal = usedbm ? m_infos.value("strength").toString()+" dBm" : m_infos.value("quality").toString()+"%";
            qualityBar->setLabel(0, signal);
        } else {
            qualityBar->setMaximumHeight(12);
        }
    }
    
    QGraphicsLinearLayout *lay = new QGraphicsLinearLayout(this);
    lay->addItem(networkIcon);
    lay->setAlignment(networkIcon, Qt::AlignVCenter);
    lay->addStretch();
    if (qualityBar) {
        lay->addItem(qualityBar);
        lay->setAlignment(qualityBar, Qt::AlignVCenter);
    }
    lay->addItem(configButton);
    lay->setAlignment(configButton, Qt::AlignVCenter);
    lay->addItem(variantButton);
    lay->setAlignment(variantButton, Qt::AlignVCenter);
}

NetworkItem::~NetworkItem()
{
    if (m_dialog)
        m_dialog->deleteLater();
}

void NetworkItem::toggleConnection()
{
    bool networkConnected = m_infos.value("connected").toBool();
    int networkId = m_infos.value("networkId").toInt();

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

void NetworkItem::askProperties()
{
    NetworkPropertiesDialog dialog(m_infos.value("networkId").toInt());
    dialog.setCaption(m_infos.value("essid").toString());
    dialog.exec();
}

void NetworkItem::askInfos()
{
    //move the dialog on the networkitem
    QGraphicsView *gv(0);
    // We position the popup relative to the view under the mouse cursor
    foreach (QGraphicsView *view, scene()->views()) {
        QPoint mousepos = view->mapFromGlobal(QCursor::pos());
        if (view->geometry().contains(mousepos)) {
            gv = view;
            break;
        }
    }
    const QPoint position = gv ? gv->mapToGlobal(gv->mapFromScene(scenePos())) : QPoint();

    const QRect available = QApplication::desktop()->availableGeometry(position);
    dialog()->adjustSize();
    QPoint pt = position;

    if (pt.x() + dialog()->width() > available.right()) {
        pt.rx() = available.right() - dialog()->width();
    }
    if (pt.x() < available.left()) {
        pt.rx() = available.left();
    }

    if (pt.y() + dialog()->height() > available.bottom()) {
        pt.ry() = available.bottom() - dialog()->height();
    }
    if (pt.y() < available.top()) {
        pt.ry() = available.top();
    }

    dialog()->move(pt);
    dialog()->animatedShow(Plasma::Down);
}

void NetworkItem::askProfileManager()
{
    ProfileManager manager;
    manager.exec();
}

InfosDialog* NetworkItem::dialog()
{
    if (!m_dialog)
        m_dialog = new InfosDialog(m_infos.value("networkId").toInt());
    return m_dialog;
}
