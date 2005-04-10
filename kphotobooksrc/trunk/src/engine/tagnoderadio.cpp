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

#include "tagnoderadio.h"

#include "filetagnodeassoc.h"
#include "filetagnodeassocradio.h"

#include <kdebug.h>


TagNodeRadio::TagNodeRadio(unsigned int id, const QString& text, const QString& comment, const QString& iconName, TagNode* parent)
    : TagNode(id, text, comment, iconName, parent) {

    kdDebug() << "[TagNodeRadio::TagNodeRadio] invoked with id: " << id << "text: " << text << ", icon: " << iconName << endl;
}


void TagNodeRadio::setTagged(File* file, bool tagged) {

    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);
    if (fileTagNodeAssoc) {
        FileTagNodeAssocRadio* fileTagNodeAssocRadio = dynamic_cast<FileTagNodeAssocRadio*>(fileTagNodeAssoc);
        fileTagNodeAssocRadio->setValue(tagged);
    } else {
        new FileTagNodeAssocRadio(file, this, tagged);
    }
}


bool TagNodeRadio::tagged(File* file) {

    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);
    if (fileTagNodeAssoc == 0) {
        return false;
    }
    
    FileTagNodeAssocRadio* fileTagNodeAssocRadio = dynamic_cast<FileTagNodeAssocRadio*>(fileTagNodeAssoc);
 
    return fileTagNodeAssocRadio->value();
}
