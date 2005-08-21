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

#include "filesystemscanner.h"

#include "engine.h"
#include "filetagnodeassoc.h"
#include "filetagnodeassocdatetime.h"
#include "filetagnodeassocstring.h"
#include "sourcedir.h"
#include "tagnodedatetime.h"
#include "tagnodestring.h"
#include "../settings/settings.h"

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qxml.h>
#include <qdatetime.h>
#include <qintdict.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qdatetime.h>

#include <libexif/exif-data.h>
#include <libexif/exif-ifd.h>
#include <libexif/exif-content.h>
#include <libexif/exif-entry.h>
#include <libexif/exif-tag.h>


Tracer* FileSystemScanner::tracer = Tracer::getInstance("kde.kphotobook.engine", "FileSystemScanner");


FileSystemScanner::FileSystemScanner(Engine* engine) :
        m_engine(engine),
        m_loopDetectionHelper(0)
{
}


FileSystemScanner::~FileSystemScanner()
{
    delete m_loopDetectionHelper;
}


void FileSystemScanner::rescan()
{
    tracer->sinvoked(__func__) << "scanning..." << endl;

    rescanSourceFolders(m_engine->m_sourceDirs, true);
}


void FileSystemScanner::rescanWithEXIF()
{
    tracer->sinvoked(__func__) << "scanning with EXIF..." << endl;

    rescanSourceFolders(m_engine->m_sourceDirs, true);
}


SourceDir* FileSystemScanner::addSourceFolder(QDir* folder, bool recursive) throw(EngineException*)
{
    // test if the given folder is addable
    testIfFolderIsAddable(folder, recursive);

    tracer->sdebug(__func__) << "adding folder: " << folder->absPath() << ", recursive: " << recursive << "..." << endl;

    m_engine->dirtyfy();

    SourceDir* sourceFolder = new SourceDir(m_engine->m_nextSourceDirId++, folder, recursive);
    sourceFolder->setFound(true);
    m_engine->m_sourceDirs->append(sourceFolder);
    m_engine->m_sourceDirDict->insert(sourceFolder->id(), sourceFolder);

    // add all files in the folder
    rescanSourceFolder(sourceFolder, false);

    // add all folders below the given folder
    if (recursive) {
        delete m_loopDetectionHelper;
        m_loopDetectionHelper = new QPtrList<QString>;
        m_loopDetectionHelper->setAutoDelete(true);
        m_loopDetectionHelper->append(new QString(sourceFolder->dir()->canonicalPath()));

        addSourceFolders(sourceFolder);
    }

    return sourceFolder;
}


void FileSystemScanner::removeSourceFolder(SourceDir* sourceFolder)
{
    tracer->sdebug(__func__) << "removing sourceFolder: " << sourceFolder->dir()->absPath() << "..." << endl;

    m_engine->dirtyfy();

    // call this method recursively for each child
    if (sourceFolder->children() && sourceFolder->children()->count()) {
        // remove the subfolders

        // we have to work with a copy of the children list because the destructor of
        // the sourceFolder is changing the sourcefolder-tree
        QPtrList<SourceDir> temp(*sourceFolder->children());

        SourceDir* child;
        for ( child = temp.first(); child; child = temp.next() ) {
            removeSourceFolder(child);
        }
    }

    // remove the specified sourcedir
    m_engine->m_sourceDirs->remove(sourceFolder);
    m_engine->m_sourceDirDict->remove(sourceFolder->id());
    delete sourceFolder;
}


void FileSystemScanner::testIfFolderIsAddable(QDir* folder, bool recursive) throw(EngineException*)
{
    tracer->sinvoked(__func__) << folder->absPath() << ", recursive: " << recursive << "..." << endl;

    // test if the given folder is already added
    SourceDir* sourceFolder;
    for ( sourceFolder = m_engine->sourceDirs()->first(); sourceFolder; sourceFolder = m_engine->sourceDirs()->next() ) {

        if (*(sourceFolder->dir()) == *folder) {
            QString detailMsg = QString(i18n("The folder you have chosen is already added to KPhotoBook.\n"
                    "Folder: %1")).arg(sourceFolder->dir()->absPath());
            throw new EngineException(
                    i18n("You cannot add the same folder more than once to KPhotoBook."),
                    detailMsg
            );
        }

        QString sourceFolderPath = sourceFolder->dir()->absPath();
        QString newFolderPath = folder->absPath();

        if (sourceFolder->recursive()) {
            // test if the new folder is a subfolder of the current sourceFolder
            if (newFolderPath.startsWith(sourceFolderPath)) {
                QString detailMsg = QString(i18n("The folder you have chosen is a subfolder of a recursively added sourcefolder.\n"
                        "Sourcefolder: %1\nNew folder: %2")).arg(sourceFolderPath).arg(newFolderPath);
                throw new EngineException(
                        i18n("You cannot add a folder which is a subfolder of an already recursively added sourcefolder because"
                        "the chosen folder is already added implicitely."),
                        detailMsg
                 );
            }
        }

        if (recursive) {
            // test if the current sourceFolder is a subfolder of the new folder
            if (sourceFolderPath.startsWith(newFolderPath)) {
                QString detailMsg = QString(i18n("The folder you have chosen to add recursively is the parentfolder of at least "
                        "one already added sourcefolders.\nSourcefolder: %1\nNew folder: %2")).arg(sourceFolderPath).arg(newFolderPath);
                throw new EngineException(
                        i18n("You cannot add the parentfolder of an already added sourcefolder recursively."),
                        detailMsg
                 );
            }
        }
    }
    
    tracer->debug(__func__, "ended");
}


