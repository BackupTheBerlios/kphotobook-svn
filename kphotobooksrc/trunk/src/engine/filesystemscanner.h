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

#ifndef _FILESYSTEMSCANNER_H_
#define _FILESYSTEMSCANNER_H_

#include "../exception.h"
class Engine;
class File;
class Folder;
class Tracer;

#include <qobject.h>
#include <qstring.h>
class QDateTime;
class QDir;
template<class type> class QPtrList;


/**
 * The FileSystemScanner scans all folders for files to be handled by kphotobook.
 * Found files are added to the engine and its EXIF data is stored in the database too.
 *
 * CVS-ID $Id: engine.h 315 2005-05-08 17:27:45Z starcube $
 */
class FileSystemScanner : public QObject
{
    Q_OBJECT

    private:
        static Tracer* tracer;

    public:
        FileSystemScanner(Engine* engine);

        ~FileSystemScanner( );

        /**
         * Only checks if all folders currently in the database still exists.
         * New folders and files are not found!
         * The EXIF data of already added files is not reread!
         */
        void rescanFast();

        /**
         * Rescans all folders for removed/added folders and files.
         * If new files are found their EXIf data is read and stored in the database.
         * The EXIF data of already added files is not reread!
         */
        void rescan();

        /**
         * Rescans all folders for removed/added folders and files.
         * The EXIF data of all files (already added or new) is read and stored in the database.
         */
        void rescanWithEXIF();

        /**
         * Adds the specified folder and all images in it to the engine.
         * If recursive is true, all folders below the given folder are added too.
         * If the folder cannot be added, an EngineException is thrown.
         */
        Folder* addFolder(QDir* dir, bool recursive) throw(EngineException*);

        /**
         * Removes the specified folder and deletes all files and associations below this folder.
         */
        void removeFolder(Folder* folder);

        /**
         * Tests if the given folder is addable. If it is not addable an exception with an
         * appropriate message is thrown.
         */
        void testIfFolderIsAddable(QDir* dir, bool recursive) const throw(EngineException*);

    public slots:
        void slotCancel();

    signals:
        void newFolder(Folder*);

        void progress(int folders, int files, int problems);
        void progress_scanningFolder(const QString& foldername);
        void progress_folderNotFound(const QString& foldername);
        void progress_loopDetected(const QString& foundFolder, const QString& alreadyAddedFolder);
        void progress_folderAlreadyAdded(const QString& folderName);
        void progress_problemOccured(const QString& description);

    private:
        void reset();
        void rescanFolders(QPtrList<Folder>* folders, bool forceEXIF, bool fast = false);
        void rescanFolder(Folder* folder, bool forceEXIF);
        void addFolders(Folder* parent);
        void readEXIF(File* file);
        QDateTime readExifDateTime(const QString& data);

        /**
         * Returns true if the folderName does not match any of the folders to exclude
         * stored in the configuration.
         */
        bool mustHandleFolder(QString folderName);

        /**
         * Returns true if the filename matches one of the file-patterns stored in the configuration.
         */
        bool mustHandleFile(QString filename);

    private:
        /**
         * Link to the engine this scanner is 'working' for.
         */
        Engine* m_engine;

        /**
         * This list is used for (pre)detecting loops while adding folders recursively.
         * The list is cleared always before the folders are rescanned.
         */
        QPtrList<QString>* m_loopDetectionHelper;

        /**
         * This field will be set to true by the slot @link slotCancel(). The scanning
         * must abort as soon as possible after this member was set to true.
         */
        bool m_cancel;

        int m_folders;
        int m_files;
        int m_problems;
};

#endif
