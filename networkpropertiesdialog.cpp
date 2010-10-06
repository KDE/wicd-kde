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

#include "networkpropertiesdialog.h"
#include "scriptsdialog.h"
#include "dbushandler.h"

#include <KMessageBox>
#include <KLocalizedString>

NetworkPropertiesDialog::NetworkPropertiesDialog(int networkId, QWidget *parent, Qt::WFlags flags)
    : KDialog(parent, flags)
    , m_networkId(networkId)
{
    setModal(true);
    setButtons( KDialog::User1 | KDialog::Cancel );
    button(KDialog::User1)->setIcon(KIcon("dialog-ok"));
    button(KDialog::User1)->setText(i18n("OK"));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *vboxlayout = new QVBoxLayout();

    m_staticIpBox = new QCheckBox(i18n("Use Static IPs"));
    m_ipEdit = new LabelEntry(i18n("IP :"));
    m_netmaskEdit = new LabelEntry(i18n("Netmask :"));
    m_gatewayEdit = new LabelEntry(i18n("Gateway :"));
    m_staticdnsBox = new QCheckBox(i18n("Use Static DNS"));
    m_globaldnsBox = new QCheckBox(i18n("Use global DNS servers"));
    m_dnsdomainEdit = new LabelEntry(i18n("DNS domain :"));
    m_searchdomainEdit = new LabelEntry(i18n("Search domain :"));
    m_dns1Edit = new LabelEntry(i18n("DNS server 1 :"));
    m_dns2Edit = new LabelEntry(i18n("DNS server 2 :"));
    m_dns3Edit = new LabelEntry(i18n("DNS server 3 :"));
    m_dhcphostnameBox = new QCheckBox(i18n("DHCP Hostname :"));
    m_dhcphostnameBox->setMinimumWidth(m_dns3Edit->labelMinimumWidth());
    m_dhcphostnameEdit = new QLineEdit();

    //optionnal: wireless
    m_autoconnectBox = new QCheckBox(i18n("Automatically connect to this network"));
    m_globalSettingsBox = new QCheckBox(i18n("Use these settings for all networks sharing this essid"));
    m_useEncryptionBox = new QCheckBox(i18n("Use Encryption"));
    m_encryptionCombo = new QComboBox();
    m_encryptlayout = new QVBoxLayout();

    m_scriptsButton = new KPushButton(KIcon("system-run"), i18n("Scripts"), this);
    m_scriptsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    vboxlayout->addWidget(m_staticIpBox);
    vboxlayout->addWidget(m_ipEdit);
    vboxlayout->addWidget(m_netmaskEdit);
    vboxlayout->addWidget(m_gatewayEdit);

    QHBoxLayout *dnslayout = new QHBoxLayout();
    dnslayout->addWidget(m_staticdnsBox);
    dnslayout->addWidget(m_globaldnsBox);
    dnslayout->addStretch();

    vboxlayout->addLayout(dnslayout);
    vboxlayout->addWidget(m_dnsdomainEdit);
    vboxlayout->addWidget(m_searchdomainEdit);
    vboxlayout->addWidget(m_dns1Edit);
    vboxlayout->addWidget(m_dns2Edit);
    vboxlayout->addWidget(m_dns3Edit);

    QHBoxLayout *dhcplayout = new QHBoxLayout();
    dhcplayout->addWidget(m_dhcphostnameBox);
    dhcplayout->addWidget(m_dhcphostnameEdit);

    vboxlayout->addLayout(dhcplayout);

    vboxlayout->addWidget(m_scriptsButton);
    vboxlayout->addStretch();

    widget->setLayout(vboxlayout);
    setMainWidget( widget );

    connect(m_staticIpBox, SIGNAL(toggled(bool)), this, SLOT(toggleIpCheckbox(bool)));
    connect(m_staticdnsBox, SIGNAL(toggled(bool)), this, SLOT(toggleStaticDnsCheckbox(bool)));
    connect(m_globaldnsBox, SIGNAL(toggled(bool)), this, SLOT(toggleGlobalDnsCheckbox(bool)));
    connect(m_useEncryptionBox, SIGNAL(toggled(bool)), this, SLOT(toggleUseEncryptionBox(bool)));
    connect(m_encryptionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(encryptMethodChanged()));
    connect(m_scriptsButton, SIGNAL(clicked()), this, SLOT(editScripts()));
    connect(this, SIGNAL(user1Clicked()), this, SLOT(validate()));

    m_staticIpBox->setChecked(false);
    toggleIpCheckbox(false);
    m_staticdnsBox->setChecked(false);
    toggleStaticDnsCheckbox(false);

    if (m_networkId != -1) {
        m_autoconnectBox->setChecked(networkProperty(m_networkId, "automatic").toBool());
        //insert autoconnect checkbox on top
        vboxlayout->insertWidget(0, m_autoconnectBox);

        m_encryptions = Wicd::encryptionlist;
        //polulate the combobox
        int typeIndex = -1;
        for (int i = 0; i < m_encryptions.length(); ++i) {
            m_encryptionCombo->addItem(m_encryptions.value(i).value("name").toString());
            //look for the current encryption type
            if (networkProperty(m_networkId, "enctype").toString() == m_encryptions.value(i).value("type").toString()) {
                typeIndex = i;
            }
        }
        m_encryptionCombo->setCurrentIndex(typeIndex);

        //insert items before script button and spacer
        vboxlayout->insertWidget(vboxlayout->count()-2, m_globalSettingsBox);
        vboxlayout->insertWidget(vboxlayout->count()-2, m_useEncryptionBox);
        vboxlayout->insertWidget(vboxlayout->count()-2, m_encryptionCombo);
        vboxlayout->insertLayout(vboxlayout->count()-2, m_encryptlayout);
        encryptMethodChanged();

        if (typeIndex != -1) {
            m_useEncryptionBox->setChecked(true);
            toggleUseEncryptionBox(true);
        } else {
            toggleUseEncryptionBox(false);
        }
    }

    load();
}

