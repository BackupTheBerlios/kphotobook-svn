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

#ifndef EXPORTSYMLINKS_H
#define EXPORTSYMLINKS_H

#include <qobject.h>
#include <qwidget.h>
#include <qthread.h>

#include <qstring.h>

#include <kfileitem.h>
#include <kprogress.h>


/**
 * This class exports images as symlinks.
 * It creates symbolic links in the set destination directory to the
 * specified images.
 *
 * CVS-ID $Id: exportsymlinks.h,v 1.2 2004/06/13 15:01:48 starcube Exp $
*/
class ExportSymlinks : public QObject { //, public QThread {

  Q_OBJECT

public:
  /**
   * Creates a new instance prepared to export to the specified directory.
   * If the specified destinationDir is invalid nothing is done here, the
   * start() method will handle this.
   *
   * @param parent The parent, used for displaying dialogs.
   * @param destinationDir The directory to export to.
   */
  ExportSymlinks(QWidget* parent, QString destinationDir);

  ~ExportSymlinks();

  /**
   * Sets the sourceFiles to create symbolic links to.
   * If the specified list is 0 or empty, the start method will do nothing.
   *
   * @param sourceFiles The files to create symlinks to.T
   */
  void setSourceFiles(const KFileItemList* sourceFiles);

  /**
   * Starts exporting the files.
   * This class handles all possible problems by asking the user what to do.
   * If the destinationDir specified in the constructor is invalid, only 
   * a message is traced and the method is leaved. It is up to the caller
   * to ensure that the destinationDir is valid.
   */
  void execute();


public slots:
  /**
   * Stops creating (exporting) symbolic links.
   * Already written links are not removed!
   */
  void slotCancel();


private:
  /**
   * Just for keeping the start method a bit smaller.
   */
  void doExport();
  
  /**
   * Does the actual job of creating the symbolic link.
   * Also all needed user interaction (rename/skip/autoskip/overwrite,...)
   * is done within this method.
   */
  void createSymlink(QString sourceFile, QString symlink);


private:
  QWidget* m_parent;
  QString m_destinationDir;

  QPtrList<KFileItem>* m_sourceFiles;

  KProgressDialog* m_progressDialog;
  
  bool m_cancelling;
  bool m_autoSkip;
  bool m_overwriteAll;
};

#endif
