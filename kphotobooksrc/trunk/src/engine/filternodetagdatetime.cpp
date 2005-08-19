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


FilterNodeTagDateTime::FilterNodeTagDateTime(FilterNode* parent, TagNodeDateTime* tagNodeDateTime, DateTimeFilterData* data) :
    FilterNode(parent),
    m_tagNodeDateTime(tagNodeDateTime),
    m_data(data)
{
}


FilterNodeTagDateTime::FilterNodeTagDateTime(TagNodeDateTime* tagNodeDateTime, DateTimeFilterData* data) :
    FilterNode(0),
    m_tagNodeDateTime(tagNodeDateTime),
    m_data(data)
{
}


FilterNodeTagDateTime::~FilterNodeTagDateTime()
{
}


bool FilterNodeTagDateTime::evaluate(File* file) throw(FilterException*)
{
    switch (m_data->getState()) {
        case DateTimeFilterData::INVALID:
        case DateTimeFilterData::NO_FILTER_SET: {
            return false;
        }
        case DateTimeFilterData::FROM_DATE_SET: {
            return m_tagNodeDateTime->tagged(file, new QDateTime(m_data->getDateTimeFrom()), 0);
        }
        case DateTimeFilterData::TO_DATE_SET: {
            return m_tagNodeDateTime->tagged(file, 0, new QDateTime(m_data->getDateTimeTo()));
        }
        case DateTimeFilterData::FROM_TO_DATE_SET: {
            return m_tagNodeDateTime->tagged(file, new QDateTime(m_data->getDateTimeFrom()), new QDateTime(m_data->getDateTimeTo()));
        }
        case DateTimeFilterData::PATTERN_DATE_SET: {
            return m_tagNodeDateTime->tagged(file, m_data->getPattern());
        }
        case DateTimeFilterData::SINGLE_DATE_SET: {
            return m_tagNodeDateTime->tagged(file, new QDateTime(m_data->getDateTimeFrom()), new QDateTime(m_data->getDateTimeTo()));
        }
        case DateTimeFilterData::NO_DATE_SET: {
            return !m_tagNodeDateTime->tagged(file);
        }
    }
    
    return false;
}


void FilterNodeTagDateTime::dump(QString indention)
{
    switch (m_data->getState()) {
        case DateTimeFilterData::INVALID:
        case DateTimeFilterData::NO_FILTER_SET: {
            break;
        }
        case DateTimeFilterData::FROM_DATE_SET: {
            dumper->sdebug("dump") << indention << *m_tagNodeDateTime->text() << " >= " << m_data->getDateTimeFrom().toString() << endl;
        }
        case DateTimeFilterData::TO_DATE_SET: {
            dumper->sdebug("dump") << indention << *m_tagNodeDateTime->text() << " <= " << m_data->getDateTimeTo().toString() << endl;
        }
        case DateTimeFilterData::FROM_TO_DATE_SET: {
            dumper->sdebug("dump") << indention << *m_tagNodeDateTime->text() << " >= " << m_data->getDateTimeFrom().toString() << " <= " << m_data->getDateTimeTo().toString() << endl;
        }
        case DateTimeFilterData::PATTERN_DATE_SET: {
            dumper->sdebug("dump") << indention << *m_tagNodeDateTime->text() << " ~= " << m_data->getPattern() << endl;
        }
        case DateTimeFilterData::SINGLE_DATE_SET: {
            dumper->sdebug("dump") << indention << *m_tagNodeDateTime->text() << " == " << m_data->getDateTimeTo().toString() << endl;
        }
        case DateTimeFilterData::NO_DATE_SET: {
            break;
        }
    }
}


#include "filternodetagdatetime.moc"
