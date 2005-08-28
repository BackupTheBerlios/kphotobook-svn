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

#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "filesystemscanner.h"
#include "filternode.h"
#include "tagnode.h"
#include "tagnodetitle.h"
#include "../tracer/tracer.h"
#include "../exception.h"

#include <qobject.h>
#include <qdict.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qintdict.h>
#include <qptrlist.h>
#include <qstring.h>

class SourceDir;
class File;
class TagNode;


/**
 * The engine managing all sourcedirs, tags and files/images.
 *
 * CVS-ID $Id$
 */
class Engine : public QObject
{
    Q_OBJECT
    
    private:
        static Tracer* tracer;
    
    public:
        friend class File;              // class File cann access everything on this class
        friend class FileSystemScanner; // class FileSystemScanner cann access everything on this class
        friend class XmlParser;         // class XmlParser cann access everything on this class
        friend class XmlWriter;         // class XmlWriter cann access everything on this class
    
        Engine();
        Engine(QFileInfo& fileinfo) throw(EngineException*);
    
        ~Engine();
    
        /**
        * Returns the FileSystemScanner for this engine.
        */
        FileSystemScanner* fileSystemScanner()
        {
            return m_fileSystemScanner;
        }
    
        void dirtyfy()
        {
            m_dirty = true;
        }
        bool dirty()
        {
            return m_dirty;
        }
    
        /**
        * Returns true if the current database has been saved already at least once.
        */
        bool alreadySavedOnce()
        {
            return m_uid != 0;
        }
    
        const QFileInfo* fileinfo()
        {
            return m_fileinfo;
        }
    
        /**
        * Returns the current opened file or an empty string if the
        * was never saved.
        */
        QString currentURL()
        {
            return m_fileinfo ? m_fileinfo->absFilePath() : "";
        }
        
        const QString* uid()
        {
            return m_uid;
        }
    
        /**
        * Returns the total number of files managed by the engine.
        */
        int totalNumberOfFiles()
        {
            return m_fileList->count();
        }
    
        /**
        * Returns the number of files selected by the last specified filter.
        */
        int filteredNumberOfFiles()
        {
            return m_fileList2display->count();
        }
    
        /**
        * Returns all currently added source directories handled by this engine.
        */
        QPtrList<SourceDir>* sourceDirs()
        {
            return m_sourceDirs;
        }
    
        /**
        * returns all possible tags of all files
        */
        QPtrList<TagNode>* tagForest()
        {
            return m_tagForest;
        }
    
        /**
        * Creates a new tag with the specified parameters.
        * If parent is not null, the new tag is a child of the specified parent.
        */
        TagNode* createTag(TagNode* parent, TagNode::Type type, const QString& text, const QString& comment, const QString& iconName);
    
        /**
        * Updates the specified tag with the spacified name and icon.
        */
        void editTag(TagNode* tag, const QString& text, const QString& comment, const QString& iconName);
    
        /**
        * Remove the specified tag and all associations linked to it.
        */
        void removeTag(TagNode* tag);
    
        /**
        * Tests if the specified tagtext is valid. There must not exist a sibling
        * with the same name. And a maintag must not be named 'EXIF'.
        * If the parent is null, it is considered to be a maintag.
        *
        * @param ignoreExifTag If true it is not tested if the specified tag is named 'EXIF'.
        */
        bool isTagTextValid(TagNode* parent, QString& text, bool ignoreExifTag = false);
    
        /**
        * Returns the tagnode representing the EXIF title.
        */
        TagNodeTitle* exifTagNodeTitle()
        {
            return m_exifTitleTag;
        }    
        
        /**
        * Returns all files matching the specified filter.
        */
        QPtrList<File>* files(FilterNode* filterRootNode);
    
        void save() throw(PersistingException*);
        void saveAs(QFileInfo& newFile) throw(PersistingException*);

    signals:
        void newTagNode(TagNode*);
    
    private:
        /**
        * Indicates if the engine is dirty, i.e. that data has changed
        * and it should be saved.
        */
        bool m_dirty;
    
        /**
        * The url of the opened xml-file.
        */
        QFileInfo* m_fileinfo;
    
        /**
        * A unique identifier for this file.
        */
        QString* m_uid;
    
        /**
        * The filesystemscanner for this engine.
        */
        FileSystemScanner* m_fileSystemScanner;
    
        //
        // sourcedir members
        //
        /**
        * Contains the next free id for a new sourcedirectory.
        */
        unsigned int m_nextSourceDirId;
    
        /**
        * This dictionary contains the id of the sourcedir as key and
        * the sourceDir-object as value.
        */
        QIntDict<SourceDir>* m_sourceDirDict;
    
        /**
        * This list contains all sourcedirectories.
        */
        QPtrList<SourceDir>* m_sourceDirs;
    
        //
        // tagnode members
        //
        /**
        * Contains the next free id for a new tagnode.
        */
        unsigned int m_nextTagNodeId;
    
        /**
        * This dictionary contains the id of the tagNode as key and
        * the tagNode-object as value.
        */
        QIntDict<TagNode>* m_tagNodeDict;
    
        /**
        * A list containing all toplevel tagNodes.
        */
        QPtrList<TagNode>* m_tagForest;
    
        /**
        * Pointer to the exif title tag.
        */
        TagNodeTitle* m_exifTitleTag;
    
        //
        // file members
        //
        /**
        * This dictonary contains the absolute path of the file as
        * key and the file-object as value.
        */
        QDict<File>* m_fileDict;
    
        /**
        * A list of all files below ALL sourcedirectories.
        */
        QPtrList<File>* m_fileList;
    
        /**
        * A list of all files to be displayed.
        */
        QPtrList<File>* m_fileList2display;
    
    private:
        /**
        * Deletes all memory allocated by this object.
        */
        void cleanUp();
    
        /**
        * Generates a unique identifier.
        * We simply return the seconds since 1970.
        */
        QString* generateUid();
    
        void createExifTagNode();
};

#endif
