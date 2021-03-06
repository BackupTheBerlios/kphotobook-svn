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

#include "filternodetagstring.h"

#include "file.h"
#include "tagnodestring.h"


FilterNodeTagString::FilterNodeTagString(FilterNode* parent, TagNodeString* tagNodeString, QString pattern) :
    FilterNode(parent),
    m_tagNodeString(tagNodeString),
    m_pattern(pattern)
{
}


FilterNodeTagString::FilterNodeTagString(TagNodeString* tagNodeString, QString pattern) :
    FilterNode(0),
    m_tagNodeString(tagNodeString),
    m_pattern(pattern)
{
}


FilterNodeTagString::~FilterNodeTagString()
{
}


bool FilterNodeTagString::evaluate(File* file) throw(FilterException*)
{
    return m_tagNodeString->tagged(file, m_pattern);
}


void FilterNodeTagString::dump(QString indention) const
{
    dumper->sdebug("dump") << indention << *m_tagNodeString->text() << " = " << m_pattern << endl;
}


#include "filternodetagstring.moc"
