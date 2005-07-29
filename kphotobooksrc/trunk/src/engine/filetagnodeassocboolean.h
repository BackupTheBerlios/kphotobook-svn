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

#ifndef FILETAGNODEASSOCBOOLEAN_H
#define FILETAGNODEASSOCBOOLEAN_H

#include "filetagnodeassoc.h"
#include "../constants.h"
#include "../tracer/tracer.h"

#include <qstring.h>

class File;
class TagNodeBoolean;

/**
 * Concrete class representing the association between a file and a TagNodeBoolean.
 * This associations contains a reference to the tagnode and the state of the association.
 *
 * CVS-ID $Id$
 */
class FileTagNodeAssocBoolean : public FileTagNodeAssoc
{
    private:
        static Tracer* tracer;
    
    public:
        FileTagNodeAssocBoolean(File* file, TagNodeBoolean* tagNodeBoolean, bool value = false);
    
        FileTagNodeAssocBoolean(File* file, TagNodeBoolean* tagNodeBoolean, QString value);
    
        ~FileTagNodeAssocBoolean()
        {
            // we don't delete anything!
        }
    
        /**
        * Returns the tagNode associated with this association.
        */
        TagNodeBoolean* tagNodeBoolean();
    
        void setValue(bool value)
        {
            m_value = value;
        }
    
        bool value()
        {
            return m_value;
        }
    
        /**
        * A boolean association must be dumped only, if the assoc is true.
        */
        virtual bool mustDump()
        {
            return m_value;
        }
    
        virtual QString valueAsString()
        {
            return (m_value ? Constants::STRING_VALUE_TRUE : Constants::STRING_VALUE_FALSE);
        }
    
        bool equals(QString* value)
        {
            return (valueAsString() == *value);
        }
        bool greaterThan(QString* value)
        {
            return (valueAsString() != *value);
        }
        bool lesserThan(QString* value)
        {
            return (valueAsString() != *value);
        }
    
    private:
        /**
        * The value of this association.
        */
        bool m_value;
};

#endif
