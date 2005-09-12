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

#include "file.h"
#include "filesystemscanner.h"
#include "filetagnodeassoc.h"
#include "filetagnodeassocboolean.h"
#include "filternode.h"
#include "folder.h"
#include "tagnodeboolean.h"
#include "tagnodetitle.h"
#include "../backend/xmlparser.h"
#include "../backend/xmlwriter.h"
#include "../constants.h"
#include "../settings/settings.h"
#include "../tracer/tracer.h"

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qdatetime.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qintdict.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qxml.h>


Tracer* Engine::tracer = Tracer::getInstance("kde.kphotobook.engine", "Engine");


Engine::Engine() :
    m_dirty(false),
    m_fileinfo(0),
    m_uid(0),

    m_fileSystemScanner(new FileSystemScanner(this)),

    m_nextSourceDirId(1),
    m_sourceDirDict(new QIntDict<Folder>()),
    m_sourceDirs(new QPtrList<Folder>()),

    m_nextTagNodeId(1),
    m_tagNodeDict(new QIntDict<TagNode>()),
    m_tagForest(new QPtrList<TagNode>()),
    m_exifTitleTag(0),

    m_fileDict(new QDict<File>()),
    m_fileList(new QPtrList<File>()),
    m_fileList2display(new QPtrList<File>())
{
    tracer->sinvoked(__func__) << "empty engine" << endl;

    // create the EXIF tagnode
    createExifTagNode();
}


Engine::Engine(QFileInfo& fileinfo) throw(EngineException*) :
    m_dirty(false),
    m_fileinfo(new QFileInfo(fileinfo)),
    m_uid(0),

    m_fileSystemScanner(new FileSystemScanner(this)),

    m_nextSourceDirId(1),
    m_sourceDirDict(new QIntDict<Folder>()),
    m_sourceDirs(new QPtrList<Folder>()),

    m_nextTagNodeId(1),
    m_tagNodeDict(new QIntDict<TagNode>()),
    m_tagForest(new QPtrList<TagNode>()),
    m_exifTitleTag(0),

    m_fileDict(new QDict<File>()),
    m_fileList(new QPtrList<File>()),
    m_fileList2display(new QPtrList<File>())
{
    tracer->sinvoked(__func__) << "with file '" << m_fileinfo->absFilePath() << "'" << endl;

    // if file does not exist, we have nothing to do
    if (!m_fileinfo->exists()) {
        QString msg = QString("File '%1' does not exist.").arg(m_fileinfo->absFilePath());
        tracer->serror(__func__) << msg << endl;
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
        tracer->serror(__func__) << msg << ": " << file.errorString() << endl;
        throw new EngineException(msg, file.errorString());
    }

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
        tracer->serror(__func__) << "Error occured during parsing the file '" << file.name() << "'" << endl;

        cleanUp();
        if (parser.exception()) {
            tracer->serror(__func__) << "exception occured: " << parser.exception()->message() << endl;

            throw parser.exception();
        } else {
            throw new EngineException(i18n("Unknown error while parsing the xml file occured!"));
        }
    }

    // generate a uid if the file does not contain one (for compatibility reason with version 0.0.5)
    if (!m_uid) {
        m_uid = generateUid();
    }

    // create the EXIF tagnode if it was not contained in the database (for compatibility reason with version 0.0.6)
    if (m_exifTitleTag == 0) {
        createExifTagNode();
    }

    // read the files in all sourcedirectories
    if (Settings::generalRescanWhileStartup()) {
        m_fileSystemScanner->rescan();
    }

    // trace a little
    tracer->sdebug(__func__) << m_fileList->count() << " images added" << endl;
}


Engine::~Engine()
{
    cleanUp();
}


QString Engine::currentURL() const
{
    return m_fileinfo ? m_fileinfo->absFilePath() : "";
}


int Engine::totalNumberOfFiles() const
{
    return m_fileList->count();
}


int Engine::filteredNumberOfFiles() const
{
    return m_fileList2display->count();
}


