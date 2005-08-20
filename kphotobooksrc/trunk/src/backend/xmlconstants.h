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

#ifndef XMLCONSTANTS_H
#define XMLCONSTANTS_H

#include <qstring.h>


/**
 * Constants used by the xml parser and writer.
 *
 * CVS-ID $Id$
 */
class XmlConstants
{
    public:
        // the constants containg the element- and attribute-names
        static const QString ELEMENT_KPHOTOBOOK;
        static const QString ELEMENT_SOURCEDIRS;
        static const QString ELEMENT_SOURCEDIR;
        static const QString ELEMENT_TAGS;
        static const QString ELEMENT_TAG;
        static const QString ELEMENT_FILES;
        static const QString ELEMENT_FILE;
        static const QString ELEMENT_TAGASSOC;
    
        static const QString ATTRIBUTE_KPHOTOBOOK_NAME;
        static const QString ATTRIBUTE_KPHOTOBOOK_UID;
    
        static const QString ATTRIBUTE_SOURCEDIR_ID;
        static const QString ATTRIBUTE_SOURCEDIR_DIR;
        static const QString ATTRIBUTE_SOURCEDIR_RECURSIVE;
    
        static const QString ATTRIBUTE_TAG_ID;
        static const QString ATTRIBUTE_TAG_NAME;
        static const QString ATTRIBUTE_TAG_COMMENT;
        static const QString ATTRIBUTE_TAG_TYPE;
        static const QString ATTRIBUTE_TAG_ICON;
        static const QString ATTRIBUTE_TAG_SECRET;
        static const QString ATTRIBUTE_TAG_READONLY;
    
        static const QString ATTRIBUTE_FILES_SOURCEDIRID;
    
        static const QString ATTRIBUTE_FILE_NAME;
        static const QString ATTRIBUTE_FILE_ROTATE;
    
        static const QString ATTRIBUTE_TAGASSOC_TAGID;
        static const QString ATTRIBUTE_TAGASSOC_VALUE;
    
    public:
        XmlConstants()
        {
        }
    
        ~XmlConstants()
        {
        }
};

#endif
