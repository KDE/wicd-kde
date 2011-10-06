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

#include"labelentry.h"
#include <QBoxLayout>

LabelEntry::LabelEntry( QString label, QWidget *parent)
    : QWidget(parent)
{
    QBoxLayout* boxlayout = new QBoxLayout(QBoxLayout::LeftToRight);
    boxlayout->setContentsMargins(0, 0, 0, 0);
    setLayout(boxlayout);
    m_label = new QLabel(label);
    m_label->setMinimumWidth(170);
    m_value = new KLineEdit();
    m_value->setMinimumWidth(200);
    boxlayout->addWidget(m_label);
    boxlayout->addWidget(m_value);
    connect(m_value, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
}

LabelEntry::~LabelEntry()
{
}

QString LabelEntry::label() const
{
    return m_label->text();
}

QString LabelEntry::text() const
{
    return m_value->text();
}

int LabelEntry::labelMinimumWidth() const
{
    return m_label->minimumWidth();
}

void LabelEntry::setText(const QString &text)
{
    m_value->setText(text);
}

void LabelEntry::setEchoMode(QLineEdit::EchoMode mode)
{
    m_value->setEchoMode(mode);
}

void LabelEntry::setEnabled(bool enable)
{
    m_label->setEnabled(enable);
    m_value->setEnabled(enable);
}
