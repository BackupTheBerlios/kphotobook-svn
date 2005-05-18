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

#ifndef FILTERNODE_H
#define FILTERNODE_H

#include "../tracer/tracer.h"
#include "../exception.h"
#include "file.h"

#include <qobject.h>
#include <qptrlist.h>

/**
 * This is the generic superclass for all concrete FilterNodes.
 * FilterNodes are used to create a tree representing a filter for the engine.
 * A node can be an operator or a tag expression...
 * 
 * CVS-ID $Id: tagnode.h 307 2005-04-17 12:47:26Z starcube $
 */
class FilterNode : public QObject {

Q_OBJECT

protected:
    static Tracer* dumper;

public:
    FilterNode(FilterNode* parent = 0);

    ~FilterNode();

    /**
     * Adds the specified child to this parent and sets the parent of the child.
     */
    void addChild(FilterNode* child);

    /**
     * Returns the number of children this FilterNode has.
     */
    bool childrenCount() {
        
        return m_children->count();
    }
    
    /**
     * Evaluates if this subtree or leafnode matches the given file.
     */
    virtual bool evaluate(File* file) throw(FilterException*) = 0;

    /**
     * Prints the value of this Filternode and calls this method with a increased
     * indention on each child.
     * The method is intended for debugging the filter.
     *
     * @indention the indention to put before the text.
     */
    virtual void dump(QString indention = QString("")) = 0;

protected:
    /**
     * The parent of this node.
     */
    FilterNode* m_parent;

    /**
     * The list of children.
     */
    QPtrList<FilterNode>* m_children;
};

#endif
