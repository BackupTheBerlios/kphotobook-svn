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

#include "xmlwriter.h"

#include "../engine/engine.h"
#include "../engine/sourcedir.h"
#include "../engine/tagnode.h"
#include "../engine/file.h"
#include "../engine/filetagnodeassoc.h"

#include <kdebug.h>

#include <qfileinfo.h>
#include <qregexp.h>

#include <typeinfo>


void XmlWriter::store(QFile* file2write) throw(PersistingException*) {

    kdDebug() << "[XmlWriter::store] invoked... file: '" << QFileInfo(*file2write).absFilePath() << "'." << endl;

    if (!file2write->open(IO_WriteOnly)) {
        QString msg = QString("Could not open file for writing: '%1'").arg(QFileInfo(*file2write).absFilePath());
        throw new PersistingException(
            msg,
            ""
        );
    }

    QTextStream stream(file2write);

    // we are forcing character set
    stream.setEncoding(QTextStream::Latin1);

    // write head of the file
    stream << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << "\n";
    stream << "<" << ELEMENT_KPHOTOBOOK << " " << ATTRIBUTE_KPHOTOBOOK_UID << "=\"" << entitize(*(m_engine->uid())) << "\">" << "\n\n";

    // dump the sourcedirs
    stream << "  <!-- source directories -->" << "\n";
    stream << "  <" << ELEMENT_SOURCEDIRS << ">" << "\n";
    SourceDir* sourceDir;
    for ( sourceDir = m_engine->m_sourceDirs->first(); sourceDir; sourceDir = m_engine->m_sourceDirs->next() ) {

        QString recursive = QString("false");
        if (sourceDir->recursive()) {
            recursive = QString("true");
        }
        stream << "    <" << ELEMENT_SOURCEDIR << " " << ATTRIBUTE_SOURCEDIR_ID << "=\"" << sourceDir->id() << "\" " << ATTRIBUTE_SOURCEDIR_DIR << "=\"" << entitize(sourceDir->dir()->absPath()) << "\" " << ATTRIBUTE_SOURCEDIR_RECURSIVE << "=\"" << recursive << "\"";

        if (sourceDir->children() && sourceDir->children()->count() > 0) {
            stream << ">" << "\n";

            // store the sourcedirectories recursively
            dumpSourceDirs(stream, sourceDir, "      ");

            stream << "    </" << ELEMENT_SOURCEDIR << ">" << "\n";
        } else {
            stream << "/>" << "\n";
        }
    }
    stream << "  </" << ELEMENT_SOURCEDIRS << ">" << "\n\n";


    // dump the whole tagtree
    stream << "  <!-- tags -->" << "\n";
    stream << "  <" << ELEMENT_TAGS << ">" << "\n";
    TagNode* tagnode;
    for ( tagnode = m_engine->m_tagForest->first(); tagnode; tagnode = m_engine->m_tagForest->next() ) {

        dumpTagNodes(stream, tagnode, QString("    "));
    }
    stream << "  </" << ELEMENT_TAGS << ">" << "\n\n";


    // dump all files
    stream << "  <!-- files -->" << "\n";
    for ( sourceDir = m_engine->m_sourceDirs->first(); sourceDir; sourceDir = m_engine->m_sourceDirs->next() ) {

        dumpFiles(stream, sourceDir, QString("  "));
    }


    // write tail of the file
    stream << "</" << ELEMENT_KPHOTOBOOK << ">" << "\n";

    file2write->close();
}


void XmlWriter::dumpSourceDirs(QTextStream& stream, SourceDir* sourceDir, QString indent) {

    SourceDir* child;
    for ( child = sourceDir->children()->first(); child; child = sourceDir->children()->next() ) {

        stream << indent << "<" << ELEMENT_SOURCEDIR << " " << ATTRIBUTE_SOURCEDIR_ID << "=\"" << child->id() << "\" " << ATTRIBUTE_SOURCEDIR_DIR << "=\"" << entitize(child->dir()->absPath()) << "\"";

        if (child->children() && child->children()->count() > 0) {
            stream << ">" << "\n";

            // store the sourcedirectories recursively
            dumpSourceDirs(stream, child, QString(indent).append("  "));

            stream << indent << "</" << ELEMENT_SOURCEDIR << ">" << "\n";
        } else {
            stream << "/>" << "\n";
        }
    }
}


