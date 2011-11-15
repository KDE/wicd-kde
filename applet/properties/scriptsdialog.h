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

#ifndef SCRIPTSDIALOG_H
#define SCRIPTSDIALOG_H

#include <KDialog>
#include <KAuth/Action>
#include <KLineEdit>

class ScriptsDialog : public KDialog
{
    Q_OBJECT

public:
    ScriptsDialog(const QString &key, const QString &path ,QWidget *parent = 0, Qt::WFlags flags = 0);
    ~ScriptsDialog();
    int authorized() const;

private slots:
    void save(KAuth::Action* action);

private:
    KLineEdit* m_precon;
    KLineEdit* m_postcon;
    KLineEdit* m_predis;
    KLineEdit* m_postdis;

    int m_authorized;

};

#endif

