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

#ifndef XMLWRITER_H
#define XMLWRITER_H

#include "exception.h"
#include "xmlconstants.h"

#include <qfile.h>
#include <qstring.h>

class Engine;
class File;
class SourceDir;
class TagNode;
class FileTagNodeAssoc;

/**
 * This class is writing the xml-file containing all needed data of the engine.
 *
 * CVS-ID $Id$
 */
class XmlWriter : public XmlConstants {

public:
    XmlWriter(Engine* engine)
        : XmlConstants()
        , m_engine(engine) {
    }

    ~XmlWriter() {
    }

    void store(QFile* file) throw(PersistingException*);

private:
    Engine* m_engine;

    void dumpSourceDirs(QTextStream& stream, SourceDir* sourceDir, QString indent);
    void dumpTagNodes(QTextStream& stream, TagNode* tagnode, QString indent);
    void dumpFiles(QTextStream& stream, SourceDir* sourceDir, QString indent);
    void dumpFile(QTextStream& stream, File* file, QString indent);
    void dumpAssoc(QTextStream& stream, FileTagNodeAssoc* assoc, QString indent);

    /**
     * Quotes all needed characters in the specified string for xml files.
     */
    QString entitize(const QString data);
};

#endif
