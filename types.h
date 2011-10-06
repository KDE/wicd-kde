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

#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QHash>
#include <QVariant>
#include <QStringList>

typedef QHash<QString, QVariant> NetworkInfo;
typedef QVariantMap Encryption;

namespace WicdState {
    enum State { NOT_CONNECTED, CONNECTING, WIRELESS, WIRED, SUSPENDED };
}

struct Status {
    uint State;
    QStringList Info;
};
Q_DECLARE_METATYPE(Status)

#endif

