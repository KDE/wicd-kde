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

#include "preferencespanel.h"
#include "dbushandler.h"
#include "global.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <KLocalizedString>

PreferencesPanel::PreferencesPanel( QWidget *parent )
    : KTabWidget( parent )
{
    QWidget* generalPanel = new QWidget(this);
    QVBoxLayout* generalLayout = new QVBoxLayout();
    generalPanel->setLayout(generalLayout);

    QFont bold;
    bold.setWeight(QFont::Bold);

    //interfaces
    QLabel* networkinterfaceLabel = new QLabel(i18n("Network interfaces"));
    networkinterfaceLabel->setFont(bold);
    generalLayout->addWidget(networkinterfaceLabel);

    wirelessinterfaceEdit = new LabelEntry(i18n("Wireless interface"));
    wiredinterfaceEdit = new LabelEntry(i18n("Wired interface"));
    alwaysshowwiredBox = new QCheckBox(i18n("Always show wired interface"));
    alwaysswitchtowiredBox = new QCheckBox(i18n("Always switch to a wired connection when available"));
    generalLayout->addWidget(wirelessinterfaceEdit);
    generalLayout->addWidget(wiredinterfaceEdit);
    generalLayout->addWidget(alwaysshowwiredBox);
    generalLayout->addWidget(alwaysswitchtowiredBox);

    //global DNS
    CollapsibleWidget* dnsWidget = new CollapsibleWidget(i18n("Global DNS servers"), this);
    useglobaldnsBox = new QCheckBox(i18n("Use global DNS servers"));
    dnsdomainEdit = new LabelEntry(i18n("DNS domain"));
    searchdomainEdit = new LabelEntry(i18n("Search domain"));
    dns1Edit = new LabelEntry(i18n("DNS server 1"));
    dns2Edit = new LabelEntry(i18n("DNS server 2"));
    dns3Edit = new LabelEntry(i18n("DNS server 3"));
    dnsWidget->addWidget(useglobaldnsBox);
    dnsWidget->addWidget(dnsdomainEdit);
    dnsWidget->addWidget(searchdomainEdit);
    dnsWidget->addWidget(dns1Edit);
    dnsWidget->addWidget(dns2Edit);
    dnsWidget->addWidget(dns3Edit);
    generalLayout->addWidget(dnsWidget);

    //other
    QLabel* otherLabel = new QLabel(i18n("Other"));
    otherLabel->setFont(bold);
    generalLayout->addWidget(otherLabel);

    QLabel* wiredautoLabel = new QLabel(i18n("Wired automatic connection"));
    wiredautoCombo = new QComboBox();
    wiredautoCombo->addItem(i18n("Use default wired network profile"));
    wiredautoCombo->addItem(i18n("Prompt for wired network profile"));
    wiredautoCombo->addItem(i18n("Use last wired network profile"));
    QHBoxLayout* wiredautoLayout = new QHBoxLayout();
    wiredautoLayout->addWidget(wiredautoLabel);
    wiredautoLayout->addWidget(wiredautoCombo);
    generalLayout->addLayout(wiredautoLayout);

    autoreconBox = new QCheckBox(i18n("Automatically reconnect on network connection lost"));
    generalLayout->addWidget(autoreconBox);

    notificationsBox = new QCheckBox(i18n("Display notifications about connection status"));
    generalLayout->addWidget(notificationsBox);

    generalLayout->addStretch();

    QWidget* externalPanel = new QWidget(this);
    QFormLayout* externalLayout = new QFormLayout();
    externalLayout->setLabelAlignment(Qt::AlignLeft);
    externalPanel->setLayout(externalLayout);

    dhcpclientCombo = new QComboBox();
    dhcpclientCombo->addItem(i18n("Automatic (recommended)"));
    dhcplist << i18n("Automatic (recommended)") << "dhclient" << "dhcpcd" << "pump" << "udhcpc";
    foreach (const QString& app, dhcplist) {
        if (DBusHandler::instance()->callDaemon("GetAppAvailable", app).toBool())
            dhcpclientCombo->addItem(app);
    }
    externalLayout->addRow(i18n("DHCP client"), dhcpclientCombo);

    wiredlinkCombo = new QComboBox();
    wiredlinkCombo->addItem(i18n("Automatic (recommended)"));
    wiredlinklist << i18n("Automatic (recommended)") << "ethtool" << "mii-tool";
    foreach (const QString& app, wiredlinklist) {
        if (DBusHandler::instance()->callDaemon("GetAppAvailable", app).toBool())
            wiredlinkCombo->addItem(app);
    }
    externalLayout->addRow(i18n("Wired link detection"), wiredlinkCombo);

    routetableCombo = new QComboBox();
    routetableCombo->addItem(i18n("Automatic (recommended)"));
    routetablelist << i18n("Automatic (recommended)") << "ip" << "route";
    foreach (const QString& app, routetablelist) {
        if (DBusHandler::instance()->callDaemon("GetAppAvailable", app).toBool())
            routetableCombo->addItem(app);
    }
    externalLayout->addRow(i18n("Route table flushing"), routetableCombo);

    QWidget* advancedPanel = new QWidget(this);
    QVBoxLayout* advancedLayout = new QVBoxLayout();
    advancedPanel->setLayout(advancedLayout);

    //WPA Supplicant
    QLabel* wpasupLabel = new QLabel("WPA Supplicant");
    wpasupLabel->setFont(bold);
    advancedLayout->addWidget(wpasupLabel);
    QLabel* driverLabel = new QLabel(i18n("Driver"));
    driverCombo = new QComboBox();
    QStringList driverlist = DBusHandler::instance()->callWireless("GetWpaSupplicantDrivers").toStringList();
    driverlist.append("ralink_legacy");
    driverCombo->addItems(driverlist);
    QHBoxLayout* driverLayout = new QHBoxLayout();
    driverLayout->addWidget(driverLabel);
    driverLayout->addWidget(driverCombo);
    advancedLayout->addLayout(driverLayout);

    //Backend
    QLabel* backendLabel = new QLabel(i18n("Backend"));
    backendLabel->setFont(bold);
    advancedLayout->addWidget(backendLabel);
    QLabel* backLabel = new QLabel(i18n("Backend"));
    backendCombo = new QComboBox();
    QStringList backendlist = DBusHandler::instance()->callDaemon("GetBackendList").toStringList();
    backendCombo->addItems(backendlist);
    QHBoxLayout* backendLayout = new QHBoxLayout();
    backendLayout->addWidget(backLabel);
    backendLayout->addWidget(backendCombo);
    advancedLayout->addLayout(backendLayout);

    //debugging
    QLabel* debuggingLabel = new QLabel(i18n("Debugging"));
    debuggingLabel->setFont(bold);
    advancedLayout->addWidget(debuggingLabel);
    debuggingBox = new QCheckBox(i18n("Enable debug mode"));
    advancedLayout->addWidget(debuggingBox);

    //wireless interface
    QLabel* wifaceLabel = new QLabel(i18n("Wireless interface"));
    wifaceLabel->setFont(bold);
    advancedLayout->addWidget(wifaceLabel);
    usedbmBox = new QCheckBox(i18n("Use dBm to measure signal strength"));
    pingstaticBox = new QCheckBox(i18n("Ping static gateways after connecting to verify association"));
    advancedLayout->addWidget(usedbmBox);
    advancedLayout->addWidget(pingstaticBox);

    advancedLayout->addStretch();

    addTab( generalPanel, i18n("General settings") );
    addTab( externalPanel, i18n("External programs") );
    addTab( advancedPanel, i18n("Advanced settings") );

    load();

}

