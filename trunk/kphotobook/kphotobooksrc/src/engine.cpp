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

#include "engine.h"

#include "constants.h"
#include "configuration.h"
#include "xmlparser.h"
#include "xmlwriter.h"
#include "sourcedir.h"
#include "tagnode.h"
#include "tagnodetitle.h"
#include "tagnodeboolean.h"
#include "filetagnodeassoc.h"
#include "filetagnodeassocboolean.h"
#include "file.h"

#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qxml.h>
#include <qdatetime.h>
#include <qintdict.h>
#include <qptrlist.h>
#include <qregexp.h>


Engine::Engine()
    : m_dirty(true)
    , m_fileinfo(0)
    , m_name(0)
    , m_nextSourceDirId(1)
    , m_nextTagNodeId(1) {

    kdDebug() << "[Engine::Engine] instantiating empty engine" << endl;

    // initialize the variables
    m_sourceDirs = new QPtrList<SourceDir>();
    m_sourceDirDict = new QIntDict<SourceDir>();
    m_loopDetectionHelper = 0;

    m_tagForest = new QPtrList<TagNode>();
    m_tagNodeDict = new QIntDict<TagNode>();

    // initialize the list of all files
    m_fileDict = new QDict<File>();
    m_fileList = new QPtrList<File>();
    m_fileList2display = new QPtrList<File>();
}


Engine::Engine(QFileInfo& fileinfo) throw(EngineException*)
    : m_dirty(false)
    , m_fileinfo(new QFileInfo(fileinfo))
    , m_name(0)
    , m_nextSourceDirId(1)
    , m_nextTagNodeId(1) {

    kdDebug() << "[Engine::Engine] instantiating engine with file '" << m_fileinfo->absFilePath() << "'" << endl;

    // if file does not exist, we have nothing to do
    if (!m_fileinfo->exists()) {
        QString msg = QString("File '%1' does not exist.").arg(m_fileinfo->absFilePath());
        kdError() << msg << endl;
        throw new EngineException(msg, "no detailmessage");
    }

    QFile file(m_fileinfo->absFilePath());

    // close the file if it is open already
    if (file.isOpen()) {
        file.close();
    }

    // open the file for reading
    if (!file.open( IO_ReadOnly )) {
        QString msg = QString("Could not open file '%1'.").arg(file.name());
        kdError() << msg << ": " << file.errorString() << endl;
        throw new EngineException(msg, file.errorString());
    }

    // initialize the variables
    m_sourceDirs = new QPtrList<SourceDir>();
    m_sourceDirDict = new QIntDict<SourceDir>();
    m_loopDetectionHelper = 0;

    m_tagForest = new QPtrList<TagNode>();
    m_tagNodeDict = new QIntDict<TagNode>();

    m_fileDict = new QDict<File>();
    m_fileList = new QPtrList<File>();
    m_fileList2display = new QPtrList<File>();

    // prepare input source
    QXmlInputSource xmlInputSource(file);

    // instantiate the parser
    XmlParser parser = XmlParser(this);

    // prepare the xml reader
    QXmlSimpleReader reader;

    // set the handler on the reader
    reader.setContentHandler(&parser);
    reader.setErrorHandler(&parser);

    // parse the document
    bool success = reader.parse(&xmlInputSource, false);

    // close the file
    file.close();

    if (!success) {
        kdError() << "[Engine::Engine] error occured during parsing the file '" << file.name() << "'" << endl;

        cleanUp();
        if (parser.exception()) {
            kdError() << "[Engine::Engine] exception occured: " << parser.exception()->message() << endl;

            throw parser.exception();
        } else {
            throw new EngineException(i18n("Unknown error while parsing the xml file occured!"));
        }
    }

    // read the files in all sourcedirectories
    rescanSourceDirs(m_sourceDirs);

    // trace a little
    kdDebug() << "[Engine::Engine] " << m_fileList->count() << " images added" << endl;
}


Engine::~Engine() {

    cleanUp();
}

void Engine::cleanUp() {

    delete m_fileinfo;
    delete m_name;

    //
    // sourcedir members
    //
    delete m_sourceDirDict;
    delete m_sourceDirs;
    delete m_loopDetectionHelper;

    //
    // tagnode members
    //
    delete m_tagNodeDict;
    delete m_tagForest;

    //
    // file members
    //
    delete m_fileList;
    delete m_fileDict;
}


