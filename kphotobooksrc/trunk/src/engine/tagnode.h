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

#ifndef TAGNODE_H
#define TAGNODE_H

#include "../tracer/tracer.h"

#include <klocale.h>

#include <qobject.h>
#include <qpixmap.h>
#include <qptrlist.h>
#include <qstring.h>

class FileTagNodeAssoc;
class File;


/**
 * The superclass for all different taypes of tagnodes.
 *
 * CVS-ID $Id$
 */
class TagNode : public QObject
{
    Q_OBJECT
    
    private:
        static Tracer* tracer;
    
    public:
    
        enum Type {
            TYPE_INVALID = -1,
            TYPE_TITLE,
            TYPE_BOOLEAN,
            TYPE_STRING,
            TYPE_RADIOGROUP,
            TYPE_RADIO,
            TYPE_DATETIME
        };
    
    public:
        /**
        * Instantiates a concrete subclass of this class TagNode.
        */
        static TagNode* createInstance(TagNode::Type typeId, unsigned int id, const QString& text, const QString& comment, const QString& iconName, TagNode* parent = 0);
    
        /**
        * Returns the unequivocal string describing the type of the specified tagnode.
        */
        static QString tagNodeType( TagNode::Type typeId )
        {
            switch ( typeId ) {
                case TagNode::TYPE_TITLE:
                    return "title";
                case TagNode::TYPE_BOOLEAN:
                    return "boolean";
                case TagNode::TYPE_STRING:
                    return "string";
                case TagNode::TYPE_RADIOGROUP:
                    return "radiogroup";
                case TagNode::TYPE_RADIO:
                    return "radio";
                case TagNode::TYPE_DATETIME:
                    return "datetime";
                default:
                    return QString::null;
            }
        }
    
        /**
        * Returns the translated text describing the type of the specified tagnode.
        */
        static QString tagNodeTypeName(TagNode::Type typeId)
        {
            switch(typeId) {
            case TagNode::TYPE_TITLE:
                return i18n("tagNodeTypeName", "title");
            case TagNode::TYPE_BOOLEAN:
                return i18n("tagNodeTypeName", "boolean");
            case TagNode::TYPE_STRING:
                return i18n("tagNodeTypeName", "string");
            case TagNode::TYPE_RADIOGROUP:
                return i18n("tagNodeTypeName", "radiogroup");
            case TagNode::TYPE_RADIO:
                return i18n("tagNodeTypeName", "radio");
            case TagNode::TYPE_DATETIME:
                return i18n("tagNodeTypeName", "datetime");
            default:
                return QString::null;
            }
        }
    
        /**
        * Returns the type-id of the tagnode type with the specified type.
        * If the specified type is invalid, TYPE_INVALID is returned.
        */
        static TagNode::Type tagNodeTypeId(const QString& type)
        {
            if (type == "title") {
                return TagNode::TYPE_TITLE;
            } else if (type == "boolean") {
                return TagNode::TYPE_BOOLEAN;
            } else if (type == "string") {
                return TagNode::TYPE_STRING;
            } else if (type == "radiogroup") {
                return TagNode::TYPE_RADIOGROUP;
            } else if (type == "radio") {
                return TagNode::TYPE_RADIO;
            } else if (type == "datetime") {
                return TagNode::TYPE_DATETIME;
            }
    
            return TagNode::TYPE_INVALID;
        }
    
        virtual bool tagged(__attribute__((unused))File* file)
        {
            return false;
        }
    
    protected:
        TagNode(unsigned int id, const QString& text, const QString& comment, const QString& iconName, TagNode* parent = 0);
    
    public:
        virtual ~TagNode();
    
        void setParent(TagNode* parent);
        TagNode* parent()
        {
            return m_parent;
        }
    
        QPtrList<TagNode>* children()
        {
            return m_children;
        }
        /**
        * Returns the child with the given text as text or 0l if no such child was found.
        */
        TagNode* child(QString text);
    
        /**
        * Returns the unequivocal typeid representing the concrete type of this tagnode.
        */
        TagNode::Type typeId()
        {
            return m_typeId;
        }
    
        /**
        * Returns the unequivocal string describing the concrete type of this tagnode.
        */
        QString type()
        {
            return tagNodeType(m_typeId);
        }
    
        /**
        * Returns the translated text describing the concrete type of this tagnode.
        */
        QString typeName()
        {
            return tagNodeTypeName(m_typeId);
        }
    
        unsigned int id()
        {
            return m_id;
        }
    
        void setReadonly(bool readonly)
        {
            m_readonly = readonly;
        }
    
        bool readonly()
        {
            return m_readonly;
        }
    
        void setText(const QString& text)
        {
            delete m_text;
            m_text = new QString(text);
        }
        QString* text()
        {
            return m_text;
        }
    
        void setComment(const QString& comment)
        {
            delete m_comment;
            m_comment = new QString(comment);
        }
        QString* comment()
        {
            return m_comment;
        }
    
        void setSecret(bool s)
        {
            m_bIsSecret = s;
        }
        bool secret()
        {
            return m_bIsSecret;
        }
    
        void setIconName(const QString& iconName);
        QString* iconName()
        {
            return m_iconName;
        }
    
        /**
        * Appends the specified assoc to the list of associations for this tagnode.
        */
        void appendAssoc(FileTagNodeAssoc* assoc);
    
        /**
        * Removes the specified assoc from the list of assocs.
        */
        void removeAssoc(FileTagNodeAssoc* assoc);
    
        /**
        * Returns the association between this tagnode and the specified file, if there
        * exists one.
        */
        FileTagNodeAssoc* getAssocToFile(File* file);
    
        /**
        * Tests if this tagnode or a tagnode below this tagnode has an association to the
        * specified file.
        */
        bool isLinkedToFile(File* file);
    
        /**
        * Returns all associations this tagnode has to files.
        */
        QPtrList<FileTagNodeAssoc>* assocs()
        {
            return m_assocs;
        }
    
        QString toString()
        {
            return QString("id: %1, name: >%2<").arg(m_id).arg(*m_text);
        }
    
    signals:
        void changed();
    
    protected:
        /**
        * The id of the type.
        */
        TagNode::Type m_typeId;
    
        /**
        * The unique id of this tagnode.
        */
        unsigned int m_id;
    
        /**
        * This field is true if the tag is readonly. The value of a readonly tag
        * cannot be changed. This is used mainly for EXIF-tags.
        * The default is false.
        */
        bool m_readonly;
    
        /**
        * The text to display for this tagnode.
        */
        QString* m_text;
    
        /**
        * The comment of this tagnode.
        */
        QString* m_comment;
    
        /**
        * defines this node as node handled with special secrecy
        */
        bool m_bIsSecret;
    
        /**
        * The name or the path of the icon to use.
        */
        QString* m_iconName;
    
        /**
        * The parent of this tagnode. May be null.
        */
        TagNode* m_parent;
    
        /**
        * The list of children.
        */
        QPtrList<TagNode>* m_children;
    
        /**
        * The list of all associations this tagnode has a reference to.
        */
        QPtrList<FileTagNodeAssoc>* m_assocs;
};

#endif
