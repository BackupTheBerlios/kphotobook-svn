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

#include "exception.h"

#include <qfileinfo.h>
#include <qdir.h>
#include <qstring.h>

#include <qptrlist.h>
#include <qintdict.h>
#include <qdict.h>

class SourceDir;
class File;
class TagNode;

/**
 * The engine managing all sourcedirs, tags and files/images.
 *
 * CVS-ID $Id$
 */
class Engine {

public:
    friend class XmlParser;  // class XmlParser cann access everything on this class
    friend class XmlWriter;  // class XmlWriter cann access everything on this class
    friend class File;

    Engine();
    Engine(QFileInfo& fileinfo) throw(EngineException*);

    ~Engine();

    bool dirty() {
        return m_dirty;
    }

    bool alreadySavedOnce() {
        return m_name != 0;
    }

    const QFileInfo* fileinfo() {
        return m_fileinfo;
    }

    const QString* name() {
        return m_name;
    }

    void rescanSourceDirs();

    /**
     * Returns the total number of files managed by the engine.
     */
    int totalNumberOfFiles() {
        return m_fileList->count();
    }

    /**
     * Returns the number of files selected by the current filter.
     */
    int filteredNumberOfFiles() {
        return m_fileList2display->count();
    }

    /**
     * Adds the specified sourcedir and all images in it to the engine.
     * If recursive is true, all directories below the sourcedir are added too.
     * If the sourcedir cannot be added, an EngineException is thrown.
     */
    SourceDir* addSourceDir(QDir* sourceDir, bool recursive) throw(EngineException*);

    /**
     * Removes the specified sourceDir and deletes all Files and associations
     * below this sourceDir.
     */
    void removeSourceDir(SourceDir* sourceDir);

    /**
     * Returns all currently added source directories handled by this engine.
     */
    QPtrList<SourceDir>* sourceDirs() {
        return m_sourceDirs;
    }

    /**
     * Returns the sourcedir the specified file belongs to.
     */
    SourceDir* sourceDirOfFile(File* file);

    /**
     * Sets the selected flag of every sourcedir to false.
     */
    void deselectAllSourceDirs();

    /**
     * returns all possible tags of all files
     */
    QPtrList<TagNode>* tagForest() {
        return m_tagForest;
    }

    /**
     * Creates a new tag with the specified parameters.
     * If parent is not null, the new tag is a child of the specified parent.
     */
    TagNode* createTag(TagNode* parent, int type, const QString& text, const QString& iconName);

    /**
     * Updates the specified tag with the spacified name and icon.
     */
    void editTag(TagNode* tag, const QString& text, const QString& iconName);

    /**
     * Remove the specified tag and all associations linked to it.
     */
    void removeTag(TagNode* tag);

    /**
    * Returns all files matching the specified filter.
    */
    QPtrList<File>* files(QPtrList<TagNode>* filter);

    void save() throw(PersistingException*);
    void saveAs(QFileInfo& newFile) throw(PersistingException*);

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
     * The name stored in the xml file.
     */
    QString* m_name;

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

    /**
     * This list is used for (pre)detecting loops while adding directories recursively.
     * The list is cleared always before the sourcedirectories are rescanned.
     */
    QPtrList<QString>* m_loopDetectionHelper;

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

    void rescanSourceDirs(QPtrList<SourceDir>* sourceDirs);
    void rescanSourceDir(SourceDir* sourceDir);
    void addSourceDirs(SourceDir* parent);
    void testIfSourceDirIsAddable(QDir* sourceDir, bool recursive) throw(EngineException*);

    /**
     * Returns true if the directoryname does not match any of the directory
     * name stored in the configuration.
     */
    bool mustHandleDirectory(QString directoryName);

    /**
     * Returns true if the filename matches one of the file-suffixes
     * stored in the configuration.
     */
    bool mustHandleFile(QString filename);

    /**
     * Deletes all memory allocated by this object.
     */
    void cleanUp();
};

#endif
