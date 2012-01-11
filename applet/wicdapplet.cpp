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
#include "profilewidget.h"
#include "infodialog.h"
#include "adhocdialog.h"

#include <QToolButton>
#include <QTimer>

#include <KLocale>
#include <KNotification>
#include <KAction>
#include <KToolInvocation>
#include <KInputDialog>
#include <KConfigDialog>

#include <Plasma/Svg>
#include <Plasma/ScrollWidget>
#include <Plasma/Separator>
#include <Plasma/ServiceJob>
#include <Plasma/ToolTipContent>
#include <Plasma/ToolTipManager>
#include <Plasma/Theme>

WicdApplet::WicdApplet(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
      m_theme(0),
      m_plotter(0)
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
}


WicdApplet::~WicdApplet()
{
}

void WicdApplet::init()
{
    m_theme->resize(contentsRect().size());
    
    Plasma::ToolTipManager::self()->registerWidget(this);

    //load dataengine
    Plasma::DataEngine *engine = dataEngine("wicd");
    if (!engine->isValid()) {
        setFailedToLaunch(true, i18n("Unable to load the Wicd data engine."));
        return;
    }
    
    setupActions();
    
    //build the popup dialog
    QGraphicsWidget *appletDialog = new QGraphicsWidget(this);
    m_dialoglayout = new QGraphicsLinearLayout(Qt::Vertical);
    
    //Network list
    m_scrollWidget = new Plasma::ScrollWidget(appletDialog);
    m_scrollWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollWidget->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    m_scrollWidget->setMaximumHeight(400);

    m_networkView = new NetworkView(m_scrollWidget);
    m_scrollWidget->setWidget(m_networkView);

    m_busyWidget = new Plasma::BusyWidget(m_scrollWidget);
    m_busyWidget->hide();

    m_dialoglayout->addItem(m_scrollWidget);
    
    //Separator
    m_dialoglayout->addItem(new Plasma::Separator(appletDialog));
    
    //Bottom bar
    QGraphicsLinearLayout* bottombarLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    
    m_messageBox = new Plasma::Label(appletDialog);
    m_messageBox->setWordWrap(false);
    bottombarLayout->addItem(m_messageBox);
    
    bottombarLayout->addStretch();
    
    m_abortButton = new Plasma::ToolButton(appletDialog);
    m_abortButton->setIcon(KIcon("dialog-cancel"));
    m_abortButton->nativeWidget()->setToolTip(i18n("Abort"));
    connect(m_abortButton, SIGNAL(clicked()), this, SLOT(cancelConnect()));
    bottombarLayout->addItem(m_abortButton);
    
    Plasma::ToolButton *reloadButton = new Plasma::ToolButton(appletDialog);
    reloadButton->nativeWidget()->setToolTip(i18n("Reload"));
    reloadButton->setAction(action("reload"));
    bottombarLayout->addItem(reloadButton);
    
    m_dialoglayout->addItem(bottombarLayout);
    
    appletDialog->setLayout(m_dialoglayout);
    setGraphicsWidget(appletDialog);

    setHasConfigurationInterface(true);
    
    // read config
    configChanged();

    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), SLOT(updateColors()));


    //prevent notification on startup
    m_status.State = 10;
    m_isScanning = false;
    //connect dataengine
    m_wicdService = engine->serviceForSource("");
    engine->connectSource("status", this);
    engine->connectSource("daemon", this);
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
    connect(reloadAction, SIGNAL(triggered()), this, SLOT(scan()));

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

