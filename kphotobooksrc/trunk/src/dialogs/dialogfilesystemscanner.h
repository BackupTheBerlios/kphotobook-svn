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

#ifndef DIALOGFILESYSTEMSCANNER_H
#define DIALOGFILESYSTEMSCANNER_H

#include <kdialogbase.h>
class KLineEdit;
class KTabWidget;
class KTextEdit;

class QCheckBox;
class QDir;


/**
 * Dialog showing the progress of scanning the filesystem
 *
 * CVS-ID $Id: dialogaddsourcedir.h 435 2005-08-02 20:36:17Z starcube $
 */
class DialogFileSystemScanner : public KDialogBase
{
    Q_OBJECT
    
    public:
        DialogFileSystemScanner(QWidget* parent = 0, const char* name = 0);
    
        ~DialogFileSystemScanner();
    
    public slots:
        void slotScanningFolder(const QString& folder);
        void slotScanProgress(int folders, int files, int messages);
        void slotFolderNotFound(const QString& foldername);
        void slotLoopDetected(const QString& foundFolder, const QString& alreadyAddedFolder);
        void slotFolderAlreadyAdded(const QString& foldername);
        void slotProblemOccured(const QString& description);
                    
    private:
        KLineEdit* m_currentFolderLineEdit;

        QLabel* m_folderCountLabel;
        QLabel* m_fileCountLabel;
        QLabel* m_messageCountLabel;

        KTabWidget* m_tabWidget;

        KTextEdit* m_folderTextEdit;
        KTextEdit* m_messageTextEdit;
};

#endif
