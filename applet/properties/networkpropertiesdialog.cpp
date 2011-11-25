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
#include "global.h"
#include "tools.h"
#include "scriptsdialog.h"
#include "dbushandler.h"

#include <KMessageBox>
#include <KLocalizedString>

NetworkPropertiesDialog::NetworkPropertiesDialog(NetworkInfo info, QWidget *parent, Qt::WFlags flags)
    : KDialog(parent, flags)
    , m_info(info)
{
    setModal(true);
    setCaption(m_info.value("essid").toString());
    setButtons( KDialog::User1 | KDialog::Cancel );
    button(KDialog::User1)->setIcon(KIcon("dialog-ok"));
    button(KDialog::User1)->setText(i18n("OK"));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *vboxlayout = new QVBoxLayout();

    m_staticIpBox = new QCheckBox(i18n("Use Static IPs"));
    m_ipEdit = new LabelEntry(i18n("IP:"));
    m_netmaskEdit = new LabelEntry(i18n("Netmask:"));
    m_gatewayEdit = new LabelEntry(i18n("Gateway:"));
    m_staticdnsBox = new QCheckBox(i18n("Use Static DNS"));
    m_globaldnsBox = new QCheckBox(i18n("Use global DNS servers"));
    m_dnsdomainEdit = new LabelEntry(i18n("DNS domain:"));
    m_searchdomainEdit = new LabelEntry(i18n("Search domain:"));
    m_dns1Edit = new LabelEntry(i18n("DNS server 1:"));
    m_dns2Edit = new LabelEntry(i18n("DNS server 2:"));
    m_dns3Edit = new LabelEntry(i18n("DNS server 3:"));
    m_dhcphostnameBox = new QCheckBox(i18n("DHCP Hostname:"));
    m_dhcphostnameBox->setMinimumWidth(m_dns3Edit->labelMinimumWidth());
    m_dhcphostnameEdit = new KLineEdit();

    //optional: wireless
    m_autoconnectBox = new QCheckBox(i18n("Automatically connect to this network"));
    m_globalSettingsBox = new QCheckBox(i18n("Use these settings for all networks sharing this ESSID"));
    m_useEncryptionBox = new QCheckBox(i18n("Use encryption"));
    m_encryptionCombo = new KComboBox();
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

    connect(m_ipEdit, SIGNAL(editingFinished()), this, SLOT(autoComplete()));
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

    m_networkId = m_info.value("networkId").toInt();
    //load properties
    load();

    if (m_networkId != -1) {
        m_autoconnectBox->setChecked(networkProperty("automatic").toBool());
        //insert autoconnect checkbox on top
        vboxlayout->insertWidget(0, m_autoconnectBox);

        m_encryptions = Wicd::encryptionlist;
        //polulate the combobox
        int typeIndex = -1;
        for (int i = 0; i < m_encryptions.length(); ++i) {
            m_encryptionCombo->addItem(m_encryptions.value(i).value("name").toString());
            //look for the current encryption type
            if (networkProperty("enctype").toString() == m_encryptions.value(i).value("type").toString()) {
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
}

NetworkPropertiesDialog::~NetworkPropertiesDialog()
{
}

void NetworkPropertiesDialog::autoComplete()
{
    bool valid = Tools::isValidIP(m_ipEdit->text());
    if (valid) {
        if (m_gatewayEdit->text().isEmpty()) {
            QStringList ipNumbers = m_ipEdit->text().split('.');
            ipNumbers.replace(3, "1");
            m_gatewayEdit->setText(ipNumbers.join("."));
        }
        if (m_netmaskEdit->text().isEmpty())
            m_netmaskEdit->setText("255.255.255.0");
    } else if (!m_ipEdit->text().isEmpty()) {
        KMessageBox::sorry(0, QString(i18n("Invalid IP address entered.")));
    }
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
            KMessageBox::sorry(this, QString(i18n("Invalid address in %1", item->label())));
            return;
        }
    }
    //check optional fields
    foreach (LabelEntry* item, optList) {
        item->setText(item->text().simplified());
        if ( (!item->text().isEmpty()) && (!Tools::isValidIP(item->text())) ) {
            KMessageBox::sorry(this, QString(i18n("Invalid address in %1", item->label())));
            return;
        }
    }
    //check wireless encryption fields
    if (m_networkId != -1) {
        if (m_useEncryptionBox->isChecked()) {
            Encryption currentEncryption = m_encryptions.value(m_encryptionCombo->currentIndex());
            //ensure all required fields are filled
            QList<QVariant> required = currentEncryption.values("require");
            foreach (const QVariant &field, required) {
                QString key = field.toStringList().value(0);
                if (m_encryptLabelEntries.value(key)->text().isEmpty()) {
                    KMessageBox::sorry(this, i18n("%1 is required.", field.toStringList().value(1).replace('_', ' ').remove('*')));
                    return;
                }
            }
        } else if (networkProperty("encryption").toBool()) {
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
        foreach (const QVariant &field, fields) {
            //a field is a pair of strings
            QString name = field.toStringList().value(1);
            QString key = field.toStringList().value(0);
            //new field implies new label entry
            LabelEntry* entry = new LabelEntry(name.replace('_', ' ').remove('*')+" :");
            entry->setEchoMode(QLineEdit::PasswordEchoOnEdit);
            entry->setText(networkProperty(key).toString());
            m_encryptlayout->addWidget(entry);
            //keep trace of this LabelEntry
            m_encryptLabelEntries.insert(key, entry);
        }
    }
}

void NetworkPropertiesDialog::editScripts()
{
    QString key;//key: bssid if wireless, profilename if wired
    QString path;//path: settings configuration file
    QString type;
    if (m_networkId == -1) {
        key = Wicd::currentprofile;
        type = "wired";
    } else {
        key = networkProperty("bssid").toString();
        type = "wireless";
    }
    path = Wicd::wicdpath+type+"-settings.conf";

    QPointer<ScriptsDialog> dialog = new ScriptsDialog(key, path, this);
    dialog->exec();
    if (dialog && dialog->authorized()) {
        if (m_networkId == -1) {
            DBusHandler::instance()->callWired("ReloadConfig");
            DBusHandler::instance()->callWired("ReadWiredNetworkProfile", key);
            DBusHandler::instance()->callWired("SaveWiredNetworkProfile", key);
        } else {
            DBusHandler::instance()->callWireless("ReloadConfig");
            DBusHandler::instance()->callWireless("ReadWirelessNetworkProfile", m_networkId);
            DBusHandler::instance()->callWireless("SaveWirelessNetworkProfile", m_networkId);
        }
    }
    delete dialog;
}

void NetworkPropertiesDialog::load()
{
    m_ipEdit->setText(networkProperty("ip").toString());
    m_staticIpBox->setChecked(!m_ipEdit->text().isEmpty());
    m_netmaskEdit->setText(networkProperty("netmask").toString());
    m_gatewayEdit->setText(networkProperty("gateway").toString());
    m_staticdnsBox->setChecked(networkProperty("use_static_dns").toBool());
    m_globaldnsBox->setChecked(networkProperty("use_global_dns").toBool());
    m_dnsdomainEdit->setText(networkProperty("dns_domain").toString());
    m_searchdomainEdit->setText(networkProperty("search_domain").toString());
    m_dns1Edit->setText(networkProperty("dns1").toString());
    m_dns2Edit->setText(networkProperty("dns2").toString());
    m_dns3Edit->setText(networkProperty("dns3").toString());
    m_dhcphostnameBox->setChecked(networkProperty("use_dhcphostname").toBool());
    m_dhcphostnameEdit->setText(networkProperty("dhcphostname").toString());
}

void NetworkPropertiesDialog::save()
{
    if (m_staticIpBox->isChecked()) {
        setNetworkProperty("ip", Tools::blankToNone(m_ipEdit->text()));
        setNetworkProperty("netmask", Tools::blankToNone(m_netmaskEdit->text()));
        setNetworkProperty("gateway", Tools::blankToNone(m_gatewayEdit->text()));
    } else {
        setNetworkProperty("ip", "");
        setNetworkProperty("netmask", "");
        setNetworkProperty("gateway", "");
    }

    if (m_staticdnsBox->isChecked() && (!m_globaldnsBox->isChecked())) {
        setNetworkProperty("use_static_dns", true);
        setNetworkProperty("use_global_dns", false);
        setNetworkProperty("dns_domain", Tools::blankToNone(m_dnsdomainEdit->text()));
        setNetworkProperty("search_domain", Tools::blankToNone(m_searchdomainEdit->text()));
        setNetworkProperty("dns1", Tools::blankToNone(m_dns1Edit->text()));
        setNetworkProperty("dns2", Tools::blankToNone(m_dns2Edit->text()));
        setNetworkProperty("dns3", Tools::blankToNone(m_dns3Edit->text()));
    } else if (m_staticdnsBox->isChecked() && m_globaldnsBox->isChecked()) {
        setNetworkProperty("use_static_dns", true);
        setNetworkProperty("use_global_dns", true);
    } else {
        setNetworkProperty("use_static_dns", false);
        setNetworkProperty("use_global_dns", false);
        setNetworkProperty("dns_domain", "");
        setNetworkProperty("search_domain", "");
        setNetworkProperty("dns1", "");
        setNetworkProperty("dns2", "");
        setNetworkProperty("dns3", "");
    }
    setNetworkProperty("use_dhcphostname", m_dhcphostnameBox->isChecked());
    setNetworkProperty("dhcphostname", Tools::blankToNone(m_dhcphostnameEdit->text()));

    if (m_networkId != -1) {
        setNetworkProperty("automatic", m_autoconnectBox->isChecked() ? "True" : "False");
        if (m_useEncryptionBox->isChecked()) {
            Encryption currentEncryption = m_encryptions.value(m_encryptionCombo->currentIndex());
            setNetworkProperty("enctype", currentEncryption.value("type"));
            //save values
            const QList<QString> keys = m_encryptLabelEntries.keys();
            foreach (const QString &key, keys) {
                setNetworkProperty(key, m_encryptLabelEntries.value(key)->text());
            }
        } else {
            setNetworkProperty("enctype", "None");
        }

        if (m_globalSettingsBox->isChecked()) {
            setNetworkProperty("use_settings_globally", true);
        } else {
            setNetworkProperty("use_settings_globally", false);
            DBusHandler::instance()->callWireless("RemoveGlobalEssidEntry", m_networkId);
        }
        DBusHandler::instance()->callWireless("SaveWirelessNetworkProfile", m_networkId);
    } else {
        DBusHandler::instance()->callWired("SaveWiredNetworkProfile", Wicd::currentprofile);
    }
}

QVariant NetworkPropertiesDialog::networkProperty(const QString &property) const
{
    if (m_networkId == -1) {
        return DBusHandler::instance()->callWired("GetWiredProperty", property);
    } else {
        return DBusHandler::instance()->callWireless("GetWirelessProperty", m_networkId, property);
    }
}

void NetworkPropertiesDialog::setNetworkProperty(const QString &property, const QVariant &value) const
{
    if (m_networkId == -1) {
        DBusHandler::instance()->callWired("SetWiredProperty", property, value);
    } else {
        DBusHandler::instance()->callWireless("SetWirelessProperty", m_networkId, property, value);
    }
}
