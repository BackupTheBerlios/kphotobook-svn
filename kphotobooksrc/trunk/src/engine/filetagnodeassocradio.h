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

#ifndef FILETAGNODEASSOCRADIO_H
#define FILETAGNODEASSOCRADIO_H

#include "filetagnodeassoc.h"
class File;
class TagNodeRadio;
class Tracer;

#include <qstring.h>


/**
 * Concrete class representing the association between a file and a TagNodeRadio.
 * This associations contains a reference to the tagnode and the state of the association.
 *
 * CVS-ID $Id: FileTagNodeAssocRadio.h 274 2005-03-25 08:52:15Z choenig $
 */
class FileTagNodeAssocRadio : public FileTagNodeAssoc
{
    private:
        static Tracer* tracer;
    
    public:
        FileTagNodeAssocRadio(File* file, TagNodeRadio* tagNode, bool value = false);
        FileTagNodeAssocRadio(File* file, TagNodeRadio* tagNode, QString value);
    
        ~FileTagNodeAssocRadio()
        {
            // we don't delete anything!
        }
    
        /**
        * Returns the tagNode associated with this association.
        */
        TagNodeRadio* tagNode() const;
    
        void setValue(bool value)
        {
            m_value = value;
        }
    
        bool value() const
        {
            return m_value;
        }
    
        /**
        * A boolean association must be dumped only, if the assoc is true.
        */
        virtual bool mustDump() const
        {
            return m_value;
        }
    
        virtual QString valueAsString() const;
    
        bool equals(QString* value) const;
        bool greaterThan(QString* value) const;
        bool lesserThan(QString* value) const;
    
    private:
        /**
        * The value of this association.
        */
        bool m_value;
};

#endif
