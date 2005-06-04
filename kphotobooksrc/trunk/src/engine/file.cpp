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

#include "file.h"

#include "engine.h"
#include "sourcedir.h"
#include "tagnode.h"
#include "filetagnodeassoc.h"

Tracer* File::tracer = Tracer::getInstance("kde.kphotobook.engine", "File");

File::File(Engine* engine, SourceDir* sourceDir, QFileInfo* fileInfo, int rotate)
    : KFileItem(KFileItem::Unknown, KFileItem::Unknown, QString("file:%1").arg(fileInfo->absFilePath()))
    , m_engine(engine)
    , m_sourceDir(sourceDir)
    , m_fileInfo(fileInfo)
    , m_rotate(rotate)
    , m_found(false)
    , m_assocs(new QPtrList<FileTagNodeAssoc>()) {
}


File::~File() {

    m_engine->m_fileList->remove(this);
    m_engine->m_fileDict->remove(this->fileInfo()->absFilePath());

    m_sourceDir->removeFile(this);

    delete m_fileInfo;

    // delete every assoc before deleting the assocslist (the destructor of the assoc is removing itself from the assoclist)
    QPtrListIterator<FileTagNodeAssoc> it(*m_assocs);
    FileTagNodeAssoc *currentAssoc;
    while ((currentAssoc = it.current()) != 0) {
        ++it;
        delete currentAssoc;
    }
    delete m_assocs;
}


void File::appendAssoc(FileTagNodeAssoc* assoc) {

    // test if an association to the same tagnode already exists
    bool assocFound = false;

    FileTagNodeAssoc* currentAssoc;
    for ( currentAssoc = m_assocs->first(); currentAssoc; currentAssoc = m_assocs->next() ) {

        // test if the tagNode of the specified association is already referenced from this file
        if (currentAssoc->tagNode() == assoc->tagNode()) {
            tracer->swarning(__func__) << "Association between file '" << assoc->file()->fileInfo()->absFilePath() << "' and tagnode '" << assoc->tagNode()->text() << "' already exists." << endl;

            // update the existing association to reflect value of the specified one
            currentAssoc->update(assoc);

            assocFound = true;
            break;
        }
    }

    // add the specified association only if it is not yet added
    if (!assocFound) {
        // set this file as file of the associations
        assoc->setFile(this);

        // add the association to the list of associations
        m_assocs->append(assoc);
    }
}


void File::removeAssoc(FileTagNodeAssoc* assoc) {

    m_assocs->remove(assoc);
}


FileTagNodeAssoc* File::getAssoc(TagNode* tagNode) {

    FileTagNodeAssoc* currentAssoc;
    for ( currentAssoc = m_assocs->first(); currentAssoc; currentAssoc = m_assocs->next() ) {

        if (currentAssoc->tagNode() == tagNode) {
            return currentAssoc;
        }
    }

    return 0;
}