NetworkPropertiesDialog::~NetworkPropertiesDialog()
{
}

void NetworkPropertiesDialog::toggleIpCheckbox(bool toggled)
{
    m_staticdnsBox->setEnabled(!toggled);
    if (toggled) m_staticdnsBox->setChecked(true);

    m_ipEdit->setEnabled(toggled);
    m_netmaskEdit->setEnabled(toggled);
    m_gatewayEdit->setEnabled(toggled);
}

void NetworkPropertiesDialog::toggleStaticDnsCheckbox(bool toggled)
{
    m_globaldnsBox->setEnabled(toggled);
    if (toggled) {
        m_dnsdomainEdit->setEnabled(!m_globaldnsBox->isChecked());
        m_searchdomainEdit->setEnabled(!m_globaldnsBox->isChecked());
        m_dns1Edit->setEnabled(!m_globaldnsBox->isChecked());
        m_dns2Edit->setEnabled(!m_globaldnsBox->isChecked());
        m_dns3Edit->setEnabled(!m_globaldnsBox->isChecked());
    } else {
        m_dnsdomainEdit->setEnabled(false);
        m_searchdomainEdit->setEnabled(false);
        m_dns1Edit->setEnabled(false);
        m_dns2Edit->setEnabled(false);
        m_dns3Edit->setEnabled(false);
        m_globaldnsBox->setChecked(false);
    }
}

void NetworkPropertiesDialog::toggleGlobalDnsCheckbox(bool toggled)
{
    bool useGlobalDns = DBusHandler::instance()->callDaemon("GetUseGlobalDNS").toBool();
    if ((!useGlobalDns) && toggled) {
        KMessageBox::sorry(this, i18n("Global DNS has not been enabled in general preferences."));
        m_globaldnsBox->setChecked(false);
        return;
    }
    if (useGlobalDns && m_staticdnsBox->isChecked()) {
        m_dnsdomainEdit->setEnabled(!toggled);
        m_searchdomainEdit->setEnabled(!toggled);
        m_dns1Edit->setEnabled(!toggled);
        m_dns2Edit->setEnabled(!toggled);
        m_dns3Edit->setEnabled(!toggled);
    }
}

void NetworkPropertiesDialog::toggleUseEncryptionBox(bool toggled)
{
    m_encryptionCombo->setEnabled(toggled);
    foreach (LabelEntry* le, m_encryptLabelEntries) {
        le->setEnabled(toggled);
    }
}

