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

#ifndef TAGTREENODESTRING_H
#define TAGTREENODESTRING_H

#include "tagtreenode.h"

class TagNodeString;

/**
 * Concrete tagtreenode for displaying a string tagnode.
 *
 * CVS-ID $Id: tagtreenodeboolean.h 274 2005-03-25 08:52:15Z choenig $
 */
class TagTreeNodeString : public TagTreeNode {

public:
    TagTreeNodeString(TagTree* parent, TagNodeString* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    TagTreeNodeString(TagTreeNode* parent, TagNodeString* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    virtual ~TagTreeNodeString();

    /**
     * Returns an expression used to describe this filter.
     * This expression is used to create the filter used by the engine to determine which images to display.
     * Normally this expression contains a tagnode and a value.
     */
    QString filter();

    /**
     * Sets the filter to find images without this tag set.
     */
    virtual void deselectFilter() {
        m_filterValue = QString("");

        // force redrawing of this listviewitem
        this->repaint();
    }

    /**
     * Resets the filter.
     */
    virtual void resetFilter() {
        m_filterValue = QString::null;

        // force redrawing of this listviewitem
        this->repaint();
    }

    /**
     * Returns the currently set filter as string representation.
     * This value is used to store in the database.
     */
    virtual QString getFilterString();
    
    /**
     * Applies the filter returned by getFilter().
     */
    virtual void applyFilterString(QString filter);

    virtual void leftClicked(TagTree* tagTree, int column);
    virtual void rightClicked(TagTree* tagTree, int column);

    virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment);
private:
    QString m_filterValue;
};

#endif
