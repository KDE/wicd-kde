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

#ifndef WICDAPPLET_HEADER
#define WICDAPPLET_HEADER

#include "networkplotter.h"
#include "networkview.h"
#include "ui_wicdappletConfig.h"

#include <Plasma/PopupApplet>
#include <Plasma/Svg>
#include <Plasma/Label>
#include <Plasma/ToolButton>
#include <Plasma/BusyWidget>
#include <Plasma/ScrollWidget>

class QSizeF;

// Define our plasma Applet
class WicdApplet : public Plasma::PopupApplet
{
    Q_OBJECT
public:
    // Basic Create/Destroy
    WicdApplet(QObject *parent, const QVariantList &args);
    ~WicdApplet();

    // The paintInterface procedure paints the applet to the desktop
    void paintInterface(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        const QRect& contentsRect);
    void init();
    QList<QAction*> contextualActions();

public slots:
    void dataUpdated(const QString &name, const Plasma::DataEngine::Data &data);
    void configChanged();
    void toolTipAboutToShow();
    void toolTipHidden();

protected:
    void popupEvent(bool show);
    void createConfigurationInterface(KConfigDialog *parent);

protected slots:
    void configAccepted();

private slots:
    void updateColors();
    void launchProfileManager();
    void cancelConnect() const;
    void showPreferences() const;
    void createAdhocDialog() const;
    void findHiddenDialog() const;
    void scan() const;
    void connectionInfoRequested();

private:
    void setupActions();
    QString qualityToIcon(int quality) const;
    void loadNetworks();
    void setScanning(bool isScanning);
    void showPlotter(bool show);
    void notify(const QString& event, const QString& message) const;
    void checkConnectionResult(const QString& result);

    // Configuration dialog
    Ui::WicdAppletConfig ui;
    bool m_showSignalStrength;
    bool m_autoScan;
    bool m_showPlotter;

    Plasma::Svg *m_theme;
    //for translations
    QHash<QString, QString> m_messageTable;

    //dataengine management
    QString m_icon;
    Status m_status;
    QString m_message;
    QString m_interface;
    Plasma::Service *m_wicdService;

    //Popup elements
    QGraphicsLinearLayout * m_dialoglayout;
    Plasma::ScrollWidget *m_scrollWidget;
    NetworkView *m_networkView;
    Plasma::BusyWidget *m_busyWidget;
    NetworkPlotter *m_plotter;
    Plasma::Label *m_messageBox;
    Plasma::ToolButton *m_abortButton;
};

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(wicd-kde, WicdApplet)
#endif
