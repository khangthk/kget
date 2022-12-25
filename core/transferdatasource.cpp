/* This file is part of the KDE project

   Copyright (C) 2008 Manolo Valdes <nolis71cu@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "transferdatasource.h"

#include "kget_debug.h"
#include <QDebug>

TransferDataSource::TransferDataSource(const QUrl &srcUrl, QObject *parent)
    : QObject(parent)
    , m_sourceUrl(srcUrl)
    , m_speed(0)
    , m_supposedSize(0)
    , m_parallelSegments(1)
    , m_currentSegments(0)
    , m_capabilities()
{
    qCDebug(KGET_DEBUG);
}

TransferDataSource::~TransferDataSource()
{
    qCDebug(KGET_DEBUG);
}

Transfer::Capabilities TransferDataSource::capabilities() const
{
    return m_capabilities;
}

void TransferDataSource::setCapabilities(Transfer::Capabilities capabilities)
{
    m_capabilities = capabilities;
    Q_EMIT capabilitiesChanged();
}

void TransferDataSource::findFileSize(KIO::fileoffset_t segmentSize)
{
    Q_UNUSED(segmentSize);
}

QPair<int, int> TransferDataSource::removeConnection()
{
    return QPair<int, int>(-1, -1);
}

QList<QPair<int, int>> TransferDataSource::assignedSegments() const
{
    return QList<QPair<int, int>>();
}

int TransferDataSource::countUnfinishedSegments() const
{
    return 0;
}

QPair<int, int> TransferDataSource::split()
{
    return QPair<int, int>(-1, -1);
}

int TransferDataSource::parallelSegments() const
{
    return m_parallelSegments;
}

void TransferDataSource::setParallelSegments(int parallelSegments)
{
    m_parallelSegments = parallelSegments;
}

int TransferDataSource::currentSegments() const
{
    return m_currentSegments;
}

int TransferDataSource::changeNeeded() const
{
    return parallelSegments() - currentSegments();
}
