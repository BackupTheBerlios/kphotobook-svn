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
#include "file.h"
#include "filetagnodeassoc.h"
#include "filetagnodeassocdatetime.h"
#include "filetagnodeassocstring.h"
#include "folder.h"
#include "tagnodedatetime.h"
#include "tagnodestring.h"
#include "tagnodetitle.h"
#include "../settings/settings.h"
#include "../tracer/tracer.h"

#include <kapplication.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qdatetime.h>
#include <qdir.h>
#include <qintdict.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qxml.h>

#include <libexif/exif-data.h>
#include <libexif/exif-ifd.h>
#include <libexif/exif-content.h>
#include <libexif/exif-entry.h>
#include <libexif/exif-tag.h>


Tracer* FileSystemScanner::tracer = Tracer::getInstance("kde.kphotobook.engine", "FileSystemScanner");


FileSystemScanner::FileSystemScanner(Engine* engine) :
        m_engine(engine),
        m_loopDetectionHelper(0),
        m_cancel(false)
{}


FileSystemScanner::~FileSystemScanner( )
{
    delete m_loopDetectionHelper;
}


void FileSystemScanner::rescanFast()
{
    tracer->sinvoked(__func__) << "scanning fast..." << endl;

    rescanFolders(m_engine->m_sourceDirs, false, true);
    m_cancel = false;
}


void FileSystemScanner::rescan()
{
    tracer->sinvoked(__func__) << "scanning..." << endl;

    rescanFolders(m_engine->m_sourceDirs, false);
    m_cancel = false;
}


void FileSystemScanner::rescanWithEXIF()
{
    tracer->sinvoked(__func__) << "scanning with EXIF..." << endl;

    rescanFolders(m_engine->m_sourceDirs, true);
    m_cancel = false;
}


Folder* FileSystemScanner::addFolder(QDir* dir, bool recursive) throw(EngineException*)
{
    // test if the given folder is addable
    testIfFolderIsAddable(dir, recursive);

    tracer->sdebug(__func__) << "adding folder: " << dir->absPath() << ", recursive: " << recursive << "..." << endl;

    m_engine->dirtyfy();

    Folder* folder = new Folder(m_engine->m_nextSourceDirId++, dir, recursive);
    folder->setFound(true);
    m_engine->m_sourceDirs->append(folder);
    m_engine->m_sourceDirDict->insert(folder->id(), folder);

    // add all files in the folder
    rescanFolder(folder, false);
    if (m_cancel) {
        m_cancel = false;
        return folder;
    }

    // add all folders below the given folder
    if (recursive) {
        delete m_loopDetectionHelper;
        m_loopDetectionHelper = new QPtrList<QString>;
        m_loopDetectionHelper->setAutoDelete(true);
        m_loopDetectionHelper->append(new QString(folder->dir()->canonicalPath()));

        addFolders(folder);
    }

    emit(newFolder(folder));

    return folder;
}


void FileSystemScanner::removeFolder(Folder* folder)
{
    tracer->sdebug(__func__) << "removing folder: " << folder->dir()->absPath() << "..." << endl;

    m_engine->dirtyfy();

    // call this method recursively for each child
    if (folder->children() && folder->children()->count()) {
        // remove the subfolders

        // we have to work with a copy of the children list because the destructor of
        // the folder is changing the folder-tree
        QPtrList<Folder> temp(*folder->children());

        Folder* child;
        for ( child = temp.first(); child; child = temp.next() ) {
            removeFolder(child);
        }
    }

    // remove the specified folder
    m_engine->m_sourceDirs->remove(folder);
    m_engine->m_sourceDirDict->remove(folder->id());
    delete folder;
}


void FileSystemScanner::testIfFolderIsAddable(QDir* dir, bool recursive) const throw(EngineException*)
{
    tracer->sinvoked(__func__) << dir->absPath() << ", recursive: " << recursive << "..." << endl;

    // test if the given folder is already added
    Folder* folder;
    for (folder = m_engine->sourceDirs()->first(); folder; folder = m_engine->sourceDirs()->next() ) {

        if (*(folder->dir()) == *dir) {
            QString detailMsg = QString(i18n("The folder you have chosen is already added to KPhotoBook.\n"
                    "Folder: %1")).arg(folder->dir()->absPath());
            throw new EngineException(
                    i18n("You cannot add the same folder more than once to KPhotoBook."),
                    detailMsg
            );
        }

        QString folderPath = folder->dir()->absPath();
        QString newFolderPath = dir->absPath();

        if (folder->recursive()) {
            // test if the new folder is a subfolder of the current sourceFolder
            if (newFolderPath.startsWith(folderPath)) {
                QString detailMsg = QString(i18n("The folder you have chosen is a subfolder of a recursively added folder.\n"
                        "Folder: %1\nNew folder: %2")).arg(folderPath).arg(newFolderPath);
                throw new EngineException(
                        i18n("You cannot add a folder which is a subfolder of an already recursively added folder because"
                        "the chosen folder is already added implicitely."),
                        detailMsg
                 );
            }
        }

        if (recursive) {
            // test if the current folder is a subfolder of the new folder
            if (folderPath.startsWith(newFolderPath)) {
                QString detailMsg = QString(i18n("The folder you have chosen to add recursively is the parentfolder of at least "
                        "one already added folders.\nFolder: %1\nNew folder: %2")).arg(folderPath).arg(newFolderPath);
                throw new EngineException(
                        i18n("You cannot add the parentfolder of an already added folder recursively."),
                        detailMsg
                 );
            }
        }
    }

    tracer->debug(__func__, "ended");
}