void XmlWriter::dumpTagNodes(QTextStream& stream, TagNode* tagnode, QString indent) {

    // dump the specified tagnode
    stream << indent << "<"
            << ELEMENT_TAG << " "
            << ATTRIBUTE_TAG_ID << "=\"" << tagnode->id() << "\" "
            << ATTRIBUTE_TAG_NAME << "=\"" << entitize(*(tagnode->text())) << "\" "
            << ATTRIBUTE_TAG_TYPE << "=\"" << *(tagnode->type()) << "\"";

    if (tagnode->comment()) {
        stream << " " << ATTRIBUTE_TAG_COMMENT << "=\"" << entitize(*(tagnode->comment())) << "\"";
    }
    
    if (tagnode->iconName()) {
        stream << " " << ATTRIBUTE_TAG_ICON << "=\"" << *(tagnode->iconName()) << "\"";
    }

    // handle children if there are any
    if (tagnode->children() && tagnode->children()->count() > 0) {
        stream << ">" << "\n";

        // loop over all children and call this method recursively
        TagNode* child;
        for (child = tagnode->children()->first(); child; child = tagnode->children()->next()) {

            dumpTagNodes(stream, child, QString(indent).append("  "));
        }

        stream << indent << "</" << ELEMENT_TAG << ">" << "\n";
    } else {
        stream << "/>" << "\n";
    }
}


void XmlWriter::dumpFiles(QTextStream& stream, SourceDir* sourceDir, QString indent) {

    stream << indent << "<" << ELEMENT_FILES << " " << ATTRIBUTE_FILES_SOURCEDIRID << "=\"" << sourceDir->id() << "\">" << "\n";

    // dump the files in the specified sourceDir
    File* file;
    for (file = sourceDir->files()->first(); file; file = sourceDir->files()->next()) {

        dumpFile(stream, file, QString(indent).append("  "));
    }

    stream << indent << "</" << ELEMENT_FILES << ">" << "\n\n";

    // call this method recursively for all subdirectories
    if (sourceDir->children() && sourceDir->children()->count() > 0) {
        SourceDir* child;
        for ( child = sourceDir->children()->first(); child; child = sourceDir->children()->next() ) {

            dumpFiles(stream, child, indent);
        }
    }
}


void XmlWriter::dumpFile(QTextStream& stream, File* file, QString indent) {

    // dump the specified tagnode
    stream << indent << "<" << ELEMENT_FILE << " " << ATTRIBUTE_FILE_NAME << "=\"" << entitize(file->fileInfo()->fileName()) << "\" " << ATTRIBUTE_FILE_ROTATE << "=\"" << file->rotate() << "\">\n";

    // dump all tag associations
    if (file->assocs() && file->assocs()->count()) {

        FileTagNodeAssoc* assoc;
        for (assoc = file->assocs()->first(); assoc; assoc = file->assocs()->next()) {

            dumpAssoc(stream, assoc, QString(indent).append("  "));
        }
    }

    stream << indent << "</" << ELEMENT_FILE << ">" << "\n";
}


void XmlWriter::dumpAssoc(QTextStream& stream, FileTagNodeAssoc* assoc, QString indent) {

    if (assoc->mustDump()) {
        stream << indent << "<" << ELEMENT_TAGASSOC << " " << ATTRIBUTE_TAGASSOC_TAGID << "=\"" << assoc->tagNode()->id() << "\" " << ATTRIBUTE_TAGASSOC_VALUE << "=\"" << entitize(assoc->valueAsString()) << "\"/>\n";
    }
}


QString XmlWriter::entitize(const QString data) {

    QString temp = QString(data);

    temp.replace('&', "&amp;"); // This _must_ come first
    temp.replace('<', "&lt;");
    temp.replace('>', "&gt;");
    temp.replace('"', "&quot;");
    temp.replace('\'', "&apos;");

    return temp;
}
