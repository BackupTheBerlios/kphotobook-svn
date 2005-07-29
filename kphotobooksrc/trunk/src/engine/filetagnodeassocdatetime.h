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

#ifndef FILETAGNODEASSOCDATETIME_H
#define FILETAGNODEASSOCDATETIME_H

#include "filetagnodeassoc.h"
#include "../tracer/tracer.h"

#include <qdatetime.h>

class File;
class TagNodeDateTime;

/**
 * Concrete class representing the association between a file and a TagNodeDateTime.
 * This associations contains a reference to the tagnode and the value of the association.
 *
 * CVS-ID $Id: filetagnodeassocboolean.h 274 2005-03-25 08:52:15Z choenig $
 */
class FileTagNodeAssocDateTime : public FileTagNodeAssoc
{
private:
    static Tracer* tracer;

public:
    FileTagNodeAssocDateTime(File* file, TagNodeDateTime* tagNodeDateTime, QDateTime value);
    FileTagNodeAssocDateTime(File* file, TagNodeDateTime* tagNodeDateTime, QString value);

    ~FileTagNodeAssocDateTime()
    {
        // we don't delete anything!
    }

    /**
     * Returns the tagNode associated with this association.
     */
    TagNodeDateTime* tagNodeDateTime();

    void setValue(QDateTime value)
    {
        m_value = value;
    }

    QDateTime value()
    {
        return m_value;
    }

    /**
     * A string association must be dumped only, if the value is not empty.
     */
    virtual bool mustDump()
    {
        return m_value.isValid();
    }

    virtual QString valueAsString()
    {
        return m_value.toString(Qt::ISODate);
    }

    bool equals(QString* value)
    {
        QDateTime other = QDateTime::fromString(*value, Qt::ISODate);
        if (!other.isValid()) {
            return false;
        }
        
        return (m_value == other);
    }
    
    bool greaterThan(QString* value)
    {
        QDateTime other = QDateTime::fromString(*value, Qt::ISODate);
        if (!other.isValid()) {
            return false;
        }
        
        return (m_value > other);
    }
    
    bool lesserThan(QString* value)
    {
        QDateTime other = QDateTime::fromString(*value, Qt::ISODate);
        if (!other.isValid()) {
            return false;
        }
        
        return (m_value < other);
    }

private:
    /**
     * The value of this association.
     */
    QDateTime m_value;
};

#endif
