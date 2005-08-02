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

#ifndef FILETAGNODEASSOC_H
#define FILETAGNODEASSOC_H

#include <qstring.h>

class File;
class TagNode;


/**
 * Abstract superclass of all associations between a file and a node.
 * This abstract class contains the reference to the associated file and the tagNode.
 *
 * CVS-ID $Id$
 */
class FileTagNodeAssoc
{
    public:
        FileTagNodeAssoc(File* file, TagNode* tagNode);
    
        /**
        * Removes this assoc from the list in the tagnode and the file.
        */
        virtual ~FileTagNodeAssoc();
    
        void setFile(File* file)
        {
            m_file = file;
        }
    
        File* file()
        {
            return m_file;
        }
    
        void setTagNode(TagNode* tagNode)
        {
            m_tagNode = tagNode;
        }
    
        TagNode* tagNode()
        {
            return m_tagNode;
        }
    
        /**
        * Determines if this association must be dumped to the database.
        * The default implementation returns true;
        */
        virtual bool mustDump()
        {
            return true;
        }
    
        virtual QString valueAsString() = 0;
    
        virtual bool equals(QString* value) = 0;
        virtual bool greaterThan(QString* value) = 0;
        virtual bool lesserThan(QString* value) = 0;
    
    protected:
        /**
        * The file this association belongs to.
        */
        File* m_file;
    
        /**
        * The tagNode this association belongs to.
        */
        TagNode* m_tagNode;
};

#endif
