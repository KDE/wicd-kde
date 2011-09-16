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

#include "wicdapplet.h"
#include "global.h"
#include "labelentry.h"
#include "profilemanager.h"

#include <QPainter>
#include <QToolButton>

#include <KLocale>
#include <KNotification>
#include <KAction>
#include <KToolInvocation>
#include <KInputDialog>
#include <KConfigDialog>

#include <Plasma/Svg>
#include <Plasma/ScrollWidget>
#include <Plasma/Separator>
#include <Plasma/ToolTipContent>
#include <Plasma/ToolTipManager>

WicdApplet::WicdApplet(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
      m_theme(0),
      m_plotter(0),
      m_infosDialog(0)
{
    KGlobal::locale()->insertCatalog("wicd-kde");
    
    Wicd::locate();
    setHasConfigurationInterface(false);
    setAspectRatioMode(Plasma::ConstrainedSquare);
    setPopupIcon(QIcon());
    setStatus(Plasma::ActiveStatus);
    m_theme = new Plasma::Svg(this);
    m_theme->setImagePath("icons/network");
    m_theme->setContainsMultipleImages(true);
    
    // this will get us the standard applet background, for free!
    setBackgroundHints(DefaultBackground);
    
    connect(DBusHandler::instance(), SIGNAL(statusChange(Status)), this, SLOT(updateStatus(Status)));
    connect(DBusHandler::instance(), SIGNAL(connectionResultSend(QString)), this, SLOT(handleConnectionResult(QString)));
    connect(DBusHandler::instance(), SIGNAL(launchChooser()), this, SLOT(launchProfileManager()));
    connect(DBusHandler::instance(), SIGNAL(scanStarted()), this, SLOT(freeze()));
    connect(DBusHandler::instance(), SIGNAL(scanEnded()), this, SLOT(unfreeze()));
    
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
}


WicdApplet::~WicdApplet()
{
    if (m_infosDialog)
        m_infosDialog->deleteLater();
    //     if (hasFailedToLaunch()) {
    //         // Do some cleanup here
    //     } else {
    //         // Save settings
    //     }
}

void WicdApplet::init()
{    
    m_theme->resize(contentsRect().size());
    
    Plasma::ToolTipManager::self()->registerWidget(this);
    
    setupActions();
    
    //build the popup dialog
    QGraphicsWidget *appletDialog = new QGraphicsWidget(this);
    m_dialoglayout = new QGraphicsLinearLayout(Qt::Vertical);
    
    //Network list
    m_scrollWidget = new Plasma::ScrollWidget(this);
    m_scrollWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollWidget->setMaximumHeight(400);

    m_networkView = new NetworkView(m_scrollWidget);
    m_scrollWidget->setWidget(m_networkView);

    m_busyWidget = new Plasma::BusyWidget(m_scrollWidget);
    m_busyWidget->hide();

    m_dialoglayout->addItem(m_scrollWidget);
    
    //Separator
    m_dialoglayout->addItem(new Plasma::Separator(this));
    
    //Bottom bar
    QGraphicsLinearLayout* bottombarLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    
    m_messageBox = new Plasma::Label(this);
    m_messageBox->setWordWrap(false);
    bottombarLayout->addItem(m_messageBox);
    
    bottombarLayout->addStretch();
    
    m_abortButton = new Plasma::ToolButton(this);
    m_abortButton->setIcon(KIcon("dialog-cancel"));
    m_abortButton->nativeWidget()->setToolTip(i18n("Abort"));
    connect(m_abortButton, SIGNAL(clicked()), this, SLOT(cancelConnect()));
    bottombarLayout->addItem(m_abortButton);
    
    Plasma::ToolButton *reloadButton = new Plasma::ToolButton(this);
    reloadButton->nativeWidget()->setToolTip(i18n("Reload"));
    reloadButton->setAction(action("reload"));
    bottombarLayout->addItem(reloadButton);
    
    m_dialoglayout->addItem(bottombarLayout);
    
    appletDialog->setLayout(m_dialoglayout);
    setGraphicsWidget(appletDialog);

    setHasConfigurationInterface(true);
    
    // read config
    configChanged();
    
    //force first status update
    m_status.State = 10;
    updateStatus(DBusHandler::instance()->status());
}

void WicdApplet::setupActions()
{
    KAction* preferencesAction = new KAction(KIcon("preferences-system-network"), i18n("Wicd Configuration"), this);
    preferencesAction->setShortcuts(KShortcut("Ctrl+P"));
    addAction("configure_wicd", preferencesAction);
    connect(preferencesAction, SIGNAL(triggered()), this, SLOT(showPreferences()));
    
    KAction* connectInfoAction = new KAction(KIcon("help-about"), i18n("Connection information"), this);
    addAction("connection_info", connectInfoAction);
    connect(connectInfoAction, SIGNAL(triggered()), this, SLOT(connectionInfoRequested()));

    KAction* createadhocAction = new KAction(KIcon("list-add"), i18n("Create an ad-hoc network"), this);
    createadhocAction->setShortcuts(KShortcut("Ctrl+A"));
    addAction("createadhoc", createadhocAction);
    connect(createadhocAction, SIGNAL(triggered()), this, SLOT(createAdhocDialog()));
    KAction* findnetworkAction = new KAction(KIcon("edit-find"), i18n("Find a hidden network"), this);
    findnetworkAction->setShortcuts(KShortcut("Ctrl+F"));
    addAction("findnetwork", findnetworkAction);
    connect(findnetworkAction, SIGNAL(triggered()), this, SLOT(findHiddenDialog()));
    
    KAction* reloadAction = new KAction(KIcon("view-refresh"), i18n("Reload"), this);
    reloadAction->setShortcuts(KShortcut("Ctrl+R"));
    addAction("reload", reloadAction);
    connect(reloadAction, SIGNAL(triggered()), DBusHandler::instance(), SLOT(scan()));

}


void WicdApplet::paintInterface(QPainter *p,
                                const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    Q_UNUSED(option);
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    p->setRenderHint(QPainter::Antialiasing);

    m_theme->paint(p, contentsRect, m_icon);
}

void WicdApplet::updateStatus(Status status)
{
    if (m_status.State != status.State) {
        if (status.State == WicdState::CONNECTING) {
            freeze();
            m_abortButton->setVisible(true);
        } else {
            unfreeze();
            m_abortButton->setVisible(false);
            if (m_plotter) {
                m_plotter->setInterface(DBusHandler::instance()->callDaemon("GetCurrentInterface").toString());
            }
            switch (status.State) {
            case WicdState::WIRED:
                notify("connected", i18n("Connected to wired network"));
                break;
            case WicdState::WIRELESS:
                notify("connected", i18n("Connected to wireless network"));
                break;
            case WicdState::NOT_CONNECTED:
                notify("disconnected", i18n("Disconnected"));
                break;
            default:
                break;
            }
        }
        m_status = status;
    }
    QString message;
    if (status.State == WicdState::WIRED) {
        m_icon = "network-wired-activated";
        message = i18n("Connected to wired network (IP: %1)", status.Infos.at(0));//info(0) ip
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
        message = i18n("Connected to %1 - %2%3 (IP: %4)",
                       status.Infos.at(1), status.Infos.at(2), unit, status.Infos.at(0));//info(1) essid
    } else if (status.State == WicdState::CONNECTING) {
        bool wired = (status.Infos.at(0)=="wired");
        if (wired) {
            message = DBusHandler::instance()->callWired("CheckWiredConnectingMessage").toString();
        } else {
            message = DBusHandler::instance()->callWireless("CheckWirelessConnectingMessage").toString();
        }
        message = m_messageTable.value(message);
        wired ? message.prepend(i18n("Wired network: ")) : message.prepend(status.Infos.at(1)+": ");
        QTimer::singleShot(500, this, SLOT(forceUpdateStatus()));
    } else {
        m_icon = "network-wired";
        message = i18n("Disconnected");
    }
    m_messageBox->setText(message);
    m_message = message;
    update();
}

void WicdApplet::setWirelessIcon(int quality)
{
    if (quality <= 25) {
        m_icon = "network-wireless-25";
    } else if (quality <= 50) {
        m_icon = "network-wireless-50";
    } else if (quality <= 75) {
        m_icon = "network-wireless-75";
    } else {
        m_icon = "network-wireless-100";
    }
}

void WicdApplet::handleConnectionResult(const QString &result)
{
    QStringList validMessages;
    validMessages << "Success" << "aborted";
    if (!validMessages.contains(result)) {
        notify("error", m_messageTable.value(result));
    }
}

void WicdApplet::launchProfileManager()
{
    DBusHandler::instance()->callDaemon("SetForcedDisconnect", true);
    ProfileManager manager;
    int accepted = manager.exec();
    if (accepted){
        DBusHandler::instance()->callWired("ReadWiredNetworkProfile", Wicd::currentprofile);
        DBusHandler::instance()->callWired("ConnectWired");
    }
    DBusHandler::instance()->callDaemon("SetNeedWiredProfileChooser", false);
}

void WicdApplet::loadNetworks()
{
    m_networkView->loadList(DBusHandler::instance()->networksList());
    graphicsWidget()->adjustSize();
}

void WicdApplet::showPlotter(bool show)
{
    //there's probably a better way to do this...
    if (show && !m_plotter) {
        m_dialoglayout->insertItem(1, new Plasma::Separator(this));
        m_plotter = new NetworkPlotter(this);
        m_plotter->setInterface(DBusHandler::instance()->callDaemon("GetCurrentInterface").toString());
        m_dialoglayout->insertItem(2, m_plotter);
    } else if (!show && m_plotter) {
        m_dialoglayout->removeAt(2);
        m_plotter->deleteLater();
        m_plotter = 0;
        Plasma::Separator *separator = dynamic_cast<Plasma::Separator *>(m_dialoglayout->itemAt(1));
        if (separator) {
            m_dialoglayout->removeItem(separator);
            delete separator;
        }
    }
    graphicsWidget()->adjustSize();
}

void WicdApplet::notify(const QString &event, const QString &message)
{
    if (m_status.State != 10) { //don't notify on startup
        KNotification *notify = new KNotification(event);
        notify->setText(message);
        notify->setComponentData(KComponentData("wicd-kde"));
        notify->sendEvent();
    }
}

void WicdApplet::forceUpdateStatus()
{
    updateStatus(DBusHandler::instance()->status());
}

void WicdApplet::freeze()
{
    m_scrollWidget->widget()->hide();
    m_busyWidget->resize(m_scrollWidget->viewportGeometry().size());
    m_busyWidget->show();
    setBusy(true);
}

void WicdApplet::unfreeze()
{
    m_busyWidget->hide();
    m_scrollWidget->widget()->show();
    setBusy(false);
    loadNetworks();
}

void WicdApplet::cancelConnect() const
{
    DBusHandler::instance()->callDaemon("CancelConnect");
    DBusHandler::instance()->callDaemon("SetForcedDisconnect", true);
}

void WicdApplet::showPreferences()
{
    KToolInvocation::startServiceByDesktopName("kcm_wicd");
}

void WicdApplet::createAdhocDialog()
{
    KDialog dialog;
    dialog.setCaption(i18n("Create an ad-hoc network"));
    dialog.setModal(true);

    QWidget *widget = new QWidget(&dialog);
    QVBoxLayout *vboxlayout = new QVBoxLayout();

    LabelEntry *essidEdit = new LabelEntry(i18n("ESSID:"));
    essidEdit->setText("My_Adhoc_Network");
    LabelEntry *ipEdit = new LabelEntry(i18n("IP:"));
    ipEdit->setText("169.254.12.10");
    LabelEntry *channelEdit = new LabelEntry(i18n("Channel:"));
    channelEdit->setText("3");
    QCheckBox *icsBox = new QCheckBox(i18n("Activate Internet Connection Sharing"));//useless?
    icsBox->setEnabled(false);
    QCheckBox *wepBox = new QCheckBox(i18n("Use Encryption (WEP only)"));
    LabelEntry *keyEdit = new LabelEntry(i18n("Key:"));
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

void WicdApplet::findHiddenDialog()
{
    bool ok;
    QString text = KInputDialog::getText(i18n("Find a hidden network"),
                                         i18n("Hidden Network ESSID"), QString(), &ok, 0);
    if (ok && !text.isEmpty()) {
        DBusHandler::instance()->callWireless("SetHiddenNetworkESSID", text);
        DBusHandler::instance()->scan();
    }
}

InfosDialog* WicdApplet::infosDialog()
{
    if (!m_infosDialog)
        m_infosDialog = new InfosDialog();
    return m_infosDialog;
}

void WicdApplet::connectionInfoRequested()
{
    infosDialog()->move(popupPosition(infosDialog()->sizeHint(), Qt::AlignRight));
    infosDialog()->animatedShow(locationToDirection(location()));
}

void WicdApplet::configChanged()
{
    KConfigGroup cg = config();
    m_showSignalStrength = cg.readEntry("Show signal strength", false);
    m_networkView->showSignalStrength(m_showSignalStrength);
    m_autoScan = cg.readEntry("Autoscan", false);
    m_showPlotter = cg.readEntry("Show plotter", false);
    showPlotter(m_showPlotter);
}

void WicdApplet::toolTipAboutToShow()
{
    Plasma::ToolTipContent c("Wicd", m_message, KIcon(m_icon));
    Plasma::ToolTipManager::self()->setContent(this, c);
}

void WicdApplet::toolTipHidden()
{
    Plasma::ToolTipManager::self()->clearContent(this);
}

void WicdApplet::popupEvent(bool show)
{
    if (!show)
        return;
    if (m_autoScan) {
        DBusHandler::instance()->scan();
    } else {
        loadNetworks();
    }
}

void WicdApplet::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *widget = new QWidget(parent);
    ui.setupUi(widget);
    parent->addPage(widget, i18n("General"), Applet::icon());
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    ui.displayqualityBox->setChecked(m_showSignalStrength ? Qt::Checked : Qt::Unchecked);
    ui.autoscanBox->setChecked(m_autoScan ? Qt::Checked : Qt::Unchecked);
    ui.plotterBox->setChecked(m_showPlotter ? Qt::Checked : Qt::Unchecked);

    connect(ui.displayqualityBox, SIGNAL(stateChanged(int)), parent, SLOT(settingsModified()));
    connect(ui.autoscanBox, SIGNAL(stateChanged(int)), parent, SLOT(settingsModified()));
    connect(ui.plotterBox, SIGNAL(stateChanged(int)), parent, SLOT(settingsModified()));
}

void WicdApplet::configAccepted()
{
    KConfigGroup cg = config();

    if (m_showSignalStrength != ui.displayqualityBox->isChecked()) {
        cg.writeEntry("Show signal strength", !m_showSignalStrength);
    }
    if (m_autoScan != ui.autoscanBox->isChecked()) {
        cg.writeEntry("Autoscan", !m_autoScan);
    }
    if (m_showPlotter != ui.plotterBox->isChecked()) {
        cg.writeEntry("Show plotter", !m_showPlotter);
    }

    emit configNeedsSaving();
}

QList<QAction*> WicdApplet::contextualActions()
{
    QList<QAction *> rv;
    QAction *actinfo = action("connection_info");
    QAction *actcrea = action("createadhoc");
    QAction *actfind = action("findnetwork");
    QAction *actconf = action("configure_wicd");
    rv << actinfo << actcrea << actfind << actconf;
    return rv;
}

#include "wicdapplet.moc"