//
// private methods
//
void FileSystemScanner::rescanSourceFolders(QPtrList<SourceDir>* sourceFolders, bool forceEXIF)
{
    SourceDir* sourceFolder = 0;
    for (sourceFolder = sourceFolders->first(); sourceFolder; sourceFolder = sourceFolders->next()) {
        tracer->sdebug(__func__) << "rescanning sourcefolder: " << sourceFolder->id() << ": " << sourceFolder->dir()->absPath() << endl;

        if (sourceFolder->dir()->exists()) {

            sourceFolder->setFound(true);

            rescanSourceFolder(sourceFolder, forceEXIF);

            if (sourceFolder->children() && sourceFolder->children()->count() > 0) {
                rescanSourceFolders(sourceFolder->children(), forceEXIF);
            }

            // scan the filesystem for new sourcefolders
            if (sourceFolder->recursive()) {
                delete m_loopDetectionHelper;
                m_loopDetectionHelper = new QPtrList<QString>;
                m_loopDetectionHelper->setAutoDelete(true);
                m_loopDetectionHelper->append(new QString(sourceFolder->dir()->canonicalPath()));

                addSourceFolders(sourceFolder);
            }
        } else {
            sourceFolder->setFound(false);

            tracer->sdebug(__func__) << " sourcefolder: " << sourceFolder->id() << ": '" << sourceFolder->dir()->absPath() << "' not found" << endl;
        }
    }
}


void FileSystemScanner::rescanSourceFolder(SourceDir* sourceFolder, bool forceEXIF)
{
    tracer->sdebug(__func__) << "adding files in sourcefolder: " << sourceFolder->id() << ": " << sourceFolder->dir()->absPath() << endl;

    if (!sourceFolder->dir()->exists()) {
        tracer->swarning(__func__) << "sourcefolder: " << sourceFolder->id() << ": " << sourceFolder->dir()->absPath()
                << " does no longer exist --> ignoring it!!!" << endl;
    }
    
    // get a list with all files in the current sourcefolder
    const QFileInfoList* filelist = sourceFolder->dir()->entryInfoList(QDir::Files);
    if (filelist) {
        QFileInfoListIterator iterator( *filelist );
        QFileInfo* fileInfo;
        while ((fileInfo = iterator.current()) != 0) {

            // only add jpeg files
            if (mustHandleFile(fileInfo->fileName())) {

                File* file = m_engine->m_fileDict->find(fileInfo->absFilePath());
                if (!file) {
                    // the file is seen for the first time --> create it
                    tracer->sdebug(__func__) << "found new file to add: '" << fileInfo->absFilePath() << "'" << endl;
                    file = new File(m_engine, sourceFolder, new QFileInfo(*fileInfo));
                    m_engine->dirtyfy();

                    sourceFolder->appendFile(file);
                    m_engine->m_fileList->append(file);
                    m_engine->m_fileDict->insert(file->fileInfo()->absFilePath(), file);

                    // read exif data from the file and store this data in the database
                    readEXIF(file);
                } else {
                    // this file already exists in the database --> reread EXIF data if forceEXIF is true
                    if (forceEXIF) {
                        readEXIF(file);
                    }
                }
            }
            ++iterator;
        }
    }
}


