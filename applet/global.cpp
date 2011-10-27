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

#include "global.h"
#include <config-python.h>

#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QHostAddress>

namespace Wicd {

    QString wicdpath;
    QString wicdencryptionpath;
    QList<Encryption> encryptionlist;
    QString currentprofile;

    QString parseLine(QString field, const QString& key)
    {
        QString result =  field.remove(key).remove('=');
        //remove new line
        result.chop(1);
        return result;
    }

    //returns the details of a given encryption
    Encryption encryption(const QString& encryptionFile)
    {
        QFile file(wicdencryptionpath+encryptionFile);
        if (!file.open(QIODevice::ReadOnly))  {
            qDebug()<<"Could not open "<<file.fileName();
            return Encryption();
        }
        Encryption encrypt;
        //the name of the file is the encryption type
        encrypt.insert("type", encryptionFile);
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            //look for the name
            if (QString(line).startsWith("name")) {
                QString name = parseLine(QString(line), "name");
                encrypt.insert("name", name);
            }
            //look for "require"
            else if (QString(line).startsWith("require")) {
                QString requireLine = parseLine(QString(line), "require");
                QStringList requiredList = requireLine.split(' ', QString::SkipEmptyParts);
                //requiredList contains a sequence of pairs(value, display value)
                if ((requiredList.length()%2) == 0) {
                    for (int i = 0; i<requiredList.length()/2; ++i) {
                        QStringList pair;
                        pair << requiredList.value(2*i) << requiredList.value(2*i+1);
                        encrypt.insertMulti("require", pair);
                    }
                }
            }
            //look for "optional"
            else if (QString(line).startsWith("optional")) {
                QString optionalLine = parseLine(QString(line), "optional");
                QStringList optionalList = optionalLine.split(' ', QString::SkipEmptyParts);
                //optionalList contains a sequence of pairs(value, display value)
                if ((optionalList.length()%2) == 0) {
                    for (int i = 0; i<optionalList.length()/2; ++i) {
                        QStringList pair;
                        pair << optionalList.value(2*i) << optionalList.value(2*i+1);
                        encrypt.insertMulti("optional", pair);
                    }
                }
            }
        }
        file.close();
        return encrypt;
    }

    void locate()
    {
        QProcess wpath;
        wpath.start(PYTHONBIN, QStringList() << "-c" << "from wicd import wpath; print wpath.etc");
        wpath.waitForFinished();
        wicdpath = QString(wpath.readAllStandardOutput());
        wicdpath.chop(1);
        wpath.start(PYTHONBIN, QStringList() << "-c" << "from wicd import wpath; print wpath.encryption");
        wpath.waitForFinished();
        wicdencryptionpath = QString(wpath.readAllStandardOutput());
        wicdencryptionpath.chop(1);

        //build the list of available encryptions
        //Get the types present in the file "active"
        QFile file(wicdencryptionpath+"active");
        if (!file.open(QIODevice::ReadOnly))  {
            qDebug()<<"Could not open "<<file.fileName();
            return;
        }
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            QString cleanPath = QString(line);
            //remove new line
            cleanPath.chop(1);
            encryptionlist.append(encryption(cleanPath));
        }
        file.close();
    }
}

namespace Tools {

    bool isValidIP(const QString& ip)
    {
        QHostAddress address(ip);
        return address.protocol() != QAbstractSocket::UnknownNetworkLayerProtocol;
    }

    QString blankToNone(const QString &text)
    {
        return (text.isEmpty()) ? QString("None") : text;
    }

    QString noneToBlank(const QString &text)
    {
        return (text == "None") ? QString() : text;
    }
}

