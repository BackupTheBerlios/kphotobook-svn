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

#ifndef FILETAGNODEASSOCSTRING_H
#define FILETAGNODEASSOCSTRING_H

#include "filetagnodeassoc.h"
class File;
class TagNodeString;
class Tracer;

#include <qstring.h>


/**
 * Concrete class representing the association between a file and a TagNodeString.
 * This associations contains a reference to the tagnode and the value of the association.
 *
 * CVS-ID $Id: filetagnodeassocboolean.h 274 2005-03-25 08:52:15Z choenig $
 */
class FileTagNodeAssocString : public FileTagNodeAssoc
{
    private:
        static Tracer* tracer;
    
    public:
        FileTagNodeAssocString(File* file, TagNodeString* tagNodeString, QString value);
    
        ~FileTagNodeAssocString()
        {
            // we don't delete anything!
        }
    
        /**
        * Returns the tagNode associated with this association.
        */
        TagNodeString* tagNodeString() const;
    
        void setValue(QString value)
        {
            m_value = value;
        }
    
        QString value() const
        {
            return m_value;
        }
    
        /**
        * A string association must be dumped only, if the value is not empty.
        */
        virtual bool mustDump() const
        {
            return !m_value.isEmpty();
        }
    
        virtual QString valueAsString() const
        {
            return m_value;
        }
    
        bool equals(QString* value) const;
        bool greaterThan(QString* value) const;
        bool lesserThan(QString* value) const;
    
    private:
        /**
        * The value of this association.
        */
        QString m_value;
};

#endif
