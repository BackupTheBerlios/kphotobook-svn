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


FilterNodeTagDateTime::FilterNodeTagDateTime(FilterNode* parent, TagNodeDateTime* tagNodeDateTime, QDateTime* fromDateTime, QDateTime* toDateTime) :
    FilterNode(parent),
    m_tagNodeDateTime(tagNodeDateTime),
    m_pattern(QString::null),
    m_fromDateTime(fromDateTime),
    m_toDateTime(toDateTime)
{
}


FilterNodeTagDateTime::FilterNodeTagDateTime(TagNodeDateTime* tagNodeDateTime, QDateTime* fromDateTime, QDateTime* toDateTime) :
    FilterNode(0),
    m_tagNodeDateTime(tagNodeDateTime),
    m_pattern(QString::null),
    m_fromDateTime(fromDateTime),
    m_toDateTime(toDateTime)
{
}


FilterNodeTagDateTime::FilterNodeTagDateTime(FilterNode* parent, TagNodeDateTime* tagNodeDateTime, QString pattern) :
    FilterNode(parent),
    m_tagNodeDateTime(tagNodeDateTime),
    m_pattern(pattern),
    m_fromDateTime(0),
    m_toDateTime(0)
{
}


FilterNodeTagDateTime::FilterNodeTagDateTime(TagNodeDateTime* tagNodeDateTime, QString pattern) :
    FilterNode(0),
    m_tagNodeDateTime(tagNodeDateTime),
    m_pattern(pattern),
    m_fromDateTime(0),
    m_toDateTime(0)
{
}


FilterNodeTagDateTime::~FilterNodeTagDateTime()
{
    delete m_fromDateTime;
    delete m_toDateTime;
}


bool FilterNodeTagDateTime::evaluate(File* file) throw(FilterException*)
{
    if (m_pattern.isNull()) {
        return m_tagNodeDateTime->tagged(file, m_fromDateTime, m_toDateTime);
    } else {
        return m_tagNodeDateTime->tagged(file, m_pattern);
    }
}


void FilterNodeTagDateTime::dump(QString indention)
{
    if (m_pattern.isNull()) {
        if (m_fromDateTime) {
            dumper->sdebug("dump") << indention << *m_tagNodeDateTime->text() << " >= " << m_fromDateTime->toString() << endl;
        }
        if (m_toDateTime) {
            dumper->sdebug("dump") << indention << *m_tagNodeDateTime->text() << " <= " << m_toDateTime->toString() << endl;
        }
    } else {
        dumper->sdebug("dump") << indention << *m_tagNodeDateTime->text() << " = " << m_pattern << endl;
    }
}


#include "filternodetagdatetime.moc"
