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
#include <stdio.h>

#include <qdir.h>

#include <kapplication.h>
#include <kmessagebox.h>
#include <kio/renamedlg.h>
#include <klocale.h>
#include <kdebug.h>


ExportSymlinks::ExportSymlinks(QWidget* parent, QString destinationDir)
  : QObject(static_cast<QObject*>(parent), "ExportSymlinks")
  , m_parent(parent)
  , m_destinationDir(destinationDir)
  , m_sourceFiles(0)
  , m_progressDialog(0)
  , m_cancelling(false)
  , m_autoSkip(false)
  , m_overwriteAll(false) {
}


ExportSymlinks::~ExportSymlinks() {

  delete m_sourceFiles;
  delete m_progressDialog;
}


void ExportSymlinks::setSourceFiles(const KFileItemList* s) {

  if ( s ) {
    m_sourceFiles = new QPtrList<KFileItem>(*s);
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

    QString symlink = QString("%1/%2").arg(m_destinationDir).arg(currentFile->name());
    m_progressDialog->setLabel(symlink);
    kapp->processEvents();

    createSymlink(currentFile->url().path(), symlink);
    
    // in any case increase progress
    m_progressDialog->progressBar()->advance(1);
    
    // exit loop if cancel was called
    if (m_cancelling) {
      break;
    }
  }
  
  m_progressDialog->hide();
}


