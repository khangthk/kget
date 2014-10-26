/***************************************************************************
*   Copyright (C) 2009 Matthias Fuchs <mat69@gmx.net>                     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
***************************************************************************/

#include "verificationdialog.h"

#include <QSortFilterProxyModel>

#include <KLocale>
#include <KMessageBox>

#include "core/filemodel.h"
#include "core/transferhandler.h"
#include "core/verifier.h"
#include "core/verificationmodel.h"
#include "core/verificationdelegate.h"
#include "settings.h"

VerificationAddDlg::VerificationAddDlg(VerificationModel *model, QWidget *parent, Qt::WFlags flags)
  : KDialog(parent, flags),
    m_model(model)
{
    setCaption(i18n("Add checksum"));
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);

    QStringList supportedTypes = Verifier::supportedVerficationTypes();
    supportedTypes.sort();
    ui.hashTypes->addItems(supportedTypes);

    setButtons(KDialog::Yes | KDialog::Cancel);
    setButtonGuiItem(KDialog::Yes, KStandardGuiItem::add());

    updateButton();

    connect(ui.newHash, &KLineEdit::textChanged, this, &VerificationAddDlg::updateButton);
    connect(ui.hashTypes, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &VerificationAddDlg::updateButton);
    connect(this, &VerificationAddDlg::yesClicked, this, &VerificationAddDlg::addChecksum);
}

QSize VerificationAddDlg::sizeHint() const
{
    QSize sh = KDialog::sizeHint();
    sh.setHeight(minimumSize().height());
    sh.setWidth(sh.width() * 1.5);
    return sh;
}

void VerificationAddDlg::updateButton()
{
    const QString type = ui.hashTypes->currentText();
    const QString hash = ui.newHash->text();
    const bool enabled = Verifier::isChecksum(type, hash);

    enableButton(KDialog::Yes, enabled);
    enableButton(KDialog::User1, enabled);
}

void VerificationAddDlg::addChecksum()
{
    if (m_model) {
        m_model->addChecksum(ui.hashTypes->currentText(), ui.newHash->text());
    }
}

VerificationDialog::VerificationDialog(QWidget *parent, TransferHandler *transfer, const QUrl &file)
  : KGetSaveSizeDialog("VerificationDialog", parent),
    m_transfer(transfer),
    m_verifier(transfer->verifier(file)),
    m_model(0),
    m_proxy(0),
    m_fileModel(0)
{
    if (m_verifier) {
        m_model = m_verifier->model();
        connect(m_verifier, &Verifier::verified, this, &VerificationDialog::slotVerified);
    }

    setWindowTitle(i18n("Transfer Verification for %1", file.fileName()));

    ui.setupUi(this);
    
    KGuiItem::assign(ui.add, KStandardGuiItem::add());
    KGuiItem::assign(ui.remove, KStandardGuiItem::remove());
    ui.verifying->hide();

    if (m_model) {
        m_proxy = new QSortFilterProxyModel(this);
        m_proxy->setSourceModel(m_model);
        ui.usedHashes->setModel(m_proxy);
        ui.usedHashes->setItemDelegate(new VerificationDelegate(this));

        QByteArray loadedState = QByteArray::fromBase64(Settings::verificationHeaderState().toAscii());
        if (!loadedState.isEmpty()) {
            ui.usedHashes->header()->restoreState(loadedState);
        }

        m_fileModel = m_transfer->fileModel();
        if (m_fileModel) {
            m_file = m_fileModel->index(file, FileItem::File);
            connect(m_fileModel, &FileModel::fileFinished, this, &VerificationDialog::fileFinished);
        }

        updateButtons();

        connect(m_model, &VerificationModel::dataChanged, this, &VerificationDialog::updateButtons);
        connect(m_model, &VerificationModel::rowsRemoved, this, &VerificationDialog::updateButtons);
        connect(ui.usedHashes, &QTreeView::clicked, this, &VerificationDialog::updateButtons);
        connect(ui.add, &QPushButton::clicked, this, &VerificationDialog::addClicked);
        connect(ui.remove, &QPushButton::clicked, this, &VerificationDialog::removeClicked);
        connect(ui.verify, &QPushButton::clicked, this, &VerificationDialog::verifyClicked);
    }

    connect(this, &VerificationDialog::finished, this, &VerificationDialog::slotFinished);
    connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

QSize VerificationDialog::sizeHint() const
{
    QSize sh = QDialog::sizeHint();
    sh.setWidth(sh.width() * 1.2);
    return sh;
}

void VerificationDialog::slotFinished()
{
    if (m_model) {
        Settings::setVerificationHeaderState(ui.usedHashes->header()->saveState().toBase64());
    }
}

void VerificationDialog::fileFinished(const QUrl &file)
{
    if (m_fileModel && (m_fileModel->getUrl(m_file) == file)) {
        updateButtons();
    }
}

void VerificationDialog::updateButtons()
{
    ui.remove->setEnabled(m_model && ui.usedHashes->selectionModel()->hasSelection());

    //check if the download finished and if the selected index is verifyable
    bool verifyEnabled = false;
    if (m_fileModel && m_fileModel->downloadFinished(m_fileModel->getUrl(m_file))) {
        const QModelIndexList indexes = ui.usedHashes->selectionModel()->selectedRows();
        if (indexes.count() == 1) {
            verifyEnabled = m_verifier->isVerifyable(indexes.first());
        }
    }
    ui.verify->setEnabled(verifyEnabled);
}

void VerificationDialog::removeClicked()
{
    while (ui.usedHashes->selectionModel()->hasSelection()) {
        const QModelIndex index = ui.usedHashes->selectionModel()->selectedRows().first();
        m_model->removeRow(m_proxy->mapToSource(index).row());
    }
}

void VerificationDialog::addClicked()
{
    VerificationAddDlg *dialog = new VerificationAddDlg(m_model, this);
    dialog->show();
}

void VerificationDialog::verifyClicked()
{
    const QModelIndex index = m_proxy->mapToSource(ui.usedHashes->selectionModel()->selectedRows().first());
    if (index.isValid()) {
         m_verifier->verify(index);
         ui.progressBar->setMaximum(0);
         ui.verifying->show();
    }
}

void VerificationDialog::slotVerified(bool verified)
{
    ui.progressBar->setMaximum(1);
    ui.verifying->hide();

    if (verified) {
        QString fileName;
        if (m_fileModel) {
            fileName = m_fileModel->getUrl(m_file).fileName();
        }

        KMessageBox::information(this,
                                 i18n("%1 was successfully verified.", fileName),
                                 i18n("Verification successful"));
    }
}


