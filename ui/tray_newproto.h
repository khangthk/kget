/* This file is part of the KDE project

   Copyright (C) 2009 by Fabian Henze <flyser42 AT gmx DOT de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef TRAY_H
#define TRAY_H

#include <knotificationitem-1/knotificationitem.h>

#include "core/observer.h"

class MainWindow;
class KGet;

/**
  * This class implements the main tray icon for kget. It has a popup
  * from which the user can open a new transfer, configure kget, 
  * minimize/restore or quit the app (default behavior).
  *
  * @short KGet's system tray icon.
  **/

class Tray : public Experimental::KNotificationItem
{
Q_OBJECT
public:
    Tray( MainWindow * parent );

    void setDownloading(bool running);
    bool isDownloading();

// filter middle mouse clicks to ask scheduler to paste URL
// This does not yet work with KNotificationItem ...
//private slots:
//    void slotActivated( QSystemTrayIcon::ActivationReason reason );
};

#endif
