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

#include "scriptshelper.h"

#include <QSettings>
#include <QFile>
#include <QStringList>
#include <QTextStream>

ActionReply ScriptsHelper::read(QVariantMap args)
{
    ActionReply reply;

    QSettings settings(args.value("filename").toString(), QSettings::IniFormat);
    QString group = args.value("key").toString();
    settings.beginGroup(group);
    reply.addData("beforescript", settings.value("beforescript"));
    reply.addData("afterscript", settings.value("afterscript"));
    reply.addData("predisconnectscript", settings.value("predisconnectscript"));
    reply.addData("postdisconnectscript", settings.value("postdisconnectscript"));
    settings.endGroup();

    return reply;
}

ActionReply ScriptsHelper::save(QVariantMap args)
{
    QString filename = args.value("filename").toString();

    QSettings* settings = new QSettings(filename, QSettings::IniFormat);
    QString group = args.value("key").toString();
    settings->beginGroup(group);
    settings->setValue("beforescript", args.value("beforescript"));
    settings->setValue("afterscript", args.value("afterscript"));
    settings->setValue("predisconnectscript", args.value("predisconnectscript"));
    settings->setValue("postdisconnectscript", args.value("postdisconnectscript"));
    settings->endGroup();
    delete settings;//force QSettings to close the file

    //QSettings insists on writing groups in ASCII so clean up file formatting
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return ActionReply::HelperErrorReply;

    QTextStream in(&file);
    QStringList lines = in.readAll().split("\n");
    file.close();

    if (!file.open(QIODevice::WriteOnly))
        return ActionReply::HelperErrorReply;

    QTextStream out(&file);
    for (int i = 0; i<lines.length(); ++i) {
        QString line = lines.at(i);
        if (line.startsWith('[') && line.endsWith(']')) {
            line.replace("%3A", ":");
        }
        out << line << "\n";
    }

    return ActionReply::SuccessReply;
}

KDE4_AUTH_HELPER_MAIN("org.kde.wicdclient.scripts", ScriptsHelper)
