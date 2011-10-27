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

#ifndef NETWORKPROPERTIESDIALOG_H
#define NETWORKPROPERTIESDIALOG_H

#include "labelentry.h"
#include "global.h"

#include <QVBoxLayout>
#include <QCheckBox>

#include <KDialog>
#include <KPushButton>
#include <KComboBox>

class NetworkPropertiesDialog : public KDialog
{
    Q_OBJECT

public:
    NetworkPropertiesDialog(int networkId, QWidget *parent = 0, Qt::WFlags flags = 0);
    ~NetworkPropertiesDialog();

private slots:
    void autoComplete();
    void toggleIpCheckbox(bool toggled);
    void toggleStaticDnsCheckbox(bool toggled);
    void toggleGlobalDnsCheckbox(bool toggled);
    void toggleUseEncryptionBox(bool toggled);
    void validate();
    void encryptMethodChanged();
    void editScripts();

private:
    void load();
    void save();
    QVariant networkProperty(int networkId, const QString& property) const;
    void setNetworkProperty(int networkId, const QString& property, const QVariant& value) const;

    int m_networkId;
    QString wiredProfile;

    QCheckBox *m_autoconnectBox;
    QCheckBox *m_staticIpBox;
    LabelEntry *m_ipEdit;
    LabelEntry *m_netmaskEdit;
    LabelEntry *m_gatewayEdit;
    QCheckBox *m_staticdnsBox;
    QCheckBox *m_globaldnsBox;
    LabelEntry *m_dnsdomainEdit;
    LabelEntry *m_searchdomainEdit;
    LabelEntry *m_dns1Edit;
    LabelEntry *m_dns2Edit;
    LabelEntry *m_dns3Edit;
    QCheckBox *m_dhcphostnameBox;
    KLineEdit *m_dhcphostnameEdit;
    QCheckBox* m_globalSettingsBox;
    QCheckBox* m_useEncryptionBox;
    KComboBox* m_encryptionCombo;
    QMap<QString, LabelEntry*> m_encryptLabelEntries;
    QVBoxLayout *m_encryptlayout;
    KPushButton *m_scriptsButton;

    QList<Encryption> m_encryptions;

};

#endif
