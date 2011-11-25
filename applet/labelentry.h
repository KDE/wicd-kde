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

#ifndef LABELENTRY_H
#define LABELENTRY_H

#include <QWidget>
#include <QLabel>
#include <KLineEdit>

class LabelEntry : public QWidget
{
    Q_OBJECT

private:
    QLabel *m_label;
    KLineEdit *m_value;

public:
    explicit LabelEntry( QString label, QWidget *parent  = 0 );
    ~LabelEntry();
    QString label() const;
    QString text() const;
    int labelMinimumWidth() const;
    void setText(const QString &text);
    void setEchoMode(QLineEdit::EchoMode mode);

signals:
    void editingFinished();

public slots:
    void setEnabled(bool enable);

};

#endif
