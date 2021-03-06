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

#include "dialogaddsourcedir.h"

#include "../settings/settings.h"

#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>

#include <qcheckbox.h>
#include <qdir.h>
#include <qiconset.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qwidget.h>


DialogAddSourceDir::DialogAddSourceDir(QWidget* parent, const char* name) :
    KDialogBase(parent, name, true, i18n("Add source folder"), KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, false)
{
    QWidget* mainPanel = new QWidget(this, "mainPanel");
    setMainWidget(mainPanel);
    QVBoxLayout* mainPanelLayout = new QVBoxLayout(mainPanel, 0, 5, "mainPanelLayout");

    // append the directory panel
    QWidget* dirPanel = new QWidget(mainPanel, "dirPanel");
    mainPanelLayout->add(dirPanel);
    QHBoxLayout* dirLayout = new QHBoxLayout(dirPanel, 0, 5, "dirPanelLayout");

    QLabel* directoryLabel = new QLabel(i18n("Folder"), dirPanel, "directoryLabel");
    dirLayout->add(directoryLabel);
    QWhatsThis::add(dirPanel, i18n("<b>Folder</b><p>Enter the path to the folder you want to add to the <i>KPhotoBook</i> database.</p>"));

    m_currentDirectoryLineEdit = new KLineEdit(dirPanel, "dirLineEdit");
    m_currentDirectoryLineEdit->setMinimumWidth(300);
    m_currentDirectoryLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    dirLayout->add(m_currentDirectoryLineEdit);

    QObject::connect(m_currentDirectoryLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotTextChanged(const QString&)));

    QIconSet folderIconSet = KGlobal::iconLoader()->loadIconSet("folder", KIcon::Small, 24, false);
    QPushButton* dirButton = new QPushButton(folderIconSet, 0, dirPanel, "dirButton");
    dirButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dirLayout->add(dirButton);

    QObject::connect(dirButton, SIGNAL(clicked()), this, SLOT(slotDirectoryButtonClicked()));

    // add checkbox
    m_recursiveCheckBox = new QCheckBox(i18n("Add folders recursively."), mainPanel, "recursiveCheckBox");
    m_recursiveCheckBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mainPanelLayout->add(m_recursiveCheckBox);
    QWhatsThis::add(m_recursiveCheckBox, i18n("<b>Add folders recursively</b><p>If enabled, <i>KPhotoBook</i> will add the folder recursively, meaning that sub folders will be added to the database as well, while not following symbolic links to avoid an infinite loop.</p>"));

    // spacer
    mainPanelLayout->addStretch(1);

    // set the directory
    m_currentDirectoryLineEdit->setText(Settings::fileSystemLastAddedSourcedir());

    // set the focus
    m_currentDirectoryLineEdit->setFocus();
}


DialogAddSourceDir::~DialogAddSourceDir()
{
    // save chosen directory only if OK is clicked
    if (result()) {
        Settings::setFileSystemLastAddedSourcedir(m_currentDirectoryLineEdit->text());
    }
}


QDir* DialogAddSourceDir::directory() const
{
    return new QDir(m_currentDirectoryLineEdit->text());
}


bool DialogAddSourceDir::recursive() const
{
    return m_recursiveCheckBox->state();
}


void DialogAddSourceDir::slotDirectoryButtonClicked()
{
    QDir dir(m_currentDirectoryLineEdit->text());

    QString choosedDir = KFileDialog::getExistingDirectory(dir.absPath(), this, "Choose the sourcedirectory to add");
    if (!choosedDir.isEmpty()) {
        m_currentDirectoryLineEdit->setText(choosedDir);
    }
}

void DialogAddSourceDir::slotTextChanged(const QString& /* text */)
{
    QDir dir(m_currentDirectoryLineEdit->text());

    this->enableButtonOK(dir.exists());
}

#include "dialogaddsourcedir.moc"
