/* This file is part of the KDE project

   Copyright (C) 2004 Dario Massarin <nekkar@libero.it>
   Copyright (C) 2006 Manolo Valdes <nolis71cu@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; version 2
   of the License.
*/

#ifndef MULTITHREADED_FACTORY_H
#define MULTITHREADED_FACTORY_H

#include "core/plugin/transferfactory.h"

class Transfer;
class TransferGroup;
class Scheduler;

class TransferMultiThreadedFactory : public TransferFactory
{
    public:
        TransferMultiThreadedFactory();
        ~TransferMultiThreadedFactory();

        Transfer * createTransfer( KUrl srcURL, KUrl destURL,
                                   TransferGroup * parent, Scheduler * scheduler,
                                   const QDomElement * e = 0 );

        TransferHandler * createTransferHandler(Transfer * transfer,
                                                Scheduler * scheduler);

        QWidget * createDetailsWidget( TransferHandler * transfer );

        const QList<KAction *> actions();
};

#endif // MULTITHREADED_FACTORY_H
