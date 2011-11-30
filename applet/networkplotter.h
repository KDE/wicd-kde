/****************************************************************************
 *  Copyright (c) 2011 Anthony Vital <anthony.vital@gmail.com>              *
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

#ifndef NETWORKPLOTTER_H
#define NETWORKPLOTTER_H

#include <Plasma/SignalPlotter>
#include <Plasma/DataEngine>
#include <Plasma/Frame>


class NetworkPlotter : public Plasma::SignalPlotter 
{
    Q_OBJECT

public:
    NetworkPlotter(QGraphicsItem *parent);
    ~NetworkPlotter();
    void setInterface(const QString interface);
    
public slots:
    void dataUpdated(const QString &name, const Plasma::DataEngine::Data &data);
    
private:
    Plasma::Frame* m_overlayFrame;
    QString m_interface;
    uint m_interval;
    QList<double> m_data;
};

#endif


