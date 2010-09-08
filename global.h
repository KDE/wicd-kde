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

#ifndef TOOLS_H
#define TOOLS_H

#include "types.h"
#include <QList>

namespace Wicd {

    extern QString wicdpath;
    extern QString wicdencryptionpath;
    extern QList<Encryption> encryptionlist;
    extern QString currentprofile;
    extern void locate();

}

namespace Tools {

    extern bool isValidIP(const QString& ip);
    extern QString blankToNone(const QString &text);
    extern QString noneToBlank(const QString &text);

}

#endif
