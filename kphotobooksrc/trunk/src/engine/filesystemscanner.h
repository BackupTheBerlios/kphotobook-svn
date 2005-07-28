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
#include "../tracer/tracer.h"

#include <qdir.h>
#include <qptrlist.h>

class Engine;
class File;
class SourceDir;

/**
 * The FileSystemScanner scans all SourceFolders for files to be handled by kphotobook.
 * Found files are added to the engine and its EXIF data is stored in the database too.
 *
 * CVS-ID $Id: engine.h 315 2005-05-08 17:27:45Z starcube $
 */
class FileSystemScanner
{
    private:
        static Tracer* tracer;
    
    public:
        FileSystemScanner(Engine* engine);
        
        ~FileSystemScanner();

        /**
         * Rescans all sourcefolders for removed/added folders and files.
         * If new files are found their EXIf data is read and stored in the database.
         * The EXIF data of already added files is not reread!
         */
        void rescan();

        /**
         * Rescans all sourcefolders for removed/added folders and files.
         * The EXIF data of all files (already added or new) is read and stored in the database.
         */
        void rescanWithEXIF();
    
        /**
        * Adds the specified sourcefolder and all images in it to the engine.
        * If recursive is true, all folders below the given folder are added too.
        * If the sourcefolder cannot be added, an EngineException is thrown.
        */
        SourceDir* addSourceFolder(QDir* folder, bool recursive) throw(EngineException*);
    
        /**
        * Removes the specified sourceFolder and deletes all files and associations below this sourceFolder.
        */
        void removeSourceFolder(SourceDir* sourceFolder);

        /**
         * Tests if the given folder is addable. If it is not addable an exception with an
         * appropriate message is thrown.
         */
        void testIfFolderIsAddable(QDir* folder, bool recursive) throw(EngineException*);

    private:
        void rescanSourceFolders(QPtrList<SourceDir>* sourceDirs, bool forceEXIF);
        void rescanSourceFolder(SourceDir* sourceDir, bool forceEXIF);
        void addSourceFolders(SourceDir* parent);
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
        Engine* m_engine;
    
        /**
         * This list is used for (pre)detecting loops while adding folders recursively.
         * The list is cleared always before the sourcefolders are rescanned.
         */
        QPtrList<QString>* m_loopDetectionHelper;

};

#endif
