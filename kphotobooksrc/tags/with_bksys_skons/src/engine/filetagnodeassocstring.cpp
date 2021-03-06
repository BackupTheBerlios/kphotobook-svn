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

#include "filetagnodeassocstring.h"

#include "file.h"
#include "tagnodestring.h"
#include "../tracer/tracer.h"

#include <typeinfo>


Tracer* FileTagNodeAssocString::tracer = Tracer::getInstance("kde.kphotobook.engine", "FileTagNodeAssocString");


FileTagNodeAssocString::FileTagNodeAssocString(File* file, TagNodeString* tagNodeString, QString value) :
    FileTagNodeAssoc(file, tagNodeString),
    m_value(value)
{
}


TagNodeString* FileTagNodeAssocString::tagNodeString() const
{
    return dynamic_cast<TagNodeString*>(m_tagNode);
}


bool FileTagNodeAssocString::equals(QString* value) const
{
    return (valueAsString() == *value);
}


bool FileTagNodeAssocString::greaterThan(QString* value) const
{
    return (valueAsString() != *value);
}


bool FileTagNodeAssocString::lesserThan(QString* value) const
{
    return (valueAsString() != *value);
}