PreferencesPanel::~PreferencesPanel()
{
}

void PreferencesPanel::load()
{
    //general
    wirelessinterfaceEdit->setText(DBusHandler::instance()->callDaemon("GetWirelessInterface").toString());
    wiredinterfaceEdit->setText(DBusHandler::instance()->callDaemon("GetWiredInterface").toString());
    alwaysshowwiredBox->setChecked(DBusHandler::instance()->callDaemon("GetAlwaysShowWiredInterface").toBool());
    alwaysswitchtowiredBox->setChecked(DBusHandler::instance()->callDaemon("GetPreferWiredNetwork").toBool());

    useglobaldnsBox->setChecked(DBusHandler::instance()->callDaemon("GetUseGlobalDNS").toBool());
    QStringList dnslist = DBusHandler::instance()->callDaemon("GetGlobalDNSAddresses").toStringList();
    dns1Edit->setText(Tools::noneToBlank(dnslist.value(0)));
    dns2Edit->setText(Tools::noneToBlank(dnslist.value(1)));
    dns3Edit->setText(Tools::noneToBlank(dnslist.value(2)));
    dnsdomainEdit->setText(Tools::noneToBlank(dnslist.value(3)));
    searchdomainEdit->setText(Tools::noneToBlank(dnslist.value(4)));

    wiredautoCombo->setCurrentIndex(DBusHandler::instance()->callDaemon("GetWiredAutoConnectMethod").toInt()-1);
    autoreconBox->setChecked(DBusHandler::instance()->callDaemon("GetAutoReconnect").toBool());
    notificationsBox->setEnabled(false);//info in wpath

    //external
    QString appname = dhcplist.value(DBusHandler::instance()->callDaemon("GetDHCPClient").toInt());
    dhcpclientCombo->setCurrentIndex(dhcpclientCombo->findText(appname));

    appname = wiredlinklist.value(DBusHandler::instance()->callDaemon("GetLinkDetectionTool").toInt());
    wiredlinkCombo->setCurrentIndex(wiredlinkCombo->findText(appname));

    appname = routetablelist.value(DBusHandler::instance()->callDaemon("GetFlushTool").toInt());
    routetableCombo->setCurrentIndex(routetableCombo->findText(appname));

    //advanced
    appname = DBusHandler::instance()->callDaemon("GetWPADriver").toString();
    driverCombo->setCurrentIndex(driverCombo->findText(appname));

    appname = DBusHandler::instance()->callDaemon("GetSavedBackend").toString();
    backendCombo->setCurrentIndex(backendCombo->findText(appname));

    debuggingBox->setChecked(DBusHandler::instance()->callDaemon("GetDebugMode").toBool());

    usedbmBox->setChecked(DBusHandler::instance()->callDaemon("GetSignalDisplayType").toInt());
    pingstaticBox->setChecked(DBusHandler::instance()->callDaemon("GetShouldVerifyAp").toBool());

}