void Engine::rescanSourceDirs() {

    kdDebug() << "[Engine::rescanSourceDirs] scanning..." << endl;

    rescanSourceDirs(m_sourceDirs);
}


SourceDir* Engine::addSourceDir(QDir* newSourceDir, bool recursive) throw(EngineException*) {

    // test if sourcedir is addable
    testIfSourceDirIsAddable(newSourceDir, recursive);

    kdDebug() << "[Engine::addSourceDir] adding sourcedir: " << newSourceDir->absPath() << ", recursive: " << recursive << "..." << endl;

    m_dirty = true;

    SourceDir* sourceDir = new SourceDir(m_nextSourceDirId++, newSourceDir, recursive);
    sourceDir->setFound(true);
    m_sourceDirs->append(sourceDir);
    m_sourceDirDict->insert(sourceDir->id(), sourceDir);

    // add all files in the sourcedir
    rescanSourceDir(sourceDir);

    // add all directories below the newSourceDir
    if (recursive) {
        delete m_loopDetectionHelper;
        m_loopDetectionHelper = new QPtrList<QString>;
        m_loopDetectionHelper->setAutoDelete(true);
        m_loopDetectionHelper->append(new QString(sourceDir->dir()->canonicalPath()));

        addSourceDirs(sourceDir);
    }

    return sourceDir;
}


void Engine::removeSourceDir(SourceDir* sourceDir) {

    kdDebug() << "[Engine::removeSourceDir] removing sourcedir: " << sourceDir->dir()->absPath() << "..." << endl;

    m_dirty = true;

    // call this method recursively for each child
    if (sourceDir->children() && sourceDir->children()->count()) {
        // remove the subdirectories

        // we have to work with a copy of the children list because the destructor of
        // the sourceDir is changing the sourcedir-tree
        QPtrList<SourceDir> temp(*sourceDir->children());

        SourceDir* child;
        for ( child = temp.first(); child; child = temp.next() ) {
            removeSourceDir(child);
        }
    }

    // remove the specified sourcedir
    m_sourceDirs->remove(sourceDir);
    m_sourceDirDict->remove(sourceDir->id());
    delete sourceDir;
}


TagNode* Engine::createTag(TagNode* parent, int type, const QString& text, const QString& iconName) {

    kdDebug() << "[Engine::createTag] creating tag with type: " << type << ", text: " << text << ", icon: " << iconName << endl;

    TagNode* tagNode = TagNode::createInstance(type, m_nextTagNodeId++, text, iconName, parent);

    // put the tagNode into the tree
    if (!parent) {
        m_tagForest->append(tagNode);
    }

    // put the new tagnode into the tagnode dictionary
    m_tagNodeDict->insert(tagNode->id(), tagNode);

    m_dirty = true;

    return tagNode;
}

void Engine::editTag(TagNode* tag, const QString& text, const QString& iconName) {

    tag->setText(text);
    tag->setIconName(iconName);

    m_dirty = true;
}


void Engine::removeTag(TagNode* tag) {

    kdDebug() << "[Engine::removeTag] removing tagnode: " << tag->text() << "..." << endl;

    m_dirty = true;

    // call this method recursively for each child
    if (tag->children() && tag->children()->count()) {
        // remove the children

        // we have to work with a copy of the children list because the destructor of
        // the tagNode is changing the tagNode-tree
        QPtrList<TagNode> temp(*tag->children());

        TagNode* child;
        for ( child = temp.first(); child; child = temp.next() ) {
            removeTag(child);
        }
    }

    // remove the specified tag from the tagforest if it is a toplevel tag
    if (!tag->parent()) {
        m_tagForest->remove(tag);
    }
    m_tagNodeDict->remove(tag->id());

    delete tag;
}


