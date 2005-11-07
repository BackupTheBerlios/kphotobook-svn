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
#include "folder.h"
#include "tagnode.h"
#include "filetagnodeassoc.h"
#include "../tracer/tracer.h"

#include <qfileinfo.h>
#include <qptrlist.h>


Tracer* File::tracer = Tracer::getInstance("kde.kphotobook.engine", "File");


File::File(Engine* engine, Folder* sourceDir, QFileInfo* fileInfo, int rotate) :
    KFileItem(KFileItem::Unknown, KFileItem::Unknown, QString("file:%1").arg(fileInfo->absFilePath())),
    m_engine(engine),
    m_sourceDir(sourceDir),
    m_fileInfo(fileInfo),
    m_rotate(rotate),
    m_found(false),
    m_assocs(new QPtrList<FileTagNodeAssoc>())
{
}


File::~File()
{
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


void File::appendAssoc(FileTagNodeAssoc* assoc)
{
    // force this file as the file of the association
    assoc->setFile(this);

    // add the association to the list of association
    m_assocs->append(assoc);
}


void File::removeAssoc(FileTagNodeAssoc* assoc)
{
    m_assocs->remove(assoc);
}


FileTagNodeAssoc* File::getAssoc(TagNode* tagNode) const
{
    FileTagNodeAssoc* currentAssoc;
    for (currentAssoc = m_assocs->first(); currentAssoc; currentAssoc = m_assocs->next()) {

        if (currentAssoc->tagNode() == tagNode) {
            return currentAssoc;
        }
    }

    return 0;
}

FileTagNodeAssoc* File::getAssoc(int id) const
{
    FileTagNodeAssoc* currentAssoc;
    for (currentAssoc = m_assocs->first(); currentAssoc; currentAssoc = m_assocs->next()) {

        if (currentAssoc->tagNode()->id() == id) {
            return currentAssoc;
        }
    }

    return 0;
}
