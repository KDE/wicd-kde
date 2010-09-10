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

#include "mainwindow.h"
#include "global.h"
#include "preferencespanel.h"

#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QInputDialog>

#include <KDialog>
#include <KStatusBar>
#include <KMessageBox>
#include <KLocalizedString>

MainWindow::MainWindow()
    : KMainWindow()
{
    setAttribute(Qt::WA_DeleteOnClose, false);
    Wicd::locate();

    //Toolbar
    QToolButton *moreButton = new QToolButton();
    moreButton->setIcon(KIcon("network-wireless"));
    moreButton->setText(i18n("Network"));
    moreButton->setToolButtonStyle(Qt::ToolButtonFollowStyle);

    QMenu *moreMenu = new QMenu();
    moreMenu->addAction(KIcon("list-add"), i18n("Create an ad-hoc network"), this, SLOT(createAdhocDialog()));
    moreMenu->addAction(KIcon("edit-find"), i18n("Find a hidden network"), this, SLOT(findHiddenDialog()));
    moreButton->setMenu(moreMenu);
    moreButton->setPopupMode(QToolButton::InstantPopup);

    QToolBar *toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    toolbar->addWidget(moreButton);
    toolbar->addSeparator();
    toolbar->addAction(KIcon("preferences-system-network"), i18n("Preferences"), this, SLOT(showPreferences()));
    toolbar->addSeparator();
    toolbar->addAction(KIcon("view-refresh"), i18n("Reload"), DBusHandler::instance(), SLOT(scan()));
    toolbar->addAction(KIcon("network-disconnect"), i18n("Disconnect"), DBusHandler::instance(), SLOT(disconnect()));

    addToolBar(toolbar);

    //Main panel
    m_networkPanel = new NetworkPanel(this);
    setCentralWidget(m_networkPanel);

    //status bar
    m_abortButton = new KPushButton(KIcon("dialog-cancel"), i18n("Abort"), this);
    statusBar()->addPermanentWidget(m_abortButton);

    //tray icon
    m_trayicon = new TrayIcon(this);
    m_trayicon->setAssociatedWidget(this);

    //autosave window size
    setAutoSaveSettings();

    connect(m_trayicon, SIGNAL(activateRequested(bool, const QPoint)), this, SLOT(activated()));
    connect(DBusHandler::instance(), SIGNAL(statusChange(Status)), this, SLOT(updateStatus(Status)));
    connect(DBusHandler::instance(), SIGNAL(connectionResultSend(QString)), this, SLOT(handleConnectionResult(QString)));
    connect(DBusHandler::instance(), SIGNAL(scanStarted()), this, SLOT(freeze()));
    connect(DBusHandler::instance(), SIGNAL(scanEnded()), this, SLOT(unfreeze()));
    connect(m_abortButton, SIGNAL(clicked()), this, SLOT(cancelConnect()));

    //to ease translations
    m_messageTable.insert("interface_down", i18n("Putting interface down..."));
    m_messageTable.insert("resetting_ip_address", i18n("Resetting IP address..."));
    m_messageTable.insert("interface_up", i18n("Putting interface up..."));
    m_messageTable.insert("generating_psk", i18n("Generating PSK..."));
    m_messageTable.insert("bad_pass", i18n("Connection Failed: Bad password."));
    m_messageTable.insert("generating_wpa_config", i18n("Generating WPA configuration"));
    m_messageTable.insert("validating_authentication", i18n("Validating authentication..."));
    m_messageTable.insert("running_dhcp", i18n("Obtaining IP address..."));
    m_messageTable.insert("done", i18n("Done connecting..."));
    m_messageTable.insert("dhcp_failed", i18n("Connection Failed: Unable to Get IP Address"));
    m_messageTable.insert("no_dhcp_offers", i18n("Connection Failed: No DHCP offers received."));
    m_messageTable.insert("verifying_association", i18n("Verifying access point association..."));
    m_messageTable.insert("association_failed", i18n("Connection failed: Could not contact the wireless access point."));
    m_messageTable.insert("setting_static_ip", i18n("Setting static IP addresses..."));
    m_messageTable.insert("aborted", i18n("Aborted"));
    m_messageTable.insert("Failed", i18n("Failed"));

    //force first status update
    m_status.State = 10;
    updateStatus(DBusHandler::instance()->status());
}

MainWindow::~MainWindow()
{
    delete m_trayicon;
    delete m_networkPanel;
    delete m_abortButton;
    DBusHandler::destroy();
}

void MainWindow::activated()
{
    restoreWindowSize(autoSaveConfigGroup());
    m_networkPanel->loadList(DBusHandler::instance()->networksList());
}