void ExportSymlinks::createSymlink(QString sourceFile, QString symlink) {

  kdDebug() << "[ExportSymlinks::createSymlink] creating symlink: " << symlink << "-->" << sourceFile << endl;
  
  // create the symbolic link
  int result = ::symlink(sourceFile.ascii(), symlink.ascii());

  // handle possible problems
  if (result != 0) {
    int errorNo = errno;
  
    kdWarning() << "[ExportSymlinks::createSymlink] could not create symlink: '" << strerror(errorNo) << "' (" << symlink << "-->" << sourceFile << ")" << endl;
    
    // an error occured
    switch ( errorNo ) {
      case EPERM: {
        kdWarning() << "[ExportSymlinks::createSymlink] the filesystem containing newpath does not support the creation of symbolic links: " << m_destinationDir << endl;
        
        QString msg = QString(i18n("The filesystem containing the destination directory does not support symbolic links:\n%1")).arg(m_destinationDir);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      case EFAULT: {
        kdWarning() << "[ExportSymlinks::createSymlink] oldpath or newpath points outside your accessible address space: " << symlink << "-->" << sourceFile << endl;
        
        QString msg = QString(i18n("Sourcefile or destinationfile points outside your accessible address space:\nSource: %1\nDestination: %2")).arg(sourceFile).arg(symlink);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      case EACCES: {
        kdWarning() << "[ExportSymlinks::createSymlink] write access to the directory containing newpath is not allowed for the process's effective uid, or one of the directories in newpath did not allow search (execute) permission: " << m_destinationDir << endl;
        
        QString msg = QString(i18n("Can not write to the specified destination directory:\n%1\nPlease check that you have write permission.")).arg(m_destinationDir);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      case ENAMETOOLONG: {
        kdWarning() << "[ExportSymlinks::createSymlink] oldpath or newpath was too long: " << symlink << "-->" << sourceFile << endl;
        
        QString msg = QString(i18n("The Sourcefilename or destinationfilename is too long:\nSource: %1\nDestination: %2")).arg(sourceFile).arg(symlink);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      case ENOENT: {
        kdWarning() << "[ExportSymlinks::createSymlink] a directory component in newpath does not exist or is a dangling symbolic link: " << m_destinationDir << endl;
        
        QString msg = QString(i18n("The destination directory is invalid:\n%1")).arg(m_destinationDir);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      case ENOTDIR: {
        kdWarning() << "[ExportSymlinks::createSymlink] a component used as a directory in newpath is not, in fact, a directory: " << m_destinationDir << endl;
        
        QString msg = QString(i18n("The destination directory is invalid:\n%1")).arg(m_destinationDir);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      case ENOMEM: {
        kdWarning() << "[ExportSymlinks::createSymlink] insufficient kernel memory was available." << endl;
        
        QString msg = QString(i18n("Insufficient kernel memory was available."));
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      case EROFS: {
        kdWarning() << "[ExportSymlinks::createSymlink] newpath is on a read-only filesystem: " << m_destinationDir << endl;
        
        QString msg = QString(i18n("The destination directory is on a read only filesystem:\n%1")).arg(m_destinationDir);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      case EEXIST: {
        kdDebug() << "[ExportSymlinks::createSymlink] newpath already exists: " << symlink << endl;

        if (m_autoSkip) {
          kdDebug() << "[ExportSymlinks::createSymlink] autoskip is enabled. proceeding with next file." << endl;
        } else if (m_overwriteAll) {
          kdDebug() << "[ExportSymlinks::createSymlink] overwrite all is enabled. deleting existing file." << endl;
          
          if(::remove(symlink.ascii()) == 0) {
            // already existing file successfully removed
            createSymlink(sourceFile, symlink);
          } else {
            // could not delete existing file
            kdWarning() << "[ExportSymlinks::createSymlink] could not delete file: '" << strerror(errno) << "' (" << symlink << ")" << endl;
            
            QString msgOverwriteAllDisabled = QString(i18n("\nFunction 'Overwrite all' disabled automatically!"));
            QString msg = QString(i18n("Could not overwrite the destination file:\n%1%2")).arg(symlink).arg(msgOverwriteAllDisabled);
            KMessageBox::sorry(m_parent, msg, i18n("Could not overwrite"));
            
            // reset overwrite all
            m_overwriteAll = false;
            
            createSymlink(sourceFile, symlink);
          }
        } else {
        
          KIO::RenameDlg* renameDlg = new KIO::RenameDlg(
            m_parent,                       // parent
            i18n("File already exists"),    // caption
            sourceFile,                     // source
            symlink,                        // destination
            (KIO::RenameDlg_Mode)(KIO::M_OVERWRITE | KIO::M_SKIP | KIO::M_MULTI)  // mode
          );
          renameDlg->setModal(true);
          int result = renameDlg->exec();
          renameDlg->hide();
          
          // analyze the choosen action
          switch (result) {
            case KIO::R_CANCEL : {
              m_cancelling = true;
              break;
            }
            
            case KIO::R_RENAME : {
              createSymlink(sourceFile, renameDlg->newDestURL().path().ascii());
              
              break;
            }

            case KIO::R_AUTO_SKIP : {
              m_autoSkip = true;
              m_overwriteAll = false;
            }
            case KIO::R_SKIP : {
              // nothing to do... just skip
              break;
            }
            
            case KIO::R_OVERWRITE_ALL : {
              m_overwriteAll = true;
              m_autoSkip = false;
            }
            case KIO::R_OVERWRITE : {
              if(::remove(symlink.ascii()) == 0) {
                // already existing file successfully removed
                createSymlink(sourceFile, symlink);
              } else {
                // could not delete existing file
                kdWarning() << "[ExportSymlinks::createSymlink] could not delete file: '" << strerror(errno) << "' (" << symlink << ")" << endl;
                
                QString msgOverwriteAllDisabled = QString::null;
                if (m_overwriteAll) {
                  msgOverwriteAllDisabled = QString(i18n("\nFunction 'Overwrite all' disabled automatically!"));
                }
                
                QString msg = QString(i18n("Could not overwrite the destination file:\n%1%2")).arg(symlink).arg(msgOverwriteAllDisabled);
                KMessageBox::sorry(m_parent, msg, i18n("Could not overwrite"));
            
                // reset overwrite all
                m_overwriteAll = false;
                
                createSymlink(sourceFile, symlink);
              }
              break;
            }
          }
          
          delete renameDlg;
        }
        break;
      }
      
      case ELOOP: {
        kdWarning() << "[ExportSymlinks::createSymlink] too many symbolic links were encountered in resolving newpath: " << m_destinationDir << endl;
        
        QString msg = QString(i18n("The destination path contains too many symbolic links:\n%1")).arg(m_destinationDir);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      case ENOSPC: {
        kdWarning() << "[ExportSymlinks::createSymlink] the device containing the file has no room for the new directory entry: " << m_destinationDir << endl;
        
        QString msg = QString(i18n("The device for the destination directory is full:\n%1")).arg(m_destinationDir);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      case EIO: {
        kdWarning() << "[ExportSymlinks::createSymlink] an I/O error occurred: " << symlink << "-->" << sourceFile << endl;
        
        QString msg = QString(i18n("An I/O error occured:\nSource: %1\nDestination: %2")).arg(sourceFile).arg(symlink);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
      
      default: {
        kdWarning() << "[ExportSymlinks::createSymlink] an unexpected error number catched (" << errorNo << "): " << symlink << "-->" << sourceFile << endl;
        
        QString msg = QString(i18n("An unexpected error occured while creating symbolic link:\nSource: %1\nDestination: %2")).arg(sourceFile).arg(symlink);
        KMessageBox::sorry(m_parent, msg, i18n("Could not export"));
        m_cancelling = true;
        break;
      }
    }
  }
}



#include "exportsymlinks.moc"
