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

#ifndef ADHOCDIALOG_H
#define ADHOCDIALOG_H

#include "labelentry.h"

#include <QCheckBox>

#include <KDialog>

class AdhocDialog : public KDialog
{
public:
    explicit AdhocDialog();
    QString essid() const;
    QString ip() const;
    QString channel() const;
    bool ics() const;
    bool wep() const;
    QString key() const;

private:
    LabelEntry *m_essidEdit;
    LabelEntry *m_ipEdit;
    LabelEntry *m_channelEdit;
    QCheckBox *m_icsBox;
    QCheckBox *m_wepBox;
    LabelEntry *m_keyEdit;
};

#endif // ADHOCDIALOG_H