QPtrList<File>* Engine::files(QString filter) {

    kdDebug() << "[Engine::files] invoked with filter '" << filter << "'"<< endl;

    // prepare filter
    QStringList filterItems = QStringList::split(QRegExp("\\&|\\|"), filter, false);

    // determine operator
    bool andOperator = false;
    if (filter.find("&") >= 0) {
        andOperator = true;
    }

    // remove all files currently shown
    m_fileList2display->clear();

    // loop over all source directories
    QIntDictIterator<SourceDir> sourceDirIt(*m_sourceDirDict);
    for ( ; sourceDirIt.current(); ++sourceDirIt ) {
        SourceDir* sourceDir = sourceDirIt.current();

        if (sourceDir->include()) {
            QPtrList<File>* fileList = sourceDir->files();

            File* file;
            for (file = fileList->first(); file; file = fileList->next()) {

                bool match = andOperator;

                // iterate over filterItems and validate
                for (QStringList::Iterator it = filterItems.begin(); it != filterItems.end(); ++it) {
                    QString filterItem(*it);

                    // test for ! and remove it
                    bool negate = false;
                    if (filterItem.startsWith("!")) {
                        negate = true;
                        filterItem = filterItem.mid(1);
                    }

                    // convert string to int
                    bool ok;
                    int tagId = filterItem.toInt(&ok);
                    if (!ok) {
                        kdWarning() << "FilterItem is not an integer: " << filterItem << endl;
                        continue;
                    }

                    // get the tagnode with the found tagId
                    TagNode* tagNode = m_tagNodeDict->find(tagId);
                    if (!tagNode) {
                        kdWarning() << "TagNode with id " << tagId << " does not exist!" << endl;
                        continue;
                    }

                    // get the tagnodeassoc to the found tagnode
                    FileTagNodeAssoc* tagNodeAssoc = file->getAssoc(tagNode);

                    bool isTagged = false;

                    // test if the file is tagged with the current tag
                    if (tagNodeAssoc) {
                        if (typeid(*tagNodeAssoc) == typeid(FileTagNodeAssocBoolean)) {
                            FileTagNodeAssocBoolean* tagNodeAssocBoolean = dynamic_cast<FileTagNodeAssocBoolean*>(tagNodeAssoc);
                            isTagged = tagNodeAssocBoolean->value();
                        }
                    }

                    if (andOperator) {
                        if (negate == isTagged) {
                            match = false;
                            break;
                        }
                    } else {
                        if (negate != isTagged) {
                            match = true;
                            break;
                        }
                    }
                }

                // append the file if it matches the filter
                if (filter.isNull() || match) {
                    m_fileList2display->append(file);
                }
            }
        }
    }

    return m_fileList2display;
}


void Engine::save() throw(PersistingException*) {

    kdDebug() << "[Engine::save] invoked..." << endl;

    if (m_dirty) {
        kdDebug() << "[Engine::save] engine is dirty --> saving it..." << endl;

        // rename current file
        QDir path = m_fileinfo->dirPath(true);
        QString oldFileName = m_fileinfo->fileName();

        // create new filename
        QString basename = QFileInfo(oldFileName).baseName(true);
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmsszzz");
        QString newFileName = QString("%1-%2.%3").arg(basename).arg(timestamp).arg(Constants::FILE_EXTENSION);

        kdDebug() << "[Engine::save] backing up file in directory '" << path.absPath() << "': '" << oldFileName << "' --> '" << newFileName << "'..." << endl;

        if (!path.rename(oldFileName, newFileName, false)) {
            QString msg = QString("Could not move old file '%1/%2' to '%3/%4'. Not saving file. Use 'Save As'.").arg(path.absPath()).arg(oldFileName).arg(path.absPath()).arg(newFileName);
            throw new PersistingException(
                msg,
                ""
            );
        }

        // save
        XmlWriter writer = XmlWriter(this);
        writer.store(new QFile(m_fileinfo->absFilePath()));
    }

    m_dirty = false;
}


void Engine::saveAs(QFileInfo& newFile) throw(PersistingException*) {

    m_fileinfo = new QFileInfo(newFile);
    m_name = new QString(newFile.fileName());

    // save
    XmlWriter writer = XmlWriter(this);
    writer.store(new QFile(m_fileinfo->absFilePath()));

    m_dirty = false;
}


