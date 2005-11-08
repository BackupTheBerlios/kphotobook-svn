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

#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_


#include "xmlconstants.h"
#include "../exception.h"
class Engine;
class File;
class Folder;
class TagNode;
class Tracer;

#include <qptrstack.h>
#include <qstring.h>
#include <qxml.h>
template<class type> class QIntDict;


/**
 * The handler for parsing the KphotoBook xml file.
 *
 * CVS-ID $Id: xmlparser.h 484 2005-08-28 19:21:48Z starcube $
 */
class XmlParser : public QXmlDefaultHandler, public XmlConstants
{
    private:
        static Tracer* tracer;
    
    public:
        enum SECTION {
            SECTION_UNDEFINED_START = 1,
            SECTION_KPHOTOBOOK,
            SECTION_SOURCEDIRS,
            SECTION_SOURCEDIRS_DONE,
            SECTION_TAGS,
            SECTION_TAGS_DONE,
            SECTION_FILES,
            SECTION_FILES_DONE,
            SECTION_UNDEFINED_END
        };
    
    public:
        XmlParser(Engine* engine);
        ~XmlParser();
    
        // content handler methods
        bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
        bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
        bool characters(const QString& ch);
    
        // error handler methods
        bool warning(const QXmlParseException& exception);
        bool error(const QXmlParseException& exception);
        bool fatalError(const QXmlParseException& exception);
    
        EngineException* exception()
        {
            return m_exception;
        }
    
    private:
        Engine* m_engine;
    
        /**
        * Contains the section we are currently parsing.
        */
        SECTION m_section;
    
        // stack containing the enclosing sourcedir tags
        QPtrStack<Folder> m_sourceDirstack;
    
        // stack containing the enclosing tags
        QPtrStack<TagNode> m_tagNodeStack;
    
        // the sourcedir of the currently handled files or 0
        Folder* m_currentSourceDir;
    
        // the file currently handled or 0
        File* m_currentFile;
    
        // the list with all tagnodes the current file is associated with or 0
        QIntDict<TagNode>* m_tagNodesOfCurrentFile;
    
        // contains the exception if there was one
        EngineException* m_exception;
    
        bool handleSourceDir(const QXmlAttributes& atts);
        bool handleTag(const QXmlAttributes& atts);
        bool handleFile(const QXmlAttributes& atts);
        bool handleTagAssoc(const QXmlAttributes& atts);
    
        QString toString(const QXmlAttributes& atts);
};

#endif // _KXMLPARSER_H_
