/****************************************************************************
 *  Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>                    *
 *                                                                          *
 *  This file is part of the KDE project.                                   *
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
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.    *
 ****************************************************************************/

#ifndef WICDKCM_H
#define WICDKCM_H

#include <kcmodule.h>
#include <ksharedconfig.h>

namespace Ui {
    class WicdKcmWidget;
}

class WicdKCM : public KCModule
{
    public:
        WicdKCM(QWidget *parent, const QVariantList &);
        ~WicdKCM();

        virtual void load();
        virtual void save();
        virtual void defaults();

    private:
        void init();

        Ui::WicdKcmWidget *m_ui;
        QHash<QString, int> m_wicdMisc;
        KSharedConfigPtr m_clientConfig;
};

#endif // WICDKCM_H