//
// public slots
//
void FileSystemScanner::slotCancel()
{
    m_cancel = true;
}


//
// private methods
//
void FileSystemScanner::rescanFolders(QPtrList<Folder>* folders, bool forceEXIF, bool fast)
{
    Folder* folder = 0;
    for (folder = folders->first(); folder; folder = folders->next()) {
        QString currentFolderPath = folder->dir()->absPath();

        tracer->sdebug(__func__) << "rescanning folder: " << folder->id() << ": " << currentFolderPath << endl;
        emit(progress_scanningFolder(currentFolderPath));

        if (folder->dir()->exists()) {

            folder->setFound(true);

            if (!fast) {
                rescanFolder(folder, forceEXIF);
                if (m_cancel) {
                    return;
                }
            }

            if (folder->children() && folder->children()->count()) {
                rescanFolders(folder->children(), forceEXIF, fast);
            }

            // scan the filesystem for new folders
            if (!fast && folder->recursive()) {
                delete m_loopDetectionHelper;
                m_loopDetectionHelper = new QPtrList<QString>;
                m_loopDetectionHelper->setAutoDelete(true);
                m_loopDetectionHelper->append(new QString(folder->dir()->canonicalPath()));

                addFolders(folder);
            }
        } else {
            folder->setFound(false);

            tracer->sdebug(__func__) << "folder: " << folder->id() << ": '" << currentFolderPath << "' not found" << endl;
            emit(progress_folderNotFound(currentFolderPath));
        }
    }
}


void FileSystemScanner::rescanFolder(Folder* folder, bool forceEXIF)
{
    // this method is called regularly while rescanning the filesystem
    // before doing something here we process all outstanding events!
    KApplication::kApplication()->processEvents();
    if (m_cancel) {
        return;
    }

    QString currentFolderPath = folder->dir()->absPath();

    tracer->sdebug(__func__) << "adding files in folder: " << folder->id() << ": " << currentFolderPath << endl;

    if (!folder->dir()->exists()) {
        tracer->swarning(__func__) << "folder: " << folder->id() << ": " << currentFolderPath
                << " does no longer exist --> ignoring it!!!" << endl;
    }

    // get a list with all files in the current folder
    const QFileInfoList* filelist = folder->dir()->entryInfoList(QDir::Files);
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
                    file = new File(m_engine, folder, new QFileInfo(*fileInfo));
                    m_engine->dirtyfy();

                    folder->appendFile(file);
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


void FileSystemScanner::addFolders(Folder* parent)
{
    tracer->sinvoked(__func__) << "with folder: " << parent->dir()->absPath() << endl;

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
                        emit(progress_loopDetected(fileInfo->absFilePath(), *alreadyAddedFolder));
                        break;
                    }
                    if ((*alreadyAddedFolder).startsWith(subfolder.canonicalPath(), true)) {
                        loopDetected = true;
                        tracer->swarning(__func__) << "loop detected, not adding folder because it is a super directory ("
                                << subfolder.canonicalPath() << ") of an already added folder: '" << *alreadyAddedFolder << "'" << endl;
                        emit(progress_loopDetected(subfolder.canonicalPath(), *alreadyAddedFolder));
                        break;
                    }
                }

                if (!loopDetected) {
                    Folder* existingFolder = 0;

                    // we have to test if the folder is already processed to prevent endless loops
                    QIntDictIterator<Folder> it(*m_engine->m_sourceDirDict);
                    while (!existingFolder && it.current()) {

                        Folder* current = it.current();

                        if (current->dir()->canonicalPath() == subfolder.canonicalPath()) {
                            existingFolder = current;
                            tracer->sdebug(__func__) << "folder is already added: " << current->dir()->canonicalPath() << endl;
                            emit(progress_folderAlreadyAdded(current->dir()->canonicalPath()));
                        }

                        ++it;
                    }

                    if (existingFolder) {

                        // add the directory to the list of handled directories for detecting loops
                        m_loopDetectionHelper->append(new QString(existingFolder->dir()->canonicalPath()));

                        // make recursive call
                        addFolders(existingFolder);
                    } else {
                        tracer->sdebug(__func__) << "found new folder to add " << fileInfo->absFilePath() << endl;

                        // create the new folder
                        Folder* child = new Folder(m_engine->m_nextSourceDirId++, new QDir(fileInfo->absFilePath()), false);
                        child->setFound(true);

                        // add the current directory to the tree
                        child->setParent(parent);

                        // put the folder into the folder dictionary (id to folder map)
                        m_engine->m_sourceDirDict->insert(child->id(), child);

                        // add the directory to the list of handled directories for detcting loops
                        m_loopDetectionHelper->append(new QString(child->dir()->canonicalPath()));

                        // add all files in the current folder
                        rescanFolder(child, false);
                        if (m_cancel) {
                            return;
                        }

                        // make recursive call
                        addFolders(child);
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
