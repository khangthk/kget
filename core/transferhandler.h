
/* This file is part of the KDE project

   Copyright (C) 2005 Dario Massarin <nekkar@libero.it>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; version 2
   of the License.
*/


#ifndef _TRANSFERHANDLER_H
#define _TRANSFERHANDLER_H

#include <qvaluelist.h>
#include <qmap.h>

#include "transfer.h"

class TransferObserver;

/**
 * Class TransferHandler:
 *
 * --- Overview ---
 * This class is the rapresentation of a Transfer object from the views'
 * perspective (proxy pattern). In fact the views never handle directly the 
 * Transfer objects themselves (becouse this would break the model/view policy).
 * As a general rule, all the code strictly related to the views should placed 
 * here (and not in the transfer implementation).
 * Here we provide the same api available in the transfer class, but we change
 * the implementation of some methods.
 * Let's give an example about this:
 * The start() function of a specific Transfer (let's say TransferKio) is a 
 * low level command that really makes the transfer start and should therefore
 * be executed only by the scheduler.
 * The start() function in this class is implemented in another way, since
 * it requests the scheduler to execute the start command to this specific transfer.
 * At this point the scheduler will evaluate this request and execute, if possible,
 * the start() function directly in the TransferKio.
 *
 * --- Notifies about the transfer changes ---
 * When a view is interested in receiving notifies about the specific transfer
 * rapresented by this TransferHandler object, it should add itself to the list
 * of observers calling the addObserver(TransferObserver *) function. On the 
 * contrary call the delObserver(TransferObserver *) function to remove it.
 *
 * --- Interrogation about what has changed in the transfer ---
 * When a TransferObserver receives a notify about a change in the Transfer, it
 * can ask to the TransferHandler for the TFlags.
 */

class TransferHandler : public Transfer
{
    friend class Transfer;

    public:

        TransferHandler(Transfer * transfer, Scheduler * scheduler);

        virtual ~TransferHandler();

        /**
         * Adds an observer to this Transfer
         *
         * @param observer Tthe new observer that should be added
         */
        void addObserver(TransferObserver * observer);

        /**
         * Removes an observer from this Transfer
         *
         * @param observer The observer that should be removed
         */
        void delObserver(TransferObserver * observer);

        /**
         * These are all virtual functions reimplemented from the Job class
         */
        void start();
        void stop();
        int elapsedTime() const;
        int remainingTime() const;
        bool isResumable() const;


        /**
         * Returns the total size of the transfer in bytes
         */
        unsigned long totalSize() const;

        /**
         * Returns the downloaded size of the transfer in bytes
         */
        unsigned long processedSize() const;

        /**
         * Returns the progress percentage of the transfer
         */
        int percent() const;

        /**
         * Returns the download speed of the transfer in bytes/sec
         */
        int speed() const;

        /**
         * Returns a string describing the current transfer status
         */
        const QString & statusText() const {return m_statusText;}

        /**
         * Returns a pixmap associated with the current transfer status
         */
        const QPixmap & statusPixmap() const {return m_statusPixmap;}

        /**
         * Returns the changes flags
         *
         * @param observer The observer that makes this request
         */
        ChangesFlags changesFlags(TransferObserver * observer) const;

        /**
         * Resets the changes flags for a given TransferObserver
         *
         * @param observer The observer that makes this request
         */
        void resetChangesFlags(TransferObserver * observer);

    private:
        /**
         * Sets a change flag in the ChangesFlags variable.
         *
         * @param change The TransferChange flag to be set
         * @param postEvent if false the handler will not post an event to the observers,
         * if true the handler will post an event to the observers
         */
        void setTransferChange(ChangesFlags change, bool postEvent=false);

        /**
         * Posts a TransferChangedEvent to all the observers.
         */
        void postTransferChangedEvent();

        Transfer * m_transfer;
        Scheduler * m_scheduler;

        QValueList<TransferObserver *> m_observers;
        QMap<TransferObserver *, ChangesFlags> m_changesFlags;
};

#endif
