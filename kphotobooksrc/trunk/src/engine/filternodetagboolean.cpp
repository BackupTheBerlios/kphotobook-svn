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

#include "filternodetagboolean.h"


FilterNodeTagBoolean::FilterNodeTagBoolean(FilterNode* parent, TagNodeBoolean* tagNodeBoolean, bool value) :
    FilterNode(parent),
    m_tagNodeBoolean(tagNodeBoolean),
    m_value(value)
{
}


FilterNodeTagBoolean::FilterNodeTagBoolean(TagNodeBoolean* tagNodeBoolean, bool value) :
    FilterNode(0),
    m_tagNodeBoolean(tagNodeBoolean),
    m_value(value)
{
}


FilterNodeTagBoolean::~FilterNodeTagBoolean()
{
}


bool FilterNodeTagBoolean::evaluate(File* file) throw(FilterException*)
{
    return m_tagNodeBoolean->tagged(file) == m_value;
}


void FilterNodeTagBoolean::dump(QString indention)
{
    dumper->sdebug("dump") << indention << *m_tagNodeBoolean->text() << (m_value ? " = true" : " = false") << endl;
}


#include "filternodetagboolean.moc"