void FileSystemScanner::addSourceFolders(SourceDir* parent)
{
    tracer->sinvoked(__func__) << "with sourceFolder: " << parent->dir()->absPath() << endl;

    // get all folders in the current directory
    const QFileInfoList* subfolderlist = parent->dir()->entryInfoList(QDir::Dirs);

    if (subfolderlist) {
        QFileInfoListIterator iterator(*subfolderlist);
        QFileInfo* fileInfo = 0;
        while ((fileInfo = iterator.current()) != 0) {

            // ignore ./.. and hidden folders (beginning with .)
            // and ignore folders called 'ThumbNails'
            if (!fileInfo->fileName().startsWith(".")
                && mustHandleFolder(fileInfo->fileName())
                ) {

                QDir subfolder(fileInfo->absFilePath());

                tracer->sdebug(__func__) << "handling subfolder: " << subfolder.absPath() << endl;

                bool loopDetected = false;

                // we have to test if this folder is part of a loop or if it is the superfolder of an already added dir
                QString* alreadyAddedFolder;
                for (alreadyAddedFolder = m_loopDetectionHelper->first(); alreadyAddedFolder; alreadyAddedFolder = m_loopDetectionHelper->next()) {

                    if (*alreadyAddedFolder == subfolder.canonicalPath()) {
                        loopDetected = true;
                        tracer->swarning(__func__) << "loop detected, not adding folder again: '" << fileInfo->absFilePath()
                                << "' is pointing to '" << *alreadyAddedFolder << "'" << endl;
                        break;
                    }
                    if ((*alreadyAddedFolder).startsWith(subfolder.canonicalPath(), true)) {
                        loopDetected = true;
                        tracer->swarning(__func__) << "loop detected, not adding folder because it is a super directory ("
                                << subfolder.canonicalPath() << ") of an already added folder: '" << *alreadyAddedFolder << "'" << endl;
                        break;
                    }
                }

                if (!loopDetected) {
                    SourceDir* existingSourceFolder = 0;

                    // we have to test if the folder is already processed to prevent endless loops
                    QIntDictIterator<SourceDir> it(*m_engine->m_sourceDirDict);
                    while (!existingSourceFolder && it.current()) {

                        SourceDir* current = it.current();

                        if (current->dir()->canonicalPath() == subfolder.canonicalPath()) {
                            existingSourceFolder = current;
                            tracer->sdebug(__func__) << "folder is already added: " << current->dir()->canonicalPath() << endl;
                        }

                        ++it;
                    }

                    if (existingSourceFolder) {

                        // add the directory to the list of handled directories for detecting loops
                        m_loopDetectionHelper->append(new QString(existingSourceFolder->dir()->canonicalPath()));

                        // make recursive call
                        addSourceFolders(existingSourceFolder);
                    } else {
                        tracer->sdebug(__func__) << "found new folder to add " << fileInfo->absFilePath() << endl;

                        // create the new SourceDir
                        SourceDir* child = new SourceDir(m_engine->m_nextSourceDirId++, new QDir(fileInfo->absFilePath()), false);
                        child->setFound(true);

                        // add the current directory to the tree
                        child->setParent(parent);

                        // put the sourcedir into the sourcedir dictionary (id to sourcedir map)
                        m_engine->m_sourceDirDict->insert(child->id(), child);

                        // add the directory to the list of handled directories for detcting loops
                        m_loopDetectionHelper->append(new QString(child->dir()->canonicalPath()));

                        // add all files in the current sourcedir
                        rescanSourceFolder(child, false);

                        // make recursive call
                        addSourceFolders(child);
                    }
                }
            }
            ++iterator;
        }
    }
}


