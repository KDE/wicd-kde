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

#include "scriptsdialog.h"
#include "dbushandler.h"
#include "global.h"

#include <QFormLayout>
#include <QTimer>

#include <KPushButton>
#include <KMessageBox>
#include <KAuth/Action>
#include <KLocalizedString>


ScriptsDialog::ScriptsDialog(int networkId, QWidget *parent, Qt::WFlags flags)
    : KDialog(parent, flags)
    , m_networkId(networkId)
{
    setModal(true);
    setCaption(i18n("Configure scripts"));

    QWidget *widget = new QWidget(this);
    QFormLayout *formlayout = new QFormLayout();
    formlayout->setLabelAlignment(Qt::AlignLeft);

    m_precon = new QLineEdit();
    m_postcon = new QLineEdit();
    m_predis = new QLineEdit();
    m_postdis = new QLineEdit();

    //arguments for the helper
    QVariantMap args;
    QString type = (m_networkId == -1) ? "wired" : "wireless";
    args["filename"] = Wicd::wicdpath+type+"-settings.conf";
    //m_key: bssid if wireless, profilename if wired
    if (m_networkId == -1) {
        m_key = Wicd::currentprofile;
    } else {
        m_key = DBusHandler::instance()->callWireless("GetWirelessProperty", m_networkId, "bssid").toString();
    }
    args["key"] = m_key;

    KAuth::Action *readscriptsAction = new KAuth::Action(QString("org.kde.wicdclient.scripts.read"));
    readscriptsAction->setHelperID("org.kde.wicdclient.scripts");
    readscriptsAction->setArguments(args);
    KAuth::ActionReply readReply = readscriptsAction->execute();
    if (readReply.failed()) {
        KMessageBox::sorry(this, i18n("KAuth returned an error code: %1", readReply.errorCode()));
        QTimer::singleShot(0, this, SLOT(close()));
        return;
    }

    formlayout->addRow(i18n("Pre-connection script"), m_precon);
    formlayout->addRow(i18n("Post-connection script"), m_postcon);
    formlayout->addRow(i18n("Pre-disconnection script"), m_predis);
    formlayout->addRow(i18n("Post-disconnection script"), m_postdis);

    m_precon->setText(Tools::noneToBlank(readReply.data().value("beforescript").toString()));
    m_postcon->setText(Tools::noneToBlank(readReply.data().value("afterscript").toString()));
    m_predis->setText(Tools::noneToBlank(readReply.data().value("predisconnectscript").toString()));
    m_postdis->setText(Tools::noneToBlank(readReply.data().value("postdisconnectscript").toString()));
    
    KAuth::Action *savescriptsAction = new KAuth::Action(QString("org.kde.wicdclient.scripts.save"));
    savescriptsAction->setHelperID("org.kde.wicdclient.scripts");
    savescriptsAction->setArguments(args);
    button(KDialog::Ok)->setAuthAction(savescriptsAction);
    connect(button(KDialog::Ok), SIGNAL(authorized(KAuth::Action*)), this, SLOT(save(KAuth::Action*)));

    widget->setLayout(formlayout);
    setMainWidget( widget );
}

ScriptsDialog::~ScriptsDialog()
{
    delete m_precon;
    delete m_postcon;
    delete m_predis;
    delete m_postdis;
}

void ScriptsDialog::save(KAuth::Action *action)
{
    action->addArgument("beforescript", Tools::blankToNone(m_precon->text()));
    action->addArgument("afterscript", Tools::blankToNone(m_postcon->text()));
    action->addArgument("predisconnectscript", Tools::blankToNone(m_predis->text()));
    action->addArgument("postdisconnectscript", Tools::blankToNone(m_postdis->text()));
    KAuth::ActionReply reply = action->execute();
    if (reply.failed()) {
        KMessageBox::sorry(this, i18n("KAuth returned an error code: %1", reply.errorCode()));
    } else {
        if (m_networkId == -1) {
            DBusHandler::instance()->callWired("ReloadConfig");
            DBusHandler::instance()->callWired("ReadWiredNetworkProfile", m_key);
            DBusHandler::instance()->callWired("SaveWiredNetworkProfile", m_key);
        } else {
            DBusHandler::instance()->callWireless("ReloadConfig");
            DBusHandler::instance()->callWireless("ReadWirelessNetworkProfile", m_networkId);
            DBusHandler::instance()->callWireless("SaveWirelessNetworkProfile", m_networkId);
        }
    }
}
