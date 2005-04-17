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

#include "filetagnodeassocradio.h"

#include "file.h"
#include "tagnoderadio.h"

#include <typeinfo>

Tracer* FileTagNodeAssocRadio::tracer = Tracer::getInstance("kde.kphotobook.engine", "FileTagNodeAssocRadio");

FileTagNodeAssocRadio::FileTagNodeAssocRadio(File* file, TagNodeRadio* tagNode, bool value)
    : FileTagNodeAssoc(file, tagNode)
    , m_value(value) {
}


FileTagNodeAssocRadio::FileTagNodeAssocRadio(File* file, TagNodeRadio* tagNode, QString value)
    : FileTagNodeAssoc(file, tagNode)
    , m_value(value == Constants::STRING_VALUE_TRUE) {
}


TagNodeRadio* FileTagNodeAssocRadio::tagNode() {
    return dynamic_cast<TagNodeRadio*>(m_tagNode);
}


void FileTagNodeAssocRadio::update(FileTagNodeAssoc* assoc) {

    if (typeid(*assoc) == typeid(FileTagNodeAssocRadio)) {
        FileTagNodeAssocRadio* concreteAssoc = dynamic_cast<FileTagNodeAssocRadio*>(assoc);
        m_value = concreteAssoc->value();
    } else {
        tracer->error("update", "The specified association is not of the type 'FileTagNodeAssocRadio'.");
    }
}

