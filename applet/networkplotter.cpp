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

#include "networkplotter.h"

#include <QGraphicsLinearLayout>

#include <KGlobalSettings>

#include <Plasma/DataEngineManager>

NetworkPlotter::NetworkPlotter(QGraphicsItem *parent = 0)
    : Plasma::SignalPlotter( parent )
{
    setThinFrame(false);
    setShowLabels(false);
    setShowTopBar(false);
    setShowVerticalLines(false);
    setShowHorizontalLines(false);
    setUseAutoRange(true);
    
    addPlot(QColor("#0099ff"));
    addPlot(QColor("#91ff00"));
    
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    setLayout(layout);
    m_overlayFrame = new Plasma::Frame(this);
    m_overlayFrame->setFont(KGlobalSettings::smallestReadableFont());
    layout->addStretch();
    QGraphicsLinearLayout* layout2 = new QGraphicsLinearLayout(Qt::Horizontal, layout);
    layout2->addStretch();
    layout2->addItem(m_overlayFrame);
    layout2->addStretch();
    layout->addItem(layout2);
    
    setMinimumHeight(60);
    
    Plasma::DataEngineManager::self()->loadEngine("systemmonitor");
    m_data = QList<double>() << -1 << -1;
    m_interval = 1000;
}

NetworkPlotter::~NetworkPlotter()
{
    Plasma::DataEngineManager::self()->unloadEngine("systemmonitor");
}

void NetworkPlotter::setInterface(const QString interface)
{
    if (interface.isEmpty() || (!interface.isEmpty() && interface == m_interface)) {
        return;
    }

    Plasma::DataEngine *e = engine();
    if (!e)
        return;
    
    e->disconnectSource("network/interfaces/"+m_interface+"/receiver/data", this);
    e->disconnectSource("network/interfaces/"+m_interface+"/transmitter/data", this);

    e->connectSource("network/interfaces/"+interface+"/receiver/data", this, m_interval);
    e->connectSource("network/interfaces/"+interface+"/transmitter/data", this, m_interval);
    
    m_interface = interface;
}

void NetworkPlotter::dataUpdated(const QString& source, const Plasma::DataEngine::Data &data)
{
    QStringList split = source.split('/');
    if (split.length() < 4) {
        return;
    }
    //are we getting received or transmitted data?
    int index = (split[3] == "receiver") ? 0 : 1;
    //fill m_data accordingly
    m_data[index] = qMax(qreal(0.0), data["value"].toDouble());
    //update plotter only when the 2 values are filled
    if (!m_data.contains(-1)) {
        addSample(m_data);
        QStringList list;
        list << QString("%1 %2").arg(m_data[0], 0, 'f', (m_data[0] > 1000.0) ? 0 : 1).arg(i18n("KiB/s"));
        list << QString("%1 %2").arg(m_data[1], 0, 'f', (m_data[1] > 1000.0) ? 0 : 1).arg(i18n("KiB/s"));
        m_overlayFrame->setText(list.join(" / "));
        m_data = QList<double>() << -1 << -1;
    }
}

Plasma::DataEngine* NetworkPlotter::engine()
{
    Plasma::DataEngine *e = Plasma::DataEngineManager::self()->engine("systemmonitor");
    if (e->isValid()) {
        return e;
    } else {
        return 0;
    }
}