TagNode* Engine::createTag(TagNode* parent, TagNode::Type type, const QString& text, const QString& comment, const QString& iconName)
{
    tracer->sinvoked(__func__) << "with type: " << type  << ", text: " << text << ", comment: " << comment << ", icon: " << iconName << endl;

    TagNode* tagNode = TagNode::createInstance(type, m_nextTagNodeId++, text, comment, iconName, parent);
    tagNode->setSecret(false);

    // put the tagNode into the tree
    if (!parent) {
        m_tagForest->append(tagNode);
    }

    // put the new tagnode into the tagnode dictionary
    m_tagNodeDict->insert(tagNode->id(), tagNode);

    m_dirty = true;

    emit(newTagNode(tagNode));

    return tagNode;
}


void Engine::editTag(TagNode* tag, const QString& text, const QString& comment, const QString& iconName)
{
    tag->setText(text);
    tag->setComment(comment);
    tag->setIconName(iconName);

    m_dirty = true;
}


void Engine::removeTag(TagNode* tag)
{
    tracer->sinvoked(__func__) << "with tagnode: " << tag->text() << "..." << endl;

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


bool Engine::isTagTextValid(TagNode* parent, QString& text, bool ignoreExifTag) const
{
    // a maintag must not be named 'EXIF' (if ignoreExifTag is false)
    if (!ignoreExifTag && parent == 0 && text.upper() == "EXIF") {
        return false;
    }

    QPtrList<TagNode>* siblings = m_tagForest;

    if (parent) {
        siblings = parent->children();
    }

    if (siblings) {
        TagNode* sibling;
        for ( sibling = siblings->first(); sibling; sibling = siblings->next() ) {
            if (*sibling->text() == text) {
                // sibling with the same name already exists!
                return false;
            }
        }
    }

    // no sibling with the same name found
    return true;
}


QPtrList<File>* Engine::files(FilterNode* filterRootNode)
{
    tracer->sinvoked(__func__) << "with filter:" << endl;
    if (filterRootNode) {
        filterRootNode->dump("");
    }

    // remove all files currently shown
    m_fileList2display->clear();

    // loop over all source directories
    QIntDictIterator<Folder> sourceDirIt(*m_sourceDirDict);
    for ( ; sourceDirIt.current(); ++sourceDirIt ) {
        Folder* sourceDir = sourceDirIt.current();

        // handle the files in this directory only if it is selected
        if (sourceDir->include()) {

            // loop over all files in the current sourcedirectory
            QPtrList<File>* fileList = sourceDir->files();
            for (File* file = fileList->first(); file; file = fileList->next()) {

                // append the file if it matches the filter
                if (filterRootNode->evaluate(file)) {
                    m_fileList2display->append(file);
                }
            }
        }
    }

    return m_fileList2display;
}


void Engine::save() throw(PersistingException*)
{
    tracer->invoked(__func__);

    if (m_dirty) {
        tracer->sdebug(__func__) << "engine is dirty --> saving it..." << endl;

        // rename current file
        QDir path = m_fileinfo->dirPath(true);
        QString oldFileName = m_fileinfo->fileName();

        // create new filename
        QString basename = QFileInfo(oldFileName).baseName(true);
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmsszzz");
        QString newFileName = QString("%1-%2.%3").arg(basename).arg(timestamp).arg(Constants::FILE_EXTENSION);

        tracer->sdebug(__func__) << "backing up file in directory '" << path.absPath() << "': '" << oldFileName << "' --> '" << newFileName << "'..." << endl;

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


void Engine::saveAs(QFileInfo& newFile) throw(PersistingException*)
{
    m_fileinfo = new QFileInfo(newFile);
    m_uid = generateUid();

    // save
    XmlWriter writer = XmlWriter(this);
    writer.store(new QFile(m_fileinfo->absFilePath()));

    m_dirty = false;
}


//
// private methods
//
void Engine::cleanUp()
{
    delete m_fileinfo;
    delete m_uid;
    delete m_fileSystemScanner;

    //
    // sourcedir members
    //
    delete m_sourceDirDict;
    delete m_sourceDirs;

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
    delete m_fileList2display;
}


QString* Engine::generateUid()
{
    QString uid = QString("%1").arg(QDateTime::currentDateTime().toTime_t());
    return new QString(uid);
}


void Engine::createExifTagNode()
{
    TagNode* exifTagNode = createTag(0, TagNode::TYPE_TITLE, i18n("EXIF"), i18n("All EXIF data read from the image."), Constants::ICON_TAG_EXIF);
    m_exifTitleTag = dynamic_cast<TagNodeTitle*>(exifTagNode);
    m_exifTitleTag->setReadonly(true);
}


#include "engine.moc"
