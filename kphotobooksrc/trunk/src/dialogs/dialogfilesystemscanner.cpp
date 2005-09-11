/***************************************************************************
 *   Copyright (C) 2003 by Michael Christen                                *
 *   starcube@my-mail.ch                                                   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "dialogfilesystemscanner.h"

#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <ktabwidget.h>
#include <ktextedit.h>

#include <qcheckbox.h>
#include <qdir.h>
#include <qiconset.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qwidget.h>


DialogFileSystemScanner::DialogFileSystemScanner(QWidget* parent, const char* name) :
    KDialogBase(parent, name, true, i18n("Scanning filesystem"), KDialogBase::Cancel, KDialogBase::Cancel, false),
    m_currentFolderLineEdit(0),
    m_folderCountLabel(0),
    m_fileCountLabel(0),
    m_messageCountLabel(0),
    m_tabWidget(0),
    m_folderTextEdit(0),
    m_messageTextEdit(0)
{
    QWidget* mainPanel = new QWidget(this, "mainPanel");
    setMainWidget(mainPanel);
    QVBoxLayout* mainPanelLayout = new QVBoxLayout(mainPanel, 0, 5, "mainPanelLayout");

    // append current folder labels
    QLabel* currentFolderLabel = new QLabel(i18n("Scanning folder:"), mainPanel, "currentFolderLabel");
    mainPanelLayout->add(currentFolderLabel);

    m_currentFolderLineEdit = new KLineEdit(mainPanel, "currentFolderLineEdit");
    m_currentFolderLineEdit->setReadOnly(true);
    m_currentFolderLineEdit->setMinimumWidth(500);
    m_currentFolderLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainPanelLayout->add(m_currentFolderLineEdit);

    // append counter labels
    QWidget* counterPanel = new QWidget(mainPanel, "counterPanel");
    QHBoxLayout* counterPanelLayout = new QHBoxLayout(counterPanel, 0, 5, "counterPanelLayout");
    
    QLabel* folderLabel = new QLabel(i18n("Scanned folders:"), counterPanel, "folderLabel");
    counterPanelLayout->add(folderLabel);
    m_folderCountLabel = new QLabel("0", counterPanel, "folderCountLabel");
    counterPanelLayout->add(m_folderCountLabel);

    QLabel* separatorLabel1 = new QLabel("/", counterPanel, "separatorLabel1");
    counterPanelLayout->add(separatorLabel1);    

    QLabel* fileLabel = new QLabel(i18n("Found files:"), counterPanel, "fileLabel");
    counterPanelLayout->add(fileLabel);
    m_fileCountLabel = new QLabel("0", counterPanel, "fileCountLabel");
    counterPanelLayout->add(m_fileCountLabel);

    QLabel* separatorLabel2 = new QLabel("/", counterPanel, "separatorLabel2");
    counterPanelLayout->add(separatorLabel2);

    QLabel* messageLabel = new QLabel(i18n("Found messages:"), counterPanel, "messageLabel");
    counterPanelLayout->add(messageLabel);
    m_messageCountLabel = new QLabel("0", counterPanel, "messageCountLabel");
    counterPanelLayout->add(m_messageCountLabel);
    
    counterPanelLayout->addStretch(1);

    mainPanelLayout->add(counterPanel);

    // append folder & message tabs
    m_tabWidget = new KTabWidget(mainPanel, "tabWidget");

    m_folderTextEdit = new KTextEdit(m_tabWidget, "folderTextEdit");
    m_tabWidget->addTab(m_folderTextEdit, i18n("Folder"));
    
    m_messageTextEdit = new KTextEdit(m_tabWidget, "messageTextEdit");
    m_tabWidget->addTab(m_messageTextEdit, i18n("Messages"));

    m_tabWidget->showPage(m_messageTextEdit);
    
    mainPanelLayout->add(m_tabWidget);
}


DialogFileSystemScanner::~DialogFileSystemScanner()
{}


//
// public slots
//
void DialogFileSystemScanner::slotScanningFolder(const QString& folder)
{
    m_currentFolderLineEdit->setSqueezedText(folder);
    m_folderTextEdit->append(folder);
}


void DialogFileSystemScanner::slotScanProgress(int folders, int files, int messages)
{
    m_folderCountLabel->setText(QString::number(folders));
    m_fileCountLabel->setText(QString::number(files));
    m_messageCountLabel->setText(QString::number(messages));
}


void DialogFileSystemScanner::slotFolderNotFound(const QString& foldername)
{
    m_messageTextEdit->append(QString(i18n("Folder does not exist: %1")).arg(foldername));
}


void DialogFileSystemScanner::slotLoopDetected(const QString& foundFolder, const QString& alreadyAddedFolder)
{
    m_messageTextEdit->append(QString(i18n("Loop detected: folder '%1' is already added: %2")).arg(foundFolder).arg(alreadyAddedFolder));
}


void DialogFileSystemScanner::slotFolderAlreadyAdded(const QString& foldername)
{
    m_messageTextEdit->append(QString(i18n("Folder already added: %1")).arg(foldername));
}


void DialogFileSystemScanner::slotProblemOccured(const QString& description)
{
    m_messageTextEdit->append(QString(i18n("Other problem occured: %1")).arg(description));
}


#include "dialogfilesystemscanner.moc"
