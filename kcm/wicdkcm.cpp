/****************************************************************************
 *  Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>                    *
 *  Copyright (c) 2010 Anthony Vital <anthony.vital@gmail.com>              *
 *                                                                          *
 *  This file is part of the KDE project.                                   *
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
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.    *
 ****************************************************************************/

#include "wicdkcm.h"

#include "ui_kcmwidget.h"
#include "wicddbusinterface.h"

#include <kpluginfactory.h>
#include <kaboutdata.h>
#include <kdeversion.h>

//solid specific includes
#include <solid/devicenotifier.h>
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/networkinterface.h>

K_PLUGIN_FACTORY(WicdKCMModuleFactory, registerPlugin<WicdKCM>();)
        K_EXPORT_PLUGIN(WicdKCMModuleFactory("kcmwicd"))

        WicdKCM::WicdKCM(QWidget* parent, const QVariantList& )
            : KCModule(WicdKCMModuleFactory::componentData(), parent)
{
    KGlobal::locale()->insertCatalog("wicd-client-kde");

    KAboutData *about = new KAboutData(I18N_NOOP("kcmwicd"), 0, ki18n("Wicd Configuration"),
                                       0, KLocalizedString(), KAboutData::License_GPL,
                                       ki18n("(c) 2009 Dario Freddi"));

    about->addAuthor(ki18n("Dario Freddi"), ki18n("Original author"), "drf@kde.org", "http://drfav.wordpress.com");
    about->addAuthor(ki18n("Anthony Vital"), ki18n("Developer"), "anthony.vital@gmail.com");
    setAboutData( about );

    setButtons(Apply);

    m_ui = new Ui::WicdKcmWidget;
    m_ui->setupUi(this);

    init();
}


WicdKCM::~WicdKCM()
{

}

