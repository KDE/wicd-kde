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

class MyApp : public KUniqueApplication
{
public:
    MyApp() : KUniqueApplication() { KGlobal::ref(); }
    ~MyApp() {}

    int newInstance() { return 0; }
};

int main(int argc, char *argv[])
{

    KAboutData aboutData( "wicd-client-kde", 0, ki18n("Wicd Client KDE"),
                          QByteArray("0.2.0"),
                          ki18n("Wicd client for KDE"),
                          KAboutData::License_GPL_V3,
                          ki18n("(c) 2010 Anthony Vital"));
    aboutData.setProgramIconName("network-wireless");

    KCmdLineArgs::init(argc, argv, &aboutData);

    if (!KUniqueApplication::start()) {
        return 0;
    }

    MyApp app;

    MainWindow *window = new MainWindow();
    window->setObjectName("wicdclientkde");

    return app.exec();
}
