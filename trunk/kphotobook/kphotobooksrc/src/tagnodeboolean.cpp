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

#include "tagnodeboolean.h"

#include "filetagnodeassoc.h"
#include "filetagnodeassocboolean.h"

#include <kdebug.h>


TagNodeBoolean::TagNodeBoolean(unsigned int id, const QString& text, const QString& iconName, TagNode* parent)
    : TagNode(id, text, iconName, parent) {

    kdDebug() << "[TagNodeBoolean::TagNodeBoolean] invoked with id: " << id << "text: " << text << ", icon: " << iconName << endl;
}


void TagNodeBoolean::setTagged(File* file, bool tagged) {

    FileTagNodeAssoc* fileTagNodeAssoc = getAssoc(file);
    if (fileTagNodeAssoc) {
        FileTagNodeAssocBoolean* fileTagNodeAssocBoolean = dynamic_cast<FileTagNodeAssocBoolean*>(fileTagNodeAssoc);
        fileTagNodeAssocBoolean->setValue(tagged);
    } else {
        new FileTagNodeAssocBoolean(file, this, tagged);
    }
}


bool TagNodeBoolean::tagged(File* file) {

    FileTagNodeAssoc* fileTagNodeAssoc = getAssoc(file);
    if (fileTagNodeAssoc == 0) {
        return false;
    }

    FileTagNodeAssocBoolean* fileTagNodeAssocBoolean = dynamic_cast<FileTagNodeAssocBoolean*>(fileTagNodeAssoc);

    return fileTagNodeAssocBoolean->value();
}


bool TagNodeBoolean::taggedRecursive(File* file) {

    bool isTagged = tagged(file);

    if (!isTagged && m_children) {
        // loop over all children
        TagNode* child;
        for (child = m_children->first(); child; child = m_children->next()) {
            if (typeid(*child) == typeid(TagNodeBoolean)) {
                TagNodeBoolean* tagNodeBooleanChild = dynamic_cast<TagNodeBoolean*>(child);

                isTagged = tagNodeBooleanChild->taggedRecursive(file);
                // abort loop as soon as found a tagged child
                if (isTagged) {
                    break;
                }
            }
        }
    }

    return isTagged;
}
