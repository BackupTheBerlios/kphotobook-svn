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

#include "tagnodestring.h"

#include "filetagnodeassoc.h"
#include "filetagnodeassocstring.h"

#include <kdebug.h>

#include <qregexp.h>

Tracer* TagNodeString::tracer = Tracer::getInstance("kde.kphotobook.engine", "TagNodeString");

TagNodeString::TagNodeString(unsigned int id, const QString& text, const QString& comment, const QString& iconName, TagNode* parent)
    : TagNode(id, text, comment, iconName, parent) {

        tracer->sinvoked("TagNodeString") << "with id: " << id << ", text: " << text << ", comment: " << comment << "icon: " << iconName << endl;
}


void TagNodeString::setTagged(File* file, QString value) {

    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);
    if (fileTagNodeAssoc) {
        FileTagNodeAssocString* fileTagNodeAssocString = dynamic_cast<FileTagNodeAssocString*>(fileTagNodeAssoc);
        fileTagNodeAssocString->setValue(value);
    } else {
        new FileTagNodeAssocString(file, this, value);
    }
}


bool TagNodeString::tagged(File* file, QString pattern) {

    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);
    if (fileTagNodeAssoc == 0) {
        return false;
    }

    FileTagNodeAssocString* fileTagNodeAssocString = dynamic_cast<FileTagNodeAssocString*>(fileTagNodeAssoc);
    QString value = fileTagNodeAssocString->value();

    tracer->sinvoked("tagged") << "with pattern: '" << pattern << "', value: " << value << endl;

    QRegExp regExp(pattern);
    if (regExp.exactMatch(value)) {
        tracer->sdebug("tagged") <<  "MATCHES!!!" << endl;
        return true;
    }

    tracer->sdebug("tagged") << "NO match!!!" << endl;
    return false;
}
