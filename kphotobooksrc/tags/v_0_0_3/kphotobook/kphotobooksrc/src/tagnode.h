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

#include <kdebug.h>
#include <klocale.h>

#include <qobject.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qptrlist.h>

class FileTagNodeAssoc;
class File;

/**
 * The superclass for all different taypes of tagnodes.
 *
 * CVS-ID $Id$
 */
class TagNode : public QObject {

Q_OBJECT

public:
    static const int TYPE_INVALID = -1;
    static const int TYPE_TITLE = 1;
    static const int TYPE_BOOLEAN = 2;

public:
    /**
     * Instantiates a concrete subclass of this class TagNode.
     */
    static TagNode* createInstance(int typeId, unsigned int id, const QString& text, const QString& iconName, TagNode* parent = 0);

    /**
     * Returns the unequivocal string describing the type of the specified tagnode.
     */
    static QString tagNodeType(int typeId) {

        switch(typeId) {
        case TYPE_TITLE:
            return "title";
        case TYPE_BOOLEAN:
            return "boolean";
        default:
            return QString::null;
        }
    }

    /**
     * Returns the translated text describing the type of the specified tagnode.
     */
    static QString tagNodeTypeName(int typeId){

        switch(typeId) {
        case TYPE_TITLE:
            return i18n("tagNodeTypeName", "title");
        case TYPE_BOOLEAN:
            return i18n("tagNodeTypeName", "boolean");
        default:
            return QString::null;
        }
    }

    static int tagNodeTypeId(int typeId) {

        return typeId;
    }

    /**
     * Returns the type-id of the tagnode type with the specified type.
     * If the specified type is invalid, TYPE_INVALID is returned.
     */
    static int tagNodeTypeId(const QString& type) {

        if (type == "title") {
            return TYPE_TITLE;
        } else if (type == "boolean") {
            return TYPE_BOOLEAN;
        }

        return TYPE_INVALID;
    }

protected:
    TagNode(unsigned int id, const QString& text, const QString& iconName, TagNode* parent = 0);

public:
    virtual ~TagNode();

    void setParent(TagNode* parent);
    TagNode* parent() {
        return m_parent;
    }

    QPtrList<TagNode>* children() {
        return m_children;
    }

    /**
     * Returns the unequivocal typeid representing the concrete type of this tagnode.
     */
    int typeId() {
        return m_typeId;
    }

    /**
     * Returns the unequivocal string describing the concrete type of this tagnode.
     */
    QString* type() {
        return m_type;
    }

    /**
     * Returns the translated text describing the concrete type of this tagnode.
     */
    QString typeName() {
        return *m_typeName;
    }

    unsigned int id() {
        return m_id;
    }

    void setText(const QString& text) {
        delete m_text;
        m_text = new QString(text);
    }
    QString* text() {
        return m_text;
    }

    void setIconName(const QString& iconName);
    QString* iconName() {
        return m_iconName;
    }

    QPixmap* icon() {
        return m_icon;
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
    FileTagNodeAssoc* getAssoc(File* file);

    /**
     * Returns all associations this tagnode has to files.
     */
    QPtrList<FileTagNodeAssoc>* assocs() {
        return m_assocs;
    }

    QString toString() {
        return QString("id: %1, name: >%2<").arg(m_id).arg(*m_text);
    }

signals:
    void changed();

protected:
    /**
     * The id of the type.
     */
    int m_typeId;

    /**
     * The type of this tagnode.
     */
    QString* m_type;

    /**
     * The translated typename.
     */
    QString* m_typeName;

    /**
     * The unique id of this tagnode.
     */
    unsigned int m_id;

    /**
     * The text to display for this tagnode.
     */
    QString* m_text;

    /**
     * The name or the path of the icon to use.
     */
    QString* m_iconName;

    /**
     * The icon to be displayed. Can be null if no icon is set.
     */
    QPixmap* m_icon;

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