void MainWindow::updateStatus(Status status)
{
    if (m_status.State != status.State) {
        if (status.State == WicdState::CONNECTING) {
            m_networkPanel->setBusy(true);
            m_abortButton->setVisible(true);
        } else {
            m_networkPanel->setBusy(false);
            m_abortButton->setVisible(false);
            m_networkPanel->loadList(DBusHandler::instance()->networksList());
        }
        m_status = status;
    }
    QString message;
    if (status.State == WicdState::WIRED) {
        m_trayicon->setIcon("network-wired");
        message = i18n("Connected to wired network (IP : %1)", status.Infos.at(0));//info(0) ip
    } else if (status.State == WicdState::WIRELESS) {
        int quality = status.Infos.at(2).toInt();//info(2) quality
        QString unit = "%";
        if (quality <= -10) {
            unit = " dBm";
            //we use dBm, so we need the "classic" quality to display an accurate icon
            quality = DBusHandler::instance()->callWireless("GetWirelessProperty",
                                                            status.Infos.at(3).toInt(), "quality").toInt();//info(3) networkId
        }
        setWirelessIcon(quality);
        message = i18n("Connected to %1 - %2%3 (IP : %4)",
                       status.Infos.at(1), status.Infos.at(2), unit, status.Infos.at(0));//info(1) essid
    } else if (status.State == WicdState::CONNECTING) {
        bool wired = (status.Infos.at(0)=="wired");
        if (wired) {
            message = DBusHandler::instance()->callWired("CheckWiredConnectingMessage").toString();
        } else {
            message = DBusHandler::instance()->callWireless("CheckWirelessConnectingMessage").toString();
        }
        message = m_messageTable.value(message);
        wired ? message.prepend(i18n("Wired network")+": ") : message.prepend(status.Infos.at(1)+": ");
        QTimer::singleShot(500, this, SLOT(forceUpdateStatus()));
    } else {
        m_trayicon->setIcon("network-disconnect");
        message = i18n("Disconnected");
    }
    m_trayicon->setToolTipSubTitle(message);
    statusBar()->showMessage(message);
}

void MainWindow::setWirelessIcon(int quality)
{
    if (quality <= 25) {
        m_trayicon->setIcon("network-wireless-connected-25");
    } else if (quality <= 50) {
        m_trayicon->setIcon("network-wireless-connected-50");
    } else if (quality <= 75) {
        m_trayicon->setIcon("network-wireless-connected-75");
    } else {
        m_trayicon->setIcon("network-wireless-connected-100");
    }
}

void MainWindow::handleConnectionResult(const QString &result)
{
    QStringList validMessages;
    validMessages << "Success" << "aborted";
    if ((!validMessages.contains(result)) && this->isVisible()) {
        if (m_messageTable.contains(result))
            KMessageBox::error(this, m_messageTable.value(result));
        else
            KMessageBox::error(this, result);
    }
}

void MainWindow::forceUpdateStatus()
{
    updateStatus(DBusHandler::instance()->status());
}

void MainWindow::freeze()
{
    m_networkPanel->setBusy(true);
}

void MainWindow::unfreeze()
{
    m_networkPanel->setBusy(false);
    m_networkPanel->loadList(DBusHandler::instance()->networksList());
}

void MainWindow::cancelConnect() const
{
    DBusHandler::instance()->callDaemon("CancelConnect");
    DBusHandler::instance()->callDaemon("SetForcedDisconnect", true);
}

void MainWindow::showPreferences()
{
    KDialog dialog(this);
    dialog.setCaption(i18n("Preferences"));
    dialog.setModal(true);
    PreferencesPanel* preferencesPanel = new PreferencesPanel();
    dialog.setMainWidget(preferencesPanel);
    int accepted = dialog.exec();
    if (accepted) {
        preferencesPanel->save();
    }
}

void MainWindow::createAdhocDialog()
{
    KDialog dialog(this);
    dialog.setCaption(i18n("Create an ad-hoc network"));
    dialog.setModal(true);

    QWidget *widget = new QWidget(&dialog);
    QVBoxLayout *vboxlayout = new QVBoxLayout();

    LabelEntry *essidEdit = new LabelEntry("ESSID");
    essidEdit->setText("My_Adhoc_Network");
    LabelEntry *ipEdit = new LabelEntry("IP");
    ipEdit->setText("169.254.12.10");
    LabelEntry *channelEdit = new LabelEntry(i18n("Channel"));
    channelEdit->setText("3");
    QCheckBox *icsBox = new QCheckBox(i18n("Activate Internet Connection Sharing"));//useless?
    icsBox->setEnabled(false);
    QCheckBox *wepBox = new QCheckBox(i18n("Use Encryption (WEP only)"));
    LabelEntry *keyEdit = new LabelEntry(i18n("Key"));
    keyEdit->setEnabled(false);
    connect(wepBox, SIGNAL(toggled(bool)), keyEdit, SLOT(setEnabled(bool)));

    vboxlayout->addWidget(essidEdit);
    vboxlayout->addWidget(ipEdit);
    vboxlayout->addWidget(channelEdit);
    vboxlayout->addWidget(icsBox);
    vboxlayout->addWidget(wepBox);
    vboxlayout->addWidget(keyEdit);
    vboxlayout->addStretch();

    widget->setLayout(vboxlayout);
    dialog.setMainWidget( widget );

    int accepted = dialog.exec();
    if (accepted) {
        DBusHandler::instance()->callWireless("CreateAdHocNetwork",
                                              essidEdit->text(),
                                              channelEdit->text(),
                                              ipEdit->text(),
                                              "WEP",
                                              keyEdit->text(),
                                              wepBox->isChecked(),
                                              false /*icsBox->isChecked()*/);
    }
}

void MainWindow::findHiddenDialog()
{
    bool ok;
    QString text = QInputDialog::getText(this, i18n("Find a hidden network"),
                                         i18n("Hidden Network ESSID"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        DBusHandler::instance()->callWireless("SetHiddenNetworkESSID", text);
        DBusHandler::instance()->scan();
    }
}