void WicdKCM::init()
{
    // Set up Wicd misc values
    m_wicdMisc["dhclient"] = 1;
    m_wicdMisc["dhcpcd"] = 2;
    m_wicdMisc["pump"] = 3;
    m_wicdMisc["udhcpc"] = 4;
    m_wicdMisc["ethtool"] = 1;
    m_wicdMisc["mii-tool"] = 2;
    m_wicdMisc["ip"] = 1;
    m_wicdMisc["route"] = 2;
    //    m_wicdMisc["gksudo"] = 1;
    //    m_wicdMisc["kdesu"] = 2;
    //    m_wicdMisc["ktsuss"] = 3;
    m_wicdMisc[i18n("Automatic (recommended)")] = 0;

    // Feature++: show the interfaces in a combobox thanks to solid
    QList<Solid::Device> list = Solid::Device::listFromType(Solid::DeviceInterface::NetworkInterface, QString());
    foreach (Solid::Device device, list)
    {
        Solid::NetworkInterface *netiface = device.as<Solid::NetworkInterface>();
        if (netiface->isWireless()) {
            m_ui->wirelessBox->addItem(netiface->ifaceName());
        } else {
            m_ui->wiredBox->addItem(netiface->ifaceName());
        }
    }

    // Load wpa supplicant drivers
    QDBusMessage msg = WicdDbusInterface::instance()->wireless().call("GetWpaSupplicantDrivers");
    QStringList wpaDrivers = msg.arguments().at(0).toStringList();
    wpaDrivers << "ralink_legacy";
    m_ui->wpaSupplicantBox->addItems(wpaDrivers);

    // Load Wicd backends
    msg = WicdDbusInterface::instance()->daemon().call("GetBackendList");
    m_ui->backendBox->addItems(msg.arguments().at(0).toStringList());

    // Now setup external applications
    QStringList tmpVal;

    // DHCP
    m_ui->dhcpBox->addItem(i18n("Automatic (recommended)"));
    tmpVal << "dhcpcd" << "pump" << "dhclient" << "udhcpc";
    foreach (const QString &str, tmpVal) {
        if (WicdDbusInterface::instance()->daemon().call("GetAppAvailable", str).arguments().at(0).toBool()) {
            m_ui->dhcpBox->addItem(str);
        }
    }

    tmpVal.clear();

    // Wired Link Detection
    m_ui->wiredLinkBox->addItem(i18n("Automatic (recommended)"));
    tmpVal << "ethtool" << "mii-tool";
    foreach (const QString &str, tmpVal) {
        if (WicdDbusInterface::instance()->daemon().call("GetAppAvailable", str).arguments().at(0).toBool()) {
            m_ui->wiredLinkBox->addItem(str);
        }
    }

    tmpVal.clear();

    // Route Table
    m_ui->routeBox->addItem(i18n("Automatic (recommended)"));
    tmpVal << "ip" << "route";
    foreach (const QString &str, tmpVal) {
        if (WicdDbusInterface::instance()->daemon().call("GetAppAvailable", str).arguments().at(0).toBool()) {
            m_ui->routeBox->addItem(str);
        }
    }

    tmpVal.clear();

    //    // Graphical sudo
    //    m_ui->sudoBox->addItem(i18n("Automatic (recommended)"));
    //    tmpVal << "gksudo" << "kdesu" << "ktsuss";
    //    foreach (const QString &str, tmpVal) {
    //        if (WicdDbusInterface::instance()->daemon().call("GetAppAvailable", str).arguments().at(0).toBool()) {
    //            m_ui->sudoBox->addItem(str);
    //        }
    //    }

    //    tmpVal.clear();

    // Now for the wired automatic connection
    m_ui->automaticWiredBox->addItem(i18n("Use default wired network profile"), 1);
    m_ui->automaticWiredBox->addItem(i18n("Prompt for wired network profile"), 2);
    m_ui->automaticWiredBox->addItem(i18n("Use last wired network profile"), 3);

    connect(m_ui->showWiredBox, SIGNAL(stateChanged(int)), this, SLOT(changed()));
    connect(m_ui->reconnectBox, SIGNAL(stateChanged(int)), this, SLOT(changed()));
    connect(m_ui->debugBox, SIGNAL(stateChanged(int)), this, SLOT(changed()));
    connect(m_ui->dbmBox, SIGNAL(stateChanged(int)), this, SLOT(changed()));
    connect(m_ui->swithcWiredBox, SIGNAL(stateChanged(int)), this, SLOT(changed()));

    connect(m_ui->dhcpBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
    connect(m_ui->wiredLinkBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
    connect(m_ui->routeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
    //    connect(m_ui->sudoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));

    connect(m_ui->automaticWiredBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));

    connect(m_ui->wiredBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
    connect(m_ui->wirelessBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
    connect(m_ui->wpaSupplicantBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));

    connect(m_ui->useDNSBox, SIGNAL(stateChanged(int)), this, SLOT(changed()));

    connect(m_ui->server1Edit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->server2Edit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->server3Edit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->domainEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->searchDomainEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));

    connect(m_ui->backendBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
}

void WicdKCM::load()
{
    m_ui->showWiredBox->setChecked(WicdDbusInterface::instance()->daemon().call("GetAlwaysShowWiredInterface").arguments().at(0).toBool());
    m_ui->reconnectBox->setChecked(WicdDbusInterface::instance()->daemon().call("GetAutoReconnect").arguments().at(0).toBool());
    m_ui->debugBox->setChecked(WicdDbusInterface::instance()->daemon().call("GetDebugMode").arguments().at(0).toBool());
    m_ui->dbmBox->setChecked(WicdDbusInterface::instance()->daemon().call("GetSignalDisplayType").arguments().at(0).toBool());
    m_ui->swithcWiredBox->setChecked(WicdDbusInterface::instance()->daemon().call("GetPreferWiredNetwork").arguments().at(0).toBool());

    foreach (const QString &key, m_wicdMisc.keys(WicdDbusInterface::instance()->daemon().call("GetDHCPClient").arguments().at(0).toInt())) {
        if (m_ui->dhcpBox->contains(key)) {
            m_ui->dhcpBox->setCurrentItem(key);
        }
    }
    foreach (const QString &key, m_wicdMisc.keys(WicdDbusInterface::instance()->daemon().call("GetLinkDetectionTool").arguments().at(0).toInt())) {
        if (m_ui->wiredLinkBox->contains(key)) {
            m_ui->wiredLinkBox->setCurrentItem(key);
        }
    }
    foreach (const QString &key, m_wicdMisc.keys(WicdDbusInterface::instance()->daemon().call("GetFlushTool").arguments().at(0).toInt())) {
        if (m_ui->routeBox->contains(key)) {
            m_ui->routeBox->setCurrentItem(key);
        }
    }
    //    foreach (const QString &key, m_wicdMisc.keys(WicdDbusInterface::instance()->daemon().call("GetSudoApp").arguments().at(0).toInt())) {
    //        if (m_ui->sudoBox->contains(key)) {
    //            m_ui->sudoBox->setCurrentItem(key);
    //        }
    //    }

    m_ui->automaticWiredBox->setCurrentIndex(WicdDbusInterface::instance()->daemon().call("GetWiredAutoConnectMethod").arguments().at(0).toInt() - 1);

    m_ui->wiredBox->setCurrentItem(WicdDbusInterface::instance()->daemon().call("GetWiredInterface").arguments().at(0).toString());
    m_ui->wirelessBox->setCurrentItem(WicdDbusInterface::instance()->daemon().call("GetWirelessInterface").arguments().at(0).toString());
    m_ui->wpaSupplicantBox->setCurrentItem(WicdDbusInterface::instance()->daemon().call("GetWPADriver").arguments().at(0).toString());

    m_ui->useDNSBox->setChecked(WicdDbusInterface::instance()->daemon().call("GetUseGlobalDNS").arguments().at(0).toBool());

    // Lazy workaround
    if (!m_ui->useDNSBox->isChecked()) {
        m_ui->useDNSBox->setChecked(true);
        m_ui->useDNSBox->setChecked(false);
    }

    if (m_ui->useDNSBox->isChecked()) {
        m_ui->server1Edit->setText(WicdDbusInterface::instance()->daemon().call("GetGlobalDNSAddresses").arguments().at(0).toString());
        m_ui->server2Edit->setText(WicdDbusInterface::instance()->daemon().call("GetGlobalDNSAddresses").arguments().at(1).toString());
        m_ui->server3Edit->setText(WicdDbusInterface::instance()->daemon().call("GetGlobalDNSAddresses").arguments().at(2).toString());
        m_ui->domainEdit->setText(WicdDbusInterface::instance()->daemon().call("GetGlobalDNSAddresses").arguments().at(3).toString());
        m_ui->searchDomainEdit->setText(WicdDbusInterface::instance()->daemon().call("GetGlobalDNSAddresses").arguments().at(4).toString());
    }

    m_ui->backendBox->setCurrentItem(WicdDbusInterface::instance()->daemon().call("GetSavedBackend").arguments().at(0).toString());
}

void WicdKCM::save()
{
    WicdDbusInterface::instance()->daemon().call("SetAlwaysShowWiredInterface", m_ui->showWiredBox->isChecked());
    WicdDbusInterface::instance()->daemon().call("SetAutoReconnect", m_ui->reconnectBox->isChecked());
    WicdDbusInterface::instance()->daemon().call("SetDebugMode", m_ui->debugBox->isChecked());
    WicdDbusInterface::instance()->daemon().call("SetSignalDisplayType", m_ui->dbmBox->isChecked());
    WicdDbusInterface::instance()->daemon().call("SetPreferWiredNetwork", m_ui->swithcWiredBox->isChecked());

    WicdDbusInterface::instance()->daemon().call("SetDHCPClient", m_wicdMisc[m_ui->dhcpBox->currentText()]);
    WicdDbusInterface::instance()->daemon().call("SetLinkDetectionTool", m_wicdMisc[m_ui->wiredLinkBox->currentText()]);
    WicdDbusInterface::instance()->daemon().call("SetFlushTool", m_wicdMisc[m_ui->routeBox->currentText()]);
    //    WicdDbusInterface::instance()->daemon().call("SetSudoApp", m_wicdMisc[m_ui->sudoBox->currentText()]);

    WicdDbusInterface::instance()->daemon().call("SetWiredAutoConnectMethod", m_ui->automaticWiredBox->itemData(m_ui->automaticWiredBox->currentIndex()));

    WicdDbusInterface::instance()->daemon().call("SetWiredInterface", m_ui->wiredBox->currentText());
    WicdDbusInterface::instance()->daemon().call("SetWirelessInterface", m_ui->wirelessBox->currentText());
    WicdDbusInterface::instance()->daemon().call("SetWPADriver", m_ui->wpaSupplicantBox->currentText());

    WicdDbusInterface::instance()->daemon().call("SetUseGlobalDNS", m_ui->useDNSBox->isChecked());

    if (m_ui->useDNSBox->isChecked()) {
        WicdDbusInterface::instance()->daemon().call("SetGlobalDNS", m_ui->server1Edit->text(), m_ui->server2Edit->text(),
                                                     m_ui->server3Edit->text(), m_ui->domainEdit->text(),
                                                     m_ui->searchDomainEdit->text());
    }

    WicdDbusInterface::instance()->daemon().call("SetBackend", m_ui->backendBox->currentText());
}

void WicdKCM::defaults()
{
    KCModule::defaults();
}