void NetworkPropertiesDialog::validate()
{
    QList<LabelEntry*> reqList;
    QList<LabelEntry*> optList;
    if (m_staticIpBox->isChecked()) {
        reqList.append(m_ipEdit);
        reqList.append(m_netmaskEdit);
        reqList.append(m_gatewayEdit);
    }
    if (m_staticdnsBox->isChecked() && (!m_globaldnsBox->isChecked())) {
        if (!m_dns1Edit->text().isEmpty()) optList.append(m_dns1Edit);
        if (!m_dns2Edit->text().isEmpty()) optList.append(m_dns2Edit);
        if (!m_dns3Edit->text().isEmpty()) optList.append(m_dns3Edit);
    }
    //check required fields
    foreach (LabelEntry* item, reqList) {
        item->setText(item->text().simplified());
        if (!Tools::isValidIP(item->text())) {
            KMessageBox::sorry(this, QString(i18n("Invalid adress in %1", item->label())));
            return;
        }
    }
    //check optionnal fields
    foreach (LabelEntry* item, optList) {
        item->setText(item->text().simplified());
        if ( (!item->text().isEmpty()) && (!Tools::isValidIP(item->text())) ) {
            KMessageBox::sorry(this, QString(i18n("Invalid adress in %1", item->label())));
            return;
        }
    }
    //check wireless encryption fields
    if (m_networkId != -1) {
        if (m_useEncryptionBox->isChecked()) {
            Encryption currentEncryption = m_encryptions.value(m_encryptionCombo->currentIndex());
            //ensure all required fields are filled
            QList<QVariant> required = currentEncryption.values("require");
            foreach (QVariant field, required) {
                QString key = field.toStringList().value(0);
                if (m_encryptLabelEntries.value(key)->text().isEmpty()) {
                    KMessageBox::sorry(this, i18n("%1 is required.", field.toStringList().value(1).replace("_", " ").remove("*")));
                    return;
                }
            }
        } else if (DBusHandler::instance()->callWireless("GetWirelessProperty", m_networkId, "encryption").toBool()) {
            KMessageBox::sorry(this, i18n("This network needs an encryption."));
            return;
        }
    }
    //everything is ok
    save();
    accept();
}

void NetworkPropertiesDialog::encryptMethodChanged()
{
    //remove previous label entries
    QMap< QString, LabelEntry* >::iterator i = m_encryptLabelEntries.begin();
    while (i != m_encryptLabelEntries.end()) {
        m_encryptlayout->removeWidget(i.value());
        delete i.value();
        i = m_encryptLabelEntries.erase(i);
    }

    //new index
    int index = m_encryptionCombo->currentIndex();
    if (index == -1)
        m_encryptionCombo->setCurrentIndex(0);

    //look into "require" and "optional" types
    QStringList types;
    types << "require" << "optional";
    for (int i = 0; i < types.length(); ++i) {
        //inspect the fields of the current type
        QList<QVariant> fields = m_encryptions.value(index).values(types.value(i));
        foreach (QVariant field, fields) {
            //a field is a pair of strings
            QString key = field.toStringList().value(0);
            QString value = field.toStringList().value(1);
            //new field implies new label entry
            LabelEntry* entry = new LabelEntry(value.replace("_", " ").remove("*")+" :");
            entry->setEchoMode(QLineEdit::PasswordEchoOnEdit);
            entry->setText(DBusHandler::instance()->callWireless("GetWirelessProperty", m_networkId, key).toString());
            m_encryptlayout->addWidget(entry);
            //keep trace of this LabelEntry
            m_encryptLabelEntries.insert(key, entry);
        }
    }
}

void NetworkPropertiesDialog::editScripts()
{
    ScriptsDialog dialog(m_networkId, this);
    dialog.exec();
}

void NetworkPropertiesDialog::load()
{
    m_ipEdit->setText(networkProperty(m_networkId, "ip").toString());
    m_staticIpBox->setChecked(!m_ipEdit->text().isEmpty());
    m_netmaskEdit->setText(networkProperty(m_networkId, "netmask").toString());
    m_gatewayEdit->setText(networkProperty(m_networkId, "gateway").toString());
    m_staticdnsBox->setChecked(networkProperty(m_networkId, "use_static_dns").toBool());
    m_globaldnsBox->setChecked(networkProperty(m_networkId, "use_global_dns").toBool());
    m_dnsdomainEdit->setText(networkProperty(m_networkId, "dns_domain").toString());
    m_searchdomainEdit->setText(networkProperty(m_networkId, "search_domain").toString());
    m_dns1Edit->setText(networkProperty(m_networkId, "dns1").toString());
    m_dns2Edit->setText(networkProperty(m_networkId, "dns2").toString());
    m_dns3Edit->setText(networkProperty(m_networkId, "dns3").toString());
    m_dhcphostnameBox->setChecked(networkProperty(m_networkId, "use_dhcphostname").toBool());
    m_dhcphostnameEdit->setText(networkProperty(m_networkId, "dhcphostname").toString());
}