void PreferencesPanel::save() const
{
    //general
    DBusHandler::instance()->callDaemon("SetWirelessInterface", wirelessinterfaceEdit->text());
    DBusHandler::instance()->callDaemon("SetWiredInterface", wiredinterfaceEdit->text());
    DBusHandler::instance()->callDaemon("SetAlwaysShowWiredInterface", alwaysshowwiredBox->isChecked());
    DBusHandler::instance()->callDaemon("SetPreferWiredNetwork", alwaysswitchtowiredBox->isChecked());

    DBusHandler::instance()->callDaemon("SetUseGlobalDNS", useglobaldnsBox->isChecked());
    DBusHandler::instance()->callDaemon("SetGlobalDNS", dns1Edit->text(), dns2Edit->text(), dns3Edit->text(), dnsdomainEdit->text(), searchdomainEdit->text());

    DBusHandler::instance()->callDaemon("SetWiredAutoConnectMethod", wiredautoCombo->currentIndex()+1);
    DBusHandler::instance()->callDaemon("SetAutoReconnect", autoreconBox->isChecked());
    DBusHandler::instance()->callDaemon("SetAutoReconnect", autoreconBox->isChecked());
    //    +notifications... info in wpath

    //external
    int wicdindex = dhcplist.indexOf(dhcpclientCombo->currentText());
    DBusHandler::instance()->callDaemon("SetDHCPClient", wicdindex);

    wicdindex = wiredlinklist.indexOf(wiredlinkCombo->currentText());
    DBusHandler::instance()->callDaemon("SetLinkDetectionTool", wicdindex);

    wicdindex = routetablelist.indexOf(routetableCombo->currentText());
    DBusHandler::instance()->callDaemon("SetFlushTool", wicdindex);

    //advanced
    DBusHandler::instance()->callDaemon("SetWPADriver", driverCombo->currentText());

    DBusHandler::instance()->callDaemon("SetBackend", backendCombo->currentText());

    DBusHandler::instance()->callDaemon("SetDebugMode", debuggingBox->isChecked());

    DBusHandler::instance()->callDaemon("SetSignalDisplayType", int(usedbmBox->isChecked()));
    DBusHandler::instance()->callDaemon("SetShouldVerifyAp", pingstaticBox->isChecked());

}

CollapsibleWidget::CollapsibleWidget( const QString& title, QWidget *parent )
    : QWidget( parent )
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* titleLabel = new QLabel(title);
    m_collapseButton = new KPushButton();
    m_collapseButton->setIcon(KIcon("go-down"));
    m_collapseButton->setCheckable(true);
    QFont bold;
    bold.setWeight(QFont::Bold);
    titleLabel->setFont(bold);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_collapseButton);
    mainLayout->addLayout(headerLayout);

    QWidget* body = new QWidget(this);
    m_bodyLayout = new QVBoxLayout(body);
    m_bodyLayout->setContentsMargins(0, 0, 0, 0);
    body->setLayout(m_bodyLayout);
    mainLayout->addWidget(body);

    connect(m_collapseButton, SIGNAL(toggled(bool)), body, SLOT(setVisible(bool)));
    connect(m_collapseButton, SIGNAL(toggled(bool)), this, SLOT(expand(bool)));
    body->setVisible(false);
}

CollapsibleWidget::~CollapsibleWidget()
{
}

void CollapsibleWidget::addWidget(QWidget *widget)
{
    m_bodyLayout->addWidget(widget);
}

void CollapsibleWidget::addLayout(QLayout *layout)
{
    m_bodyLayout->addLayout(layout);
}

void CollapsibleWidget::expand(bool doexpand)
{
    m_collapseButton->setIcon(KIcon(doexpand?"go-up":"go-down"));
}