//
// private methods
//
void Engine::rescanSourceDirs(QPtrList<SourceDir>* sourceDirs) {

    SourceDir* sourceDir = 0;
    for (sourceDir = sourceDirs->first(); sourceDir; sourceDir = sourceDirs->next()) {
        kdDebug() << "[Engine::rescanSourceDirs] rescanning sourcedir: " << sourceDir->id() << ": " << sourceDir->dir()->absPath() << endl;

        sourceDir->setFound(false);

        if (sourceDir->dir()->exists()) {

            sourceDir->setFound(true);

            rescanSourceDir(sourceDir);

            if (sourceDir->children() && sourceDir->children()->count() > 0) {
                rescanSourceDirs(sourceDir->children());
            }
        } else {
            kdDebug() << "[Engine::rescanSourceDirs] sourcedir: " << sourceDir->id() << ": '" << sourceDir->dir()->absPath() << "' not found" << endl;
        }

        // scan the filesystem for new sourcedirs
        if (sourceDir->recursive()) {
            delete m_loopDetectionHelper;
            m_loopDetectionHelper = new QPtrList<QString>;
            m_loopDetectionHelper->setAutoDelete(true);
            m_loopDetectionHelper->append(new QString(sourceDir->dir()->canonicalPath()));

            addSourceDirs(sourceDir);
        }
    }
}


void Engine::rescanSourceDir(SourceDir* sourceDir) {

    kdDebug() << "[Engine::rescanSourceDir] adding files in sourcedir: " << sourceDir->id() << ": " << sourceDir->dir()->absPath() << endl;

    // TODO: setFound(false) on all files in the sourcedir!

    if (sourceDir->dir()->exists()) {
        // get a list with all files in the current sourcedir
        const QFileInfoList* filelist = sourceDir->dir()->entryInfoList(QDir::Files);

        if (filelist) {
            QFileInfoListIterator iterator( *filelist );
            QFileInfo* fileInfo;
            while ((fileInfo = iterator.current()) != 0) {

                // only add jpeg files
                if (mustHandleFile(fileInfo->fileName())) {

                    File* file = m_fileDict->find(fileInfo->absFilePath());
                    if (!file) {
                        // the file is seen for the first time --> create it
                        kdDebug() << "[Engine::rescanSourceDir] found new file to add: '" << fileInfo->absFilePath() << "'" << endl;
                        file = new File(this, sourceDir, new QFileInfo(*fileInfo));
                        m_dirty = true;

                        sourceDir->appendFile(file);
                        m_fileList->append(file);
                        m_fileDict->insert(file->fileInfo()->absFilePath(), file);
                    }
                }
                ++iterator;
            }
        }
    } else {
        kdWarning() << "[Engine::rescanSourceDir] sourcedir: " << sourceDir->id() << ": " << sourceDir->dir()->absPath() << " does no longer exist --> ignoring it!!!" << endl;
    }
}


void Engine::testIfSourceDirIsAddable(QDir* newDir, bool recursive) throw(EngineException*) {

    kdDebug() << "[Engine::testIfSourceDirIsAddable] is sourcedir addable " << newDir->absPath() << ", recursive: " << recursive << "..." << endl;

    // test if the chosen sourcedir is already added
    SourceDir* sourceDir;
    for ( sourceDir = sourceDirs()->first(); sourceDir; sourceDir = sourceDirs()->next() ) {

        if (*(sourceDir->dir()) == *newDir) {
            QString detailMsg = QString(i18n("The directory you have choosen is already added to KPhotoBook.\nDirectory: %1")).arg(sourceDir->dir()->absPath());
            throw new EngineException(
                i18n("You cannot add the same directory more than once to KPhotoBook."),
                detailMsg
            );
        }

        QString sourceDirPath = sourceDir->dir()->absPath();
        QString newDirPath = newDir->absPath();

        if (sourceDir->recursive()) {
            // test if the new dir is a subdirectory of the current sourcedir
            if (newDirPath.startsWith(sourceDirPath)) {
                QString detailMsg = QString(i18n("The directory you have chosen is a subdirectory of a recursive added sourcedirectory.\nSourcedirectory: %1\nDirectory: %2")).arg(sourceDirPath).arg(newDirPath);
                throw new EngineException(
                    i18n("You cannot add a directory which is a subdirectory of an already recursively added sourcedirectory because the chosen directory is already added implicitely."),
                    detailMsg
                );
            }
        }

        if (recursive) {
            // test if the current sourcedir is a subdirectory of the new dir
            if (sourceDirPath.startsWith(newDirPath)) {
                QString detailMsg = QString(i18n("The directory you have chosen to add recursively is the parentdirectory of at least one added sourcedirectory.\nSourcedirectory: %1\nDirectory: %2")).arg(sourceDirPath).arg(newDirPath);
                throw new EngineException(
                    i18n("You cannot add the parentdirectory of an already added sourcedirectory recursively."),
                    detailMsg
                );
            }
        }
    }
}


