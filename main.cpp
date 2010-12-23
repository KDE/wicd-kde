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

#include <KUniqueApplication>
#include <KCmdLineArgs>
#include <KAboutData>

#include "mainwindow.h"

class WicdClient : public KUniqueApplication
{
public:
    WicdClient() : KUniqueApplication()
    {
        KGlobal::ref();
        window = new MainWindow();
        window->setObjectName("wicdclientkde");
    }

    ~WicdClient()
    {
        delete window;
    }

    int newInstance() { return 0; }

private:
    MainWindow * window;
};

int main(int argc, char *argv[])
{

    KAboutData aboutData( "wicd-kde", 0, ki18n("Wicd Client KDE"),
                          QByteArray("0.2.1"),
                          ki18n("Wicd client for KDE"),
                          KAboutData::License_GPL_V3,
                          ki18n("(c) 2010 Anthony Vital"));
    aboutData.addAuthor(ki18n("Anthony Vital"), ki18n("Author"), "anthony.vital@gmail.com");
    aboutData.addCredit(ki18n("Dario Freddi"), ki18n("Contributor and original KCM author"), "drf@kde.org", "http://drfav.wordpress.com");
    aboutData.addCredit(ki18n("Brieuc Roblin"), ki18n("Contributor"), "brieuc.roblin@gmail.com");
    aboutData.addCredit(ki18n("Yoann Laissus"), ki18n("Contributor"), "yoann.laissus@gmail.com");
    aboutData.setBugAddress("anthony.vital@gmail.com");
    aboutData.setProgramIconName("network-wireless");

    KCmdLineArgs::init(argc, argv, &aboutData);

    if (!KUniqueApplication::start()) {
        return 0;
    }

    WicdClient app;

    return app.exec();
}
