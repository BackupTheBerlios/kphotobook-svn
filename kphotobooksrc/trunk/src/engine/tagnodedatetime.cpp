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

#include <kglobal.h>

#include <qregexp.h>

Tracer* TagNodeDateTime::tracer = Tracer::getInstance("kde.kphotobook.engine", "TagNodeDateTime");

TagNodeDateTime::TagNodeDateTime(unsigned int id, const QString& text, const QString& comment, const QString& iconName, TagNode* parent)
    : TagNode(id, text, comment, iconName, parent)
{
    tracer->sinvoked(__func__) << "with id: " << id << ", text: " << text << ", comment: " << comment << "icon: " << iconName << endl;
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


bool TagNodeDateTime::tagged(File* file, QDateTime* fromDateTime, QDateTime* toDateTime)
{
    tracer->invoked(__func__);

    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);
    if (fileTagNodeAssoc == 0) {
        // no tag is linked to this file
        return false;
    }

    FileTagNodeAssocDateTime* assoc = dynamic_cast<FileTagNodeAssocDateTime*>(fileTagNodeAssoc);

    bool isTagged = true;
    if (fromDateTime && fromDateTime->isValid()) {
        isTagged = isTagged && (*fromDateTime <= assoc->value());
    }
    if (toDateTime && toDateTime->isValid()) {
        isTagged = isTagged && (*toDateTime >= assoc->value());
    }
    
    return isTagged;
}


bool TagNodeDateTime::tagged(File* file, QString pattern)
{
    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);
    if (fileTagNodeAssoc == 0) {
        // if no tag is linked to this file, the only matching pattern is '()' for matching an empty string
        return pattern == "()";
    }

    FileTagNodeAssocDateTime* assoc = dynamic_cast<FileTagNodeAssocDateTime*>(fileTagNodeAssoc);
    QString value = KGlobal::locale()->formatDateTime(assoc->value(), true, true);

    tracer->sinvoked(__func__) << "with pattern: '" << pattern << "', value: '" << value << "'" << endl;

    QRegExp regExp(pattern);
    if (regExp.exactMatch(value)) {
        tracer->sdebug(__func__) <<  "MATCHES!!!" << endl;
        return true;
    }

    tracer->sdebug(__func__) << "NO match!!!" << endl;
    return false;
}


bool TagNodeDateTime::tagged(File* file)
{
    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);
    if (fileTagNodeAssoc == 0) {
        // no tag is linked to this file
        return false;
    }
    
    FileTagNodeAssocDateTime* assoc = dynamic_cast<FileTagNodeAssocDateTime*>(fileTagNodeAssoc);
    return assoc->value().isValid();
}