void NetworkPropertiesDialog::save()
{
    if (m_staticIpBox->isChecked()) {
        setNetworkProperty(m_networkId, "ip", Tools::blankToNone(m_ipEdit->text()));
        setNetworkProperty(m_networkId, "netmask", Tools::blankToNone(m_netmaskEdit->text()));
        setNetworkProperty(m_networkId, "gateway", Tools::blankToNone(m_gatewayEdit->text()));
    } else {
        setNetworkProperty(m_networkId, "ip", "");
        setNetworkProperty(m_networkId, "netmask", "");
        setNetworkProperty(m_networkId, "gateway", "");
    }

    if (m_staticdnsBox->isChecked() && (!m_globaldnsBox->isChecked())) {
        setNetworkProperty(m_networkId, "use_static_dns", true);
        setNetworkProperty(m_networkId, "use_global_dns", false);
        setNetworkProperty(m_networkId, "dns_domain", Tools::blankToNone(m_dnsdomainEdit->text()));
        setNetworkProperty(m_networkId, "search_domain", Tools::blankToNone(m_searchdomainEdit->text()));
        setNetworkProperty(m_networkId, "dns1", Tools::blankToNone(m_dns1Edit->text()));
        setNetworkProperty(m_networkId, "dns2", Tools::blankToNone(m_dns2Edit->text()));
        setNetworkProperty(m_networkId, "dns3", Tools::blankToNone(m_dns3Edit->text()));
    } else if (m_staticdnsBox->isChecked() && m_globaldnsBox->isChecked()) {
        setNetworkProperty(m_networkId, "use_static_dns", true);
        setNetworkProperty(m_networkId, "use_global_dns", true);
    } else {
        setNetworkProperty(m_networkId, "use_static_dns", false);
        setNetworkProperty(m_networkId, "use_global_dns", false);
        setNetworkProperty(m_networkId, "dns_domain", "");
        setNetworkProperty(m_networkId, "search_domain", "");
        setNetworkProperty(m_networkId, "dns1", "");
        setNetworkProperty(m_networkId, "dns2", "");
        setNetworkProperty(m_networkId, "dns3", "");
    }
    setNetworkProperty(m_networkId, "use_dhcphostname", m_dhcphostnameBox->isChecked());
    setNetworkProperty(m_networkId, "dhcphostname", Tools::blankToNone(m_dhcphostnameEdit->text()));

    if (m_networkId != -1) {
        setNetworkProperty(m_networkId, "automatic", m_autoconnectBox->isChecked() ? "True" : "False");
        if (m_useEncryptionBox->isChecked()) {
            Encryption currentEncryption = m_encryptions.value(m_encryptionCombo->currentIndex());
            setNetworkProperty(m_networkId, "enctype", currentEncryption.value("type"));
            //save values
            foreach (QString key, m_encryptLabelEntries.keys()) {
                setNetworkProperty(m_networkId, key, m_encryptLabelEntries.value(key)->text());
            }
        } else {
            setNetworkProperty(m_networkId, "enctype", "None");
        }

        if (m_globalSettingsBox->isChecked()) {
            setNetworkProperty(m_networkId, "use_settings_globally", true);
        } else {
            setNetworkProperty(m_networkId, "use_settings_globally", false);
            DBusHandler::instance()->callWireless("RemoveGlobalEssidEntry", m_networkId);
        }
        DBusHandler::instance()->callWireless("SaveWirelessNetworkProfile", m_networkId);
    } else {
        DBusHandler::instance()->callWired("SaveWiredNetworkProfile", Wicd::currentprofile);
    }
}

QVariant NetworkPropertiesDialog::networkProperty(int networkId, const QString &property) const
{
    if (networkId == -1) {
        return DBusHandler::instance()->callWired("GetWiredProperty", property);
    } else {
        return DBusHandler::instance()->callWireless("GetWirelessProperty", networkId, property);
    }
}

void NetworkPropertiesDialog::setNetworkProperty(int networkId, const QString &property, const QVariant &value) const
{
    if (networkId == -1) {
        DBusHandler::instance()->callWired("SetWiredProperty", property, value);
    } else {
        DBusHandler::instance()->callWireless("SetWirelessProperty", networkId, property, value);
    }
}