void Engine::addSourceDirs(SourceDir* parent) {

    kdDebug() << "[Engine::addSourceDirs] adding folders in: " << parent->dir()->absPath() << endl;

    // get all directories in the current directory
    const QFileInfoList* subdirlist = parent->dir()->entryInfoList(QDir::Dirs);

    if (subdirlist) {
        QFileInfoListIterator iterator(*subdirlist);
        QFileInfo* fileInfo;
        while ((fileInfo = iterator.current()) != 0) {

            // ignore ./.. and directories beginning with .
            // and ignore directories called 'ThumbNails'
            if (!fileInfo->fileName().startsWith(".")
                && mustHandleDirectory(fileInfo->fileName())
                ) {

                QDir subfolder(fileInfo->absFilePath());

                kdDebug() << "[Engine::addSourceDirs] handling subfolder: " << subfolder.absPath() << endl;

                bool loopDetected = false;

                // we have to test if this directory is part of a loop or is the superdirectory of an already added dir
                QString* alreadyAddedDir;
                for (alreadyAddedDir = m_loopDetectionHelper->first(); alreadyAddedDir; alreadyAddedDir = m_loopDetectionHelper->next()) {

                    if (*alreadyAddedDir == subfolder.canonicalPath()) {
                        loopDetected = true;
                        kdWarning() << "[Engine::addSourceDirs] loop detected, not adding directory again: '" << fileInfo->absFilePath() << "' is pointing to '" << *alreadyAddedDir << "'" << endl;
                        break;
                    }
                    if ((*alreadyAddedDir).startsWith(subfolder.canonicalPath(), true)) {
                        loopDetected = true;
                        kdWarning() << "[Engine::addSourceDirs] loop detected, not adding directory because it is a super directory (" << subfolder.canonicalPath() << ") of an already added directory: '" << *alreadyAddedDir << "'" << endl;
                        break;
                    }
                }

                if (!loopDetected) {
                    SourceDir* existingSourceDir = 0;

                    // we have to test if the directory is already processed to prevent endless loops
                    QIntDictIterator<SourceDir> it(*m_sourceDirDict);
                    while (!existingSourceDir && it.current()) {

                        SourceDir* current = it.current();

                        if (current->dir()->canonicalPath() == subfolder.canonicalPath()) {
                            existingSourceDir = current;
                            kdDebug() << "[Engine::addSourceDirs] directory is already added: " << current->dir()->canonicalPath() << endl;
                        }

                        ++it;
                    }

                    if (existingSourceDir) {

                        // add the directory to the list of handled directories for detcting loops
                        m_loopDetectionHelper->append(new QString(existingSourceDir->dir()->canonicalPath()));

                        // add all files in the current sourcedir
                        rescanSourceDir(existingSourceDir);

                        // make recursive call
                        addSourceDirs(existingSourceDir);
                    } else {
                        kdDebug() << "[Engine::addSourceDirs] found new sourcedir to add " << fileInfo->absFilePath() << endl;

                        // create the new SourceDir
                        SourceDir* child = new SourceDir(m_nextSourceDirId++, new QDir(fileInfo->absFilePath()), false);
                        child->setFound(true);

                        // add the current directory to the tree
                        child->setParent(parent);

                        // put the sourcedir into the sourcedir dictionary (id to sourcedir map)
                        m_sourceDirDict->insert(child->id(), child);

                        // add the directory to the list of handled directories for detcting loops
                        m_loopDetectionHelper->append(new QString(child->dir()->canonicalPath()));

                        // add all files in the current sourcedir
                        rescanSourceDir(child);

                        // make recursive call
                        addSourceDirs(child);
                    }
                }
            }
            ++iterator;
        }
    }
}


bool Engine::mustHandleDirectory(QString directoryName) {

    QStringList subdirsToIgnore = Configuration::getInstance()->subdirsToIgnore();

    for (QStringList::Iterator it = subdirsToIgnore.begin(); it != subdirsToIgnore.end(); ++it) {
        if (directoryName == *it) {
            return false;
        }
    }

    return true;
}


bool Engine::mustHandleFile(QString filename) {

    QStringList filetypesToHandle = Configuration::getInstance()->filetypesToHandle();

    for ( QStringList::Iterator it = filetypesToHandle.begin(); it != filetypesToHandle.end(); ++it ) {
        if (filename.endsWith(*it)) {
            return true;
        }
    }

    return false;
}
