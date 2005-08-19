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

#include "datetimefilterdata.h"

#include "../uitrees/treehelper.h"


Tracer* DateTimeFilterData::tracer = Tracer::getInstance("kde.kphotobook.utils", "DateTimeFilterData");


DateTimeFilterData::DateTimeFilterData() :
    m_state(INVALID),
    m_fromDateTime(QDateTime()),
    m_toDateTime(QDateTime()),
    m_pattern(QString::null)
{
}


DateTimeFilterData::DateTimeFilterData(const QString& filterstring) :
    m_state(INVALID),
    m_fromDateTime(QDateTime()),
    m_toDateTime(QDateTime()),
    m_pattern(QString::null)
{
    setFilterString(filterstring);
}


DateTimeFilterData::~DateTimeFilterData()
{
}


void DateTimeFilterData::setFilterString(const QString& filterstring)
{
    QString filter = filterstring.stripWhiteSpace();
    if (filter.isEmpty()) {
        // no filter is set
        m_state = NO_FILTER_SET;
    } else if (filter.startsWith(">= ")) {
        // from date is set
        m_state = FROM_DATE_SET;
        m_fromDateTime = QDateTime::fromString(filter.mid(2).stripWhiteSpace(), Qt::ISODate);
    } else if (filter.startsWith("<= ")) {
        // to date is set
        m_state = TO_DATE_SET;
        m_toDateTime = QDateTime::fromString(filter.mid(2).stripWhiteSpace(), Qt::ISODate);
    } else if (filter.contains(" - ")) {
        // from and to date are set
        m_state = FROM_TO_DATE_SET;

        int pos = filter.find(" - ");
        m_fromDateTime = QDateTime::fromString(filter.mid(0, pos).stripWhiteSpace(), Qt::ISODate);
        m_toDateTime = QDateTime::fromString(filter.mid(pos+3).stripWhiteSpace(), Qt::ISODate);
    } else if (filter.startsWith("= ")) {
        // singledate is set
        m_state = SINGLE_DATE_SET;
        m_fromDateTime = QDateTime::fromString(filter.mid(2).stripWhiteSpace(), Qt::ISODate);
        m_toDateTime = m_fromDateTime;
    } else if (filter == "()") {
        // 'no date' filter is set
        m_state = NO_DATE_SET;
    } else {
        // 'pattern' filter is set
        m_state = PATTERN_DATE_SET;
        m_pattern = QString(filter);
    }

    emit dataChanged();
}


void DateTimeFilterData::setFilterIsInvalid()
{
    m_state = INVALID;
    emit dataChanged();
}


void DateTimeFilterData::setFilterNoFilterSet()
{
    m_state = NO_FILTER_SET;
    emit dataChanged();
}


void DateTimeFilterData::setFilterNoDateSet()
{
    m_state = NO_DATE_SET;
    emit dataChanged();
}


void DateTimeFilterData::setFilterDateTimeFrom(const QDateTime& fromDateTime)
{
    if (!fromDateTime.isValid()) {
        m_state = INVALID;
    } else {
        m_state = FROM_DATE_SET;
        m_fromDateTime = QDateTime(fromDateTime);
    }
    emit dataChanged();
}


void DateTimeFilterData::setFilterDateTimeTo(const QDateTime& toDateTime)
{
    if (!toDateTime.isValid()) {
        m_state = INVALID;
    } else {
        m_state = TO_DATE_SET;
        m_toDateTime = QDateTime(toDateTime);
    }
    emit dataChanged();
}


void DateTimeFilterData::setFilterDateTimeRange(const QDateTime& fromDateTime, const QDateTime& toDateTime)
{
    if (!fromDateTime.isValid() || !toDateTime.isValid()) {
        m_state = INVALID;
    } else {
        m_state = FROM_TO_DATE_SET;
        m_fromDateTime = QDateTime(fromDateTime);
        m_toDateTime = QDateTime(toDateTime);
    }
    emit dataChanged();
}


void DateTimeFilterData::setFilterSingleDate(const QDateTime& singleDate)
{
    if (!singleDate.isValid()) {
        m_state = INVALID;
    } else {
        m_state = SINGLE_DATE_SET;
        m_fromDateTime = QDateTime(singleDate);
        m_toDateTime = QDateTime(singleDate);
    }
    emit dataChanged();
}


void DateTimeFilterData::setFilterPattern(const QString& pattern)
{
    tracer->sinvoked(__func__) << "with pattern: " << pattern << endl;
    m_state = PATTERN_DATE_SET;
    m_pattern = QString(pattern);
    emit dataChanged();
}


const QString DateTimeFilterData::toString()
{
    switch (m_state) {
        case INVALID:
        case NO_FILTER_SET: {
            return QString::null;
        }
        case FROM_DATE_SET: {
            return QString(">= %1").
                    arg(TreeHelper::formatDateTime(m_fromDateTime));
        }
        case TO_DATE_SET: {
            return QString("<= %1").
                    arg(TreeHelper::formatDateTime(m_toDateTime));
        }
        case FROM_TO_DATE_SET: {
            return QString("%1 - %2").
                    arg(TreeHelper::formatDateTime(m_fromDateTime)).
                    arg(TreeHelper::formatDateTime(m_toDateTime));
        }
        case PATTERN_DATE_SET: {
            return m_pattern;
        }
        case SINGLE_DATE_SET: {
            return QString("= %1").
                    arg(TreeHelper::formatDateTime(m_fromDateTime));
        }
        case NO_DATE_SET: {
            return QString("()");
        }
    }

    return QString::null;
}


const QString DateTimeFilterData::marshal()
{
    switch (m_state) {
        case INVALID:
        case NO_FILTER_SET: {
            return QString::null;
        }
        case FROM_DATE_SET: {
            return QString(">= %1").
                    arg(m_fromDateTime.toString(Qt::ISODate));
        }
        case TO_DATE_SET: {
            return QString("<= %1").
                    arg(m_toDateTime.toString(Qt::ISODate));
        }
        case FROM_TO_DATE_SET: {
            return QString("%1 - %2").
                    arg(m_fromDateTime.toString(Qt::ISODate)).
                    arg(m_toDateTime.toString(Qt::ISODate));
        }
        case PATTERN_DATE_SET: {
            return m_pattern;
        }
        case SINGLE_DATE_SET: {
            return QString("= %1").
                    arg(m_fromDateTime.toString(Qt::ISODate));
        }
        case NO_DATE_SET: {
            return QString("()");
        }
    }

    return QString::null;
}
