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

#include "filternodetagdatetime.h"


FilterNodeTagDateTime::FilterNodeTagDateTime(FilterNode* parent, TagNodeDateTime* tagNodeDateTime, QString pattern)
    : FilterNode(parent)
    , m_tagNodeDateTime(tagNodeDateTime)
    , m_pattern(pattern)
{
}


FilterNodeTagDateTime::FilterNodeTagDateTime(TagNodeDateTime* tagNodeDateTime, QString pattern)
    : FilterNode(0)
    , m_tagNodeDateTime(tagNodeDateTime)
    , m_pattern(pattern)
{
}


FilterNodeTagDateTime::~FilterNodeTagDateTime()
{
}


bool FilterNodeTagDateTime::evaluate(File* file) throw(FilterException*)
{
    return m_tagNodeDateTime->tagged(file, m_pattern);
}


void FilterNodeTagDateTime::dump(QString indention)
{
    dumper->sdebug("dump") << indention << *m_tagNodeDateTime->toString() << " = " << m_pattern << endl;
}


#include "filternodetagdatetime.moc"
