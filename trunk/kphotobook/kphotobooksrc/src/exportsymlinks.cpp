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

#include "exportsymlinks.h"

#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <qdir.h>

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>


ExportSymlinks::ExportSymlinks(QWidget* parent, QString destinationDir)
  : QObject(static_cast<QObject*>(parent), "ExportSymlinks")
  , m_parent(parent)
  , m_destinationDir(destinationDir)
  , m_sourceFiles(0)
  , m_progressDialog(0)
  , m_cancelling(false) {
}


ExportSymlinks::~ExportSymlinks() {

  delete m_sourceFiles;
  delete m_progressDialog;
}


void ExportSymlinks::setSourceFiles(const KFileItemList* sourceFiles) {

  if ( sourceFiles ) {
    m_sourceFiles = new QPtrList<KFileItem>(*sourceFiles);
  }
}


void ExportSymlinks::execute() {

  // test the destination directory
  if (m_destinationDir.isEmpty()) {
    kdError() << "[ExportSymlinks::start] the specified 'destinationDir' is null or empty!" << endl;
    return;
  }
  QDir dir(m_destinationDir);
  if (!dir.exists()) {
    kdError() << "[ExportSymlinks::start] the specified 'destinationDir' is null or empty!" << endl;
    return;
  }

  // test the files to export
  if (!m_sourceFiles || m_sourceFiles->isEmpty()) {
    kdWarning() << "[ExportSymlinks::start] no files to export!" << endl;
    return;
  }
  
  // setup progress dialog
  m_progressDialog = new KProgressDialog(m_parent, "exportSymlinksProgressDialog", i18n("Exporting symbolic links"), i18n("Initializing export..."), true);
  m_progressDialog->progressBar()->setTotalSteps(m_sourceFiles->count());
  m_progressDialog->setAllowCancel(true);
  connect(m_progressDialog, SIGNAL(cancelClicked()), this, SLOT(slotCancel()));
  m_progressDialog->show();
  
  // do the job
  doExport();
}


//
// public slots
//
void ExportSymlinks::slotCancel() {

  m_cancelling = true;
}


//
// private methods
//
void ExportSymlinks::doExport() {

  KFileItem * currentFile;
  for ( currentFile = m_sourceFiles->first(); currentFile; currentFile = m_sourceFiles->next() ) {
    kdDebug() << "[ExportSymlinks::doExport] creating symlink: " << m_destinationDir << "/" << currentFile->name() << "-->" << currentFile->url().path() << endl;

    QString symlink = QString("%1/%2").arg(m_destinationDir).arg(currentFile->name());
    m_progressDialog->setLabel(symlink);
    kapp->processEvents();

    int result = ::symlink(currentFile->url().path().ascii(), symlink.ascii());
    if (result != 0) {
      int errorNo = errno;
    
      kdWarning() << "[ExportSymlinks::doExport] could not create symlink: '" << strerror(errorNo) << "' (" << m_destinationDir << "/" << currentFile->name() << "-->" << currentFile->url().path() << ")" << endl;
      
      // an error occured
      switch ( errorNo ) {
      case EACCES:
        kdWarning() << "[ExportSymlinks::doExport] can not write to destination directory: " << m_destinationDir << endl;
        break;
      }
    }
    
    // in any case increase progress
    m_progressDialog->progressBar()->advance(1);
    
    // exit loop if cancel was called
    if (m_cancelling) {
      break;
    }
  }
  
  m_progressDialog->hide();
}



#include "exportsymlinks.moc"
