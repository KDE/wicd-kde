/****************************************************************************
 *  Copyright (c) 2010-2011 Anthony Vital <anthony.vital@gmail.com>         *
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

#include "adhocdialog.h"

#include <QVBoxLayout>

#include <KLocalizedString>

AdhocDialog::AdhocDialog()
    : KDialog()
{
    setModal(true);
    setCaption(i18n("Create an ad-hoc network"));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *vboxlayout = new QVBoxLayout();

    m_essidEdit = new LabelEntry(i18n("ESSID:"));
    m_essidEdit->setText(i18nc("Template name for an ad-hoc network", "My_Adhoc_Network"));
    m_ipEdit = new LabelEntry(i18n("IP:"));
    m_ipEdit->setText("169.254.12.10");
    m_channelEdit = new LabelEntry(i18n("Channel:"));
    m_channelEdit->setText("3");
    m_icsBox = new QCheckBox(i18n("Activate Internet Connection Sharing"));//useless?
    m_icsBox->setEnabled(false);
    m_wepBox = new QCheckBox(i18n("Use Encryption (WEP only)"));
    m_keyEdit = new LabelEntry(i18n("Key:"));
    m_keyEdit->setEnabled(false);
    connect(m_wepBox, SIGNAL(toggled(bool)), m_keyEdit, SLOT(setEnabled(bool)));

    vboxlayout->addWidget(m_essidEdit);
    vboxlayout->addWidget(m_ipEdit);
    vboxlayout->addWidget(m_channelEdit);
    vboxlayout->addWidget(m_icsBox);
    vboxlayout->addWidget(m_wepBox);
    vboxlayout->addWidget(m_keyEdit);
    vboxlayout->addStretch();

    widget->setLayout(vboxlayout);
    setMainWidget( widget );
}

QString AdhocDialog::essid() const
{
    return m_essidEdit->text();
}

QString AdhocDialog::ip() const
{
    return m_ipEdit->text();
}

QString AdhocDialog::channel() const
{
    return m_channelEdit->text();
}

bool AdhocDialog::ics() const
{
    return m_icsBox->isChecked();
}

bool AdhocDialog::wep() const
{
    return m_wepBox->isChecked();
}

QString AdhocDialog::key() const
{
    return m_keyEdit->text();
}
