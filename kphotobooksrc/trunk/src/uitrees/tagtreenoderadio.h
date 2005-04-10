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

#ifndef TAGTREENODERADIO_H
#define TAGTREENODERADIO_H

#include "tagtreenode.h"

class TagNodeRadio;

/**
 * Concrete tagtreenode for displaying a boolean tagnode.
 *
 * CVS-ID $Id: tagtreenodeboolean.h 289 2005-04-03 19:35:11Z starcube $
 */
class TagTreeNodeRadio : public TagTreeNode {

public:
    enum FilterState {FILTERSTATE_EXCLUDE = -1, FILTERSTATE_IGNORE, FILTERSTATE_INCLUDE };   

public:
    TagTreeNodeRadio(TagTree* parent, TagNodeRadio* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    TagTreeNodeRadio(TagTreeNode* parent, TagNodeRadio* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    virtual ~TagTreeNodeRadio();

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
        m_filterState = TagTreeNodeRadio::FILTERSTATE_EXCLUDE;

        // force redrawing of this listviewitem
        this->repaint();
    }

    /**
     * Resets the filter.
     */
    virtual void resetFilter() {
        m_filterState = TagTreeNodeRadio::FILTERSTATE_IGNORE;

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
    TagTreeNodeRadio::FilterState m_filterState;
};

#endif
