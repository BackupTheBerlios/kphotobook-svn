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

#include "tagnodedatetime.h"

#include "filetagnodeassoc.h"
#include "filetagnodeassocdatetime.h"

#include <qregexp.h>

Tracer* TagNodeDateTime::tracer = Tracer::getInstance("kde.kphotobook.engine", "TagNodeDateTime");

TagNodeDateTime::TagNodeDateTime(unsigned int id, const QString& text, const QString& comment, const QString& iconName, TagNode* parent)
    : TagNode(id, text, comment, iconName, parent)
{
    tracer->sinvoked("TagNodeDateTime") << "with id: " << id << ", text: " << text << ", comment: " << comment << "icon: " << iconName << endl;
}


void TagNodeDateTime::setTagged(File* file, QDateTime value)
{
    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);
    
    if (fileTagNodeAssoc) {
        FileTagNodeAssocDateTime* fileTagNodeAssocDateTime = dynamic_cast<FileTagNodeAssocDateTime*>(fileTagNodeAssoc);
        fileTagNodeAssocDateTime->setValue(value);
    } else {
        new FileTagNodeAssocDateTime(file, this, value);
    }
}


bool TagNodeDateTime::tagged(File* file, QString pattern)
{
    ///@todo implement method 'tagged(...)'
    /*
    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);

    if (fileTagNodeAssoc == 0) {
        // if no tag is linked to this file, the only matching pattern is '()' for matching an empty string
        return pattern == "()";
    }

    FileTagNodeAssocString* fileTagNodeAssocString = dynamic_cast<FileTagNodeAssocString*>(fileTagNodeAssoc);
    QString value = fileTagNodeAssocString->value();

    tracer->sinvoked("tagged") << "with pattern: '" << pattern << "', value: '" << value << "'" << endl;

    QRegExp regExp(pattern);
    if (regExp.exactMatch(value)) {
        tracer->sdebug("tagged") <<  "MATCHES!!!" << endl;
        return true;
    }

    tracer->sdebug("tagged") << "NO match!!!" << endl;
    */
    return false;
}


bool TagNodeDateTime::tagged(File* file)
{
    ///@todo implement method 'tagged(...)'
    /*
    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);

    if (fileTagNodeAssoc == 0) {
        // if no tag is linked to this file, the only matching pattern is '()' for matching an empty string
        return false;
    } else {
        FileTagNodeAssocString* fileTagNodeAssocString = dynamic_cast<FileTagNodeAssocString*>(fileTagNodeAssoc);

        QString s = fileTagNodeAssocString->value();
        return !s.isEmpty();
    }
    */
}