void FileSystemScanner::readEXIF(File* file)
{
    static QString EXIFIFD_NAME_EXIF = exif_ifd_get_name(EXIF_IFD_EXIF);

    int maxDataLength = 255;
    char* charData = new char[maxDataLength];
    
    ExifData* exifData = exif_data_new_from_file(file->fileInfo()->absFilePath().ascii());
//    exif_data_dump(exifData);
    
    for (int i = 0; i < EXIF_IFD_COUNT; i++) {
        ExifContent* content = exifData->ifd[i];

        QString group = QString(exif_ifd_get_name((ExifIfd) i));
        if (group == EXIFIFD_NAME_EXIF) {

            for (unsigned int e = 0; e < content->count; e++) {
                ExifEntry* entry = content->entries[e];

                ExifTag exifTagType = entry->tag;
                QString title = QString(exif_tag_get_title(entry->tag));
                QString name = QString(exif_tag_get_name(entry->tag));

                exif_entry_get_value(entry, charData, maxDataLength);
                QString data = QString(charData);
                
                QString description = QString(exif_tag_get_description(entry->tag));
//                tracer->sdebug(__func__) << "    - " << title << " / " << name << " = " << data << " (" << description << ")" << endl;

                // get the exif tagnode title and the tagnode representing this tag from the engine
                TagNode* exifTagNodeTitle = m_engine->exifTagNodeTitle();
                TagNode* exifTagNodeEntry = exifTagNodeTitle->child(title);

                switch (exifTagType) {
                    case EXIF_TAG_DATE_TIME_ORIGINAL:
                    case EXIF_TAG_DATE_TIME_DIGITIZED: {
                        QDateTime dateTime = readExifDateTime(data);
                        TagNodeDateTime* exifTagNodeEntryDateTime = dynamic_cast<TagNodeDateTime*>(exifTagNodeEntry);
                        if (exifTagNodeEntryDateTime == 0) {
                            // tagnode for this EXIF entry does not exist --> create it
                            exifTagNodeEntryDateTime = dynamic_cast<TagNodeDateTime*>(m_engine->createTag(exifTagNodeTitle, TagNode::TYPE_DATETIME, title, description, QString::null));
                            exifTagNodeEntryDateTime->setReadonly(true);
                        }
                        if (dateTime.isValid()) {
                            // create new assoc
                            new FileTagNodeAssocDateTime(file, exifTagNodeEntryDateTime, dateTime);
                        }

                        break;
                    }
                    default: {
                        TagNodeString* exifTagNodeEntryString = dynamic_cast<TagNodeString*>(exifTagNodeEntry);
                        if (exifTagNodeEntryString == 0) {
                            // tagnode for this EXIF entry does not exist --> create it
                            exifTagNodeEntryString = dynamic_cast<TagNodeString*>(m_engine->createTag(exifTagNodeTitle, TagNode::TYPE_STRING, title, description, QString::null));
                            exifTagNodeEntryString->setReadonly(true);
                        }
                        // create new assoc
                        new FileTagNodeAssocString(file, exifTagNodeEntryString, data);
                    }
                }
            }
        }
    }

    exif_data_free(exifData);
    delete charData;
}


QDateTime FileSystemScanner::readExifDateTime(const QString& data)
{
    // EXIF date format is: YYYY:MM:DD hh:mm:ss
    bool ok;
    QString subString = data.mid(0, 4);
    int year = subString.toInt(&ok);
    if (!ok) {
        tracer->swarning(__func__) << "The date read from EXIF is invalid: " << data << " (year=" << subString << ")"<< endl;
        return QDateTime();
    }

    subString = data.mid(5, 2);
    int month = subString.toInt(&ok);
    if (!ok) {
        tracer->swarning(__func__) << "The date read from EXIF is invalid: " << data << " (month=" << subString << ")"<< endl;
        return QDateTime();
    }

    subString = data.mid(8, 2);
    int day = subString.toInt(&ok);
    if (!ok) {
        tracer->swarning(__func__) << "The date read from EXIF is invalid: " << data << " (day=" << subString << ")"<< endl;
        return QDateTime();
    }


    subString = data.mid(11, 2);
    int hour = subString.toInt(&ok);
    if (!ok) {
        tracer->swarning(__func__) << "The date read from EXIF is invalid: " << data << " (hour=" << subString << ")"<< endl;
        return QDateTime();
    }

    subString = data.mid(14, 2);
    int minute = subString.toInt(&ok);
    if (!ok) {
        tracer->swarning(__func__) << "The date read from EXIF is invalid: " << data << " (minute=" << subString << ")"<< endl;
        return QDateTime();
    }

    subString = data.mid(17, 2);
    int second = subString.toInt(&ok);
    if (!ok) {
        tracer->swarning(__func__) << "The date read from EXIF is invalid: " << data << " (second=" << subString << ")"<< endl;
        return QDateTime();
    }

    QDateTime dateTime = QDateTime(QDate(year, month, day), QTime(hour, minute, second));
    
    return dateTime;
}


bool FileSystemScanner::mustHandleFolder(QString folderName)
{
    QStringList folderToIgnore = Settings::fileFilterSubdirsToIgnore();

    for (QStringList::Iterator it = folderToIgnore.begin(); it != folderToIgnore.end(); ++it) {
        QRegExp regExp(*it);
        regExp.setWildcard(true);
        if (regExp.exactMatch(folderName)) {
            return false;
        }
    }

    return true;
}


bool FileSystemScanner::mustHandleFile(QString filename)
{
    QStringList filetypesToHandle = Settings::fileFilterFileToHandle();

    for ( QStringList::Iterator it = filetypesToHandle.begin(); it != filetypesToHandle.end(); ++it ) {
        QRegExp regExp(*it);
        regExp.setWildcard(true);
        if (regExp.exactMatch(filename)) {
            return true;
        }
    }

    return false;
}
