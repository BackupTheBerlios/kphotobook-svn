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

#include "filetagnodeassocboolean.h"

#include "file.h"
#include "tagnodeboolean.h"
#include "../constants.h"
#include "../tracer/tracer.h"

#include <typeinfo>


Tracer* FileTagNodeAssocBoolean::tracer = Tracer::getInstance("kde.kphotobook.engine", "FileTagNodeAssocBoolean");


FileTagNodeAssocBoolean::FileTagNodeAssocBoolean(File* file, TagNodeBoolean* tagNodeBoolean, bool value) :
    FileTagNodeAssoc(file, tagNodeBoolean),
    m_value(value)
{
}


FileTagNodeAssocBoolean::FileTagNodeAssocBoolean(File* file, TagNodeBoolean* tagNodeBoolean, QString value) :
    FileTagNodeAssoc(file, tagNodeBoolean),
    m_value(value == Constants::STRING_VALUE_TRUE)
{
}


TagNodeBoolean* FileTagNodeAssocBoolean::tagNodeBoolean()
{
    return dynamic_cast<TagNodeBoolean*>(m_tagNode);
}


QString FileTagNodeAssocBoolean::valueAsString() const
{
    return (m_value ? Constants::STRING_VALUE_TRUE : Constants::STRING_VALUE_FALSE);
}    


bool FileTagNodeAssocBoolean::equals(QString* value) const
{
    return (valueAsString() == *value);
}


bool FileTagNodeAssocBoolean::greaterThan(QString* value) const
{
    return (valueAsString() != *value);
}


bool FileTagNodeAssocBoolean::lesserThan(QString* value) const
{
    return (valueAsString() != *value);
}