void WicdApplet::dataUpdated(const QString& source, const Plasma::DataEngine::Data &data)
{
    if (source == "status") {
        Status status;
        status.State = data["state"].toUInt();
        status.Info = data["info"].toStringList();
        m_interface = data["interface"].toString();
        if (m_plotter) {
            m_plotter->setInterface(m_interface);
        }
        if (m_status.State != status.State) {
            if (status.State == WicdState::CONNECTING) {
                setBusy(true);
                m_abortButton->setVisible(true);
            } else {
                setBusy(false);
                m_abortButton->setVisible(false);
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
        }
        m_status = status;
        QString message;
        if (status.State == WicdState::WIRED) {
            m_icon = "network-wired-activated";
            message = i18n("Connected to wired network (IP: %1)", status.Info.at(0));//info(0) ip
        } else if (status.State == WicdState::WIRELESS) {
            int quality = status.Info.at(2).toInt();//info(2) quality
            QString unit('%');
            if (quality <= -10) {
                unit = " dBm";
            }
            m_icon = qualityToIcon(quality);
            message = i18n("Connected to %1 - %2%3 (IP: %4)",
                           status.Info.at(1), status.Info.at(2), unit, status.Info.at(0));//info(1) essid
        } else if (status.State == WicdState::CONNECTING) {
            m_icon = "network-wired";
            bool wired = (status.Info.at(0)=="wired");
            message = data["message"].toString();
            wired ? message.prepend(i18n("Wired network: ")) : message.prepend(status.Info.at(1)+": ");
        } else {
            m_icon = "network-wired";
            message = i18n("Disconnected");
        }
        m_message = message;
        m_messageBox->setText(m_message);
        update();
    } else if (source == "daemon") {
        if (!data["running"].toBool()) {
            m_message = i18n("The Wicd daemon is not running.");
            m_messageBox->setText(m_message);
        }
        if (data["profileNeeded"].toBool()) {
            //QTimer::singleShot ensures the applet is done with init()
            QTimer::singleShot(0, this, SLOT(launchProfileManager()));
        }
        if (m_isScanning != data["scanning"].toBool()) {
            m_isScanning = data["scanning"].toBool();
            setBusy(m_isScanning);
        }
        if (!data["error"].toString().isEmpty()) {
            notify("error", data["error"].toString());
        }
    }
}

QString WicdApplet::qualityToIcon(int quality) const
{
    if (quality <= -10) {
        //we use dbm
        if (quality >= -60) {
            return "network-wireless-100";
        } else if (quality >= -70) {
            return "network-wireless-75";
        } else if (quality >= -80) {
            return "network-wireless-50";
        } else {
            return "network-wireless-25";
        }
    } else if (quality <= 25) {
        return "network-wireless-25";
    } else if (quality <= 50) {
        return "network-wireless-50";
    } else if (quality <= 75) {
        return "network-wireless-75";
    } else {
        return "network-wireless-100";
    }
}


void WicdApplet::paintInterface(QPainter *p,
                                const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    Q_UNUSED(option);
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    p->setRenderHint(QPainter::Antialiasing);

    m_theme->paint(p, contentsRect, m_icon);
}

void WicdApplet::launchProfileManager()
{
    ProfileDialog *profileDialog = new ProfileDialog(this);
    profileDialog->move(popupPosition(profileDialog->sizeHint()));
    profileDialog->animatedShow(locationToDirection(location()));
}

void WicdApplet::loadNetworks()
{
    m_networkView->loadNetworks();
    graphicsWidget()->adjustSize();
    //we need to defer the scrolling to let adjustSize() do its work to prevent a graphical glitch
    QTimer::singleShot(0, this, SLOT(autoScroll()));
}

void WicdApplet::autoScroll()
{
    m_scrollWidget->ensureItemVisible(m_networkView->currentNetworkItem());
}

void WicdApplet::showPlotter(bool show)
{
    //there's probably a better way to do this...
    if (show && !m_plotter) {
        m_dialoglayout->insertItem(1, new Plasma::Separator(this));
        m_plotter = new NetworkPlotter(this);
        m_plotter->setInterface(m_interface);
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

void WicdApplet::notify(const QString &event, const QString &message) const
{
    if (m_status.State != 10) { //don't notify on startup
        KNotification *notify = new KNotification(event);
        notify->setText(message);
        notify->setComponentData(KComponentData("wicd-kde"));
        notify->sendEvent();
    }
}

void WicdApplet::setBusy(bool busy)
{
    if (busy) {
        m_scrollWidget->widget()->hide();
        m_busyWidget->resize(m_scrollWidget->viewportGeometry().size());
        m_busyWidget->show();
    } else {
        loadNetworks();
        m_busyWidget->hide();
        m_scrollWidget->widget()->show();
    }
    Plasma::PopupApplet::setBusy(busy);
}

void WicdApplet::cancelConnect() const
{
    KConfigGroup op = m_wicdService->operationDescription("cancelConnect");
    m_wicdService->startOperationCall(op);
}

void WicdApplet::showPreferences() const
{
    KToolInvocation::startServiceByDesktopName("kcm_wicd");
}

void WicdApplet::createAdhocDialog() const
{
    QPointer<AdhocDialog> dialog = new AdhocDialog();
    if (dialog->exec() == QDialog::Accepted) {
        KConfigGroup op = m_wicdService->operationDescription("createAdHocNetwork");
        op.writeEntry("essid", dialog->essid());
        op.writeEntry("channel", dialog->channel());
        op.writeEntry("ip", dialog->ip());
        op.writeEntry("key", dialog->key());
        op.writeEntry("wep", dialog->wep());
        m_wicdService->startOperationCall(op);
    }
    delete dialog;
}

void WicdApplet::findHiddenDialog() const
{
    bool ok;
    QString text = KInputDialog::getText(i18n("Find a hidden network"),
                                         i18n("Hidden Network ESSID"), QString(), &ok, 0);
    if (ok && !text.isEmpty()) {
        KConfigGroup op = m_wicdService->operationDescription("findHiddenNetwork");
        op.writeEntry("essid", text);
        m_wicdService->startOperationCall(op);
    }
}

void WicdApplet::scan() const
{
    KConfigGroup op = m_wicdService->operationDescription("scan");
    m_wicdService->startOperationCall(op);
}

void WicdApplet::connectionInfoRequested()
{
    InfoDialog *infoDialog = new InfoDialog(m_status);
    infoDialog->move(popupPosition(infoDialog->sizeHint(), Qt::AlignRight));
    infoDialog->animatedShow(locationToDirection(location()));
}

void WicdApplet::configChanged()
{
    KConfigGroup cg = config();
    m_showSignalStrength = cg.readEntry("Show signal strength", false);
    m_networkView->showSignalStrength(m_showSignalStrength);
    m_autoScan = cg.readEntry("Autoscan", false);
    m_showPlotter = cg.readEntry("Show plotter", false);
    if (!hasFailedToLaunch())
        showPlotter(m_showPlotter);
}

void WicdApplet::toolTipAboutToShow()
{
    Plasma::ToolTipContent c(name(), m_message, KIcon(icon()));
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
        scan();
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

void WicdApplet::updateColors()
{
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
    setPalette(pal);
    //Plasma::Label doesn't use the plasma theme color scheme by default, so we force it.
    m_messageBox->setPalette(pal);
}

#include "wicdapplet.moc"
