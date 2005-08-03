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

#include "tagnode.h"

#include "file.h"
#include "filetagnodeassoc.h"
#include "filetagnodeassocboolean.h"
#include "tagnodeboolean.h"
#include "tagnodedatetime.h"
#include "tagnoderadiogroup.h"
#include "tagnoderadio.h"
#include "tagnodestring.h"
#include "tagnodetitle.h"

#include <kglobal.h>
#include <kiconloader.h>

#include <qiconset.h>

#include <typeinfo>


Tracer* TagNode::tracer = Tracer::getInstance("kde.kphotobook.engine", "TagNode");


TagNode* TagNode::createInstance(TagNode::Type typeId, unsigned int id, const QString& text, const QString& comment, const QString& iconName, TagNode* parent)
{
    TagNode* newTagNode = 0;

    // create the new tagnode
    switch(typeId) {
        case TagNode::TYPE_TITLE: {
            newTagNode = new TagNodeTitle(id, text, comment, iconName, parent);
            break;
        }
        case TagNode::TYPE_BOOLEAN: {
            newTagNode = new TagNodeBoolean(id, text, comment, iconName, parent);
            break;
        }
        case TagNode::TYPE_STRING: {
            newTagNode = new TagNodeString(id, text, comment, iconName, parent);
            break;
        }
        case TagNode::TYPE_RADIOGROUP: {
            newTagNode = new TagNodeRadioGroup(id, text, comment, iconName, parent);
            break;
        }
        case TagNode::TYPE_RADIO: {
            newTagNode = new TagNodeRadio(id, text, comment, iconName, parent);
            break;
        }
        case TagNode::TYPE_DATETIME: {
            newTagNode = new TagNodeDateTime(id, text, comment, iconName, parent);
            break;
        }
        default: {
            tracer->swarning(__func__) << "Called with unknown TypeID!" << endl;
            return 0;
        }
    }

    // do some more initialization on the just created concrete tagnode
    newTagNode->m_typeId = typeId;

    return newTagNode;
}


TagNode::TagNode(unsigned int id, const QString& text, const QString& comment, const QString& iconName, TagNode* parent) :
    QObject(0, text.ascii()),
    m_typeId(TagNode::TYPE_INVALID),
    m_id(id),
    m_readonly(false),
    m_text(new QString(text)),
    m_comment(new QString(comment)),
    m_iconName(0),
    m_parent(0),
    m_children(0),
    m_assocs(new QPtrList<FileTagNodeAssoc>())
{
    tracer->sinvoked(__func__) << "with id: " << id << ", text: " << text << ", comment: " << comment << "icon: " << iconName << endl;

    setIconName(iconName);
    if (parent) {
        setParent(parent);
    }
}


TagNode::~TagNode()
{
    // remove this sourcedir from the children list of the parent
    if (m_parent) {
        m_parent->m_children->remove(this);
        m_parent = 0;
    }

    // set the parent of all children to 0
    TagNode* child;
    if (m_children) {
        for (child = m_children->first(); child; child = m_children->next()) {
            child->m_parent = 0;
        }
    }

    // delete every assoc before deleting the assocslist (the destructor of the assoc is removing itself from the assoclist)
    QPtrListIterator<FileTagNodeAssoc> it(*m_assocs);
    FileTagNodeAssoc *currentAssoc;
    while ((currentAssoc = it.current()) != 0) {
        ++it;
        delete currentAssoc;
    }
    delete m_assocs;

    // delete all members
    delete m_text;
    delete m_iconName;
    delete m_children;
}


void TagNode::setParent(TagNode* parent)
{
    if (!parent) {
        tracer->sdebug(__func__) << "no parent specified, doing nothing" << endl;
        return;
    }

    if (m_parent) {
        tracer->sdebug(__func__) << "TagNode '" << this->toString() << "' already has a parent tagnode: '" << parent->toString() << "'" << endl;
        return;
    }

    m_parent = parent;

    // initialize children pointerlist if necessary
    if (!parent->m_children) {
        parent->m_children = new QPtrList<TagNode>;
    }

    // add this sourcedir as child of the parent
    parent->m_children->append(this);
}


TagNode* TagNode::child(QString text)
{
    if (m_children) {
        TagNode* child = 0;
        for (child = m_children->first(); child; child = m_children->next() ) {
            if ((*(child->text())) == text) {
                return child;
            }
        }
    }

    return 0;
}


void TagNode::setIconName(const QString& iconName)
{
    delete m_iconName;
    m_iconName = new QString(iconName);
}


void TagNode::appendAssoc(FileTagNodeAssoc* assoc)
{
    // force this tagNode as tagNode of the association
    assoc->setTagNode(this);

    // add the association to the list of association
    m_assocs->append(assoc);
}


void TagNode::removeAssoc(FileTagNodeAssoc* assoc)
{
    m_assocs->remove(assoc);
}


FileTagNodeAssoc* TagNode::getAssocToFile(File* file)
{
    FileTagNodeAssoc* currentAssoc;
    for ( currentAssoc = m_assocs->first(); currentAssoc; currentAssoc = m_assocs->next() ) {

        if (currentAssoc->file() == file) {
            return currentAssoc;
        }
    }

    return 0;
}


bool TagNode::isLinkedToFile(File* file)
{
    FileTagNodeAssoc* fileTagNodeAssoc = getAssocToFile(file);
    if (fileTagNodeAssoc != 0) {
        // a file is considered to be linked with this tag if there is an association between
        // but if the tag is of type boolean, the assoc must have the value 'true'
        if (typeid(*fileTagNodeAssoc) == typeid(FileTagNodeAssocBoolean)) {
            FileTagNodeAssocBoolean* fileTagNodeAssocBoolean = dynamic_cast<FileTagNodeAssocBoolean*>(fileTagNodeAssoc);
            if (fileTagNodeAssocBoolean->value()) {
                return true;
            }
        } else {
            return true;
        }
    }

    if (m_children) {
        // loop over all children
        TagNode* child;
        for (child = m_children->first(); child; child = m_children->next()) {
            if (child->isLinkedToFile(file)) {
                return true;
            }
        }
    }

    return false;
}
