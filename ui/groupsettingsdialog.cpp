/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>
   Copyright (C) 2009 Matthias Fuchs <mat69@gmx.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "groupsettingsdialog.h"

#include "core/transfergrouphandler.h"
#include <KFileDialog>

#ifdef HAVE_NEPOMUK
    #include <Nepomuk/Tag>
    #include <Nepomuk/TagWidget>
#endif

GroupSettingsDialog::GroupSettingsDialog(QWidget *parent, TransferGroupHandler *group)
  : KDialog(parent),
    m_group(group)
{
    setCaption(i18n("Group Settings for %1", group->name()));
    showButtonSeparator(true);

    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);

    setMainWidget(widget);

    ui.downloadBox->setValue(group->downloadLimit(Transfer::VisibleSpeedLimit));
    ui.uploadBox->setValue(group->uploadLimit(Transfer::VisibleSpeedLimit));

    if (ui.downloadBox->value() != 0)
        ui.downloadCheck->setChecked(true);

    if (ui.uploadBox->value() != 0)
        ui.uploadCheck->setChecked(true);

    ui.defaultFolderRequester->setMode(KFile::Directory);
    QString path = group->defaultFolder();
    ui.defaultFolderRequester->setUrl(path);
    ui.defaultFolderRequester->setStartDir(KUrl(KGet::generalDestDir(true)));

    ui.regExpEdit->setText(group->regExp().pattern());

#ifdef HAVE_NEPOMUK
    m_tagWidget = new Nepomuk::TagWidget(this);
    m_tagWidget->setSelectedTags(group->tags());
    m_tagWidget->setModeFlags(Nepomuk::TagWidget::MiniMode);
    ui.nepomukWidget->layout()->addWidget(m_tagWidget);
#else
    ui.nepomukWidget->hide();
#endif

    connect(this, SIGNAL(accepted()), SLOT(save()));
}

GroupSettingsDialog::~GroupSettingsDialog()
{
}

void GroupSettingsDialog::save()
{
    m_group->setDefaultFolder(ui.defaultFolderRequester->url().toLocalFile(KUrl::AddTrailingSlash));

    if (ui.downloadCheck->isChecked())
        m_group->setDownloadLimit(ui.downloadBox->value(), Transfer::VisibleSpeedLimit);
    else
        m_group->setDownloadLimit(0, Transfer::VisibleSpeedLimit);

    if (ui.uploadCheck->isChecked())
        m_group->setUploadLimit(ui.uploadBox->value(), Transfer::VisibleSpeedLimit);
    else
        m_group->setUploadLimit(0, Transfer::VisibleSpeedLimit);

    QRegExp regExp;
    regExp.setPattern(ui.regExpEdit->text());
    m_group->setRegExp(regExp);

#ifdef HAVE_NEPOMUK
    m_group->setTags(m_tagWidget->selectedTags());
#endif
}

#include "groupsettingsdialog.moc"
