/* This file is part of the KDE project

   Copyright (C) 2007 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "btdetailswidget.h"

#include "bttransferhandler.h"
#include "ui_btdetailswidgetfrm.h"

#include <kdebug.h>

BTDetailsWidget::BTDetailsWidget(BTTransferHandler * transfer)
    : m_transfer(transfer)
{
    setupUi(this);

    srcEdit->setText(transfer->source().url());
    destEdit->setText(transfer->dest().url());

    transfer->addObserver(this);
    //This updates the widget with the right values
    transferChangedEvent(transfer);
}

BTDetailsWidget::~BTDetailsWidget()
{
    m_transfer->delObserver(this);
}

void BTDetailsWidget::transferChangedEvent(TransferHandler * transfer)
{
    TransferHandler::ChangesFlags transferFlags = m_transfer->changesFlags(this);

    if(transferFlags & Transfer::Tc_Speed)
    {
        kDebug(5001) << m_transfer->dlRate();
        dlSpeedLabel->setText(KGlobal::locale()->formatByteSize(m_transfer->dlRate()));
        ulSpeedLabel->setText(KGlobal::locale()->formatByteSize(m_transfer->ulRate()));
    }

    seederLabel->setText(QString().setNum(m_transfer->seedsConnected()) + "(" + QString().setNum(m_transfer->seedsDisconnected()) + ")");
    leecherLabel->setText(QString().setNum(m_transfer->leechesConnected()) + "(" + QString().setNum(m_transfer->leechesDisconnected()) + ")");
    chunksDownloadedLabel->setText(QString().setNum(m_transfer->chunksDownloaded()));
    chunksExcludedLabel->setText(QString().setNum(m_transfer->chunksExcluded()));
    chunksAllLabel->setText(QString().setNum(m_transfer->chunksTotal()));
    chunksLeftLabel->setText(QString().setNum(m_transfer->chunksLeft()));
    progressBar->setValue(m_transfer->percent());

    m_transfer->resetChangesFlags(this);
}

#include "btdetailswidget.moc"
