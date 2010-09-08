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
#include <QLineEdit>

class ScriptsDialog : public KDialog
{
    Q_OBJECT

public:
    ScriptsDialog(int networkId, QWidget *parent = 0, Qt::WFlags flags = 0);
    ~ScriptsDialog();

private slots:
    void save(KAuth::Action* action);

private:
    QLineEdit* m_precon;
    QLineEdit* m_postcon;
    QLineEdit* m_predis;
    QLineEdit* m_postdis;
    int m_networkId;
    QString m_key;

};

#endif

