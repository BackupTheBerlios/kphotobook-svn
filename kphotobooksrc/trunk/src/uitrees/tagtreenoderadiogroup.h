/***************************************************************************
 *   Copyright (C) 2005 by Christian hoenig                                *
 *   kphotobook@mail.hoenig.cc                                             *
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

#ifndef TAGTREENODERADIOGROUP_H
#define TAGTREENODERADIOGROUP_H

#include "../tracer/tracer.h"

#include "tagtreenode.h"
#include "tagtreenoderadio.h"

class TagNodeRadioGroup;

/**
 * Concrete tagtreenode for displaying a radiogroup.
 *
 * CVS-ID $Id: tagtreenoderadiogroup.h 274 2005-03-25 08:52:15Z choenig $
 */
class TagTreeNodeRadioGroup : public TagTreeNode {

private:
    static Tracer* tracer;

public:
    enum FilterState {FILTERSTATE_EXCLUDE = -1, FILTERSTATE_IGNORE, FILTERSTATE_INCLUDE };

    TagTreeNodeRadioGroup(TagTree* parent, TagNodeRadioGroup* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    TagTreeNodeRadioGroup(TagTreeNode* parent, TagNodeRadioGroup* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    virtual ~TagTreeNodeRadioGroup();

    /**
     * Returns an FilterNode used to describe this filter.
     */
    virtual FilterNode* filter();

    /**
     * Sets the filter to find images without this tag set.
     */
    virtual void deselectFilter() {
        m_filterState = FILTERSTATE_EXCLUDE;

        // force redrawing of this listviewitem
        this->repaint();
    }

    /**
     * Resets the filter.
     */
    virtual void resetFilter() {
        m_filterState = FILTERSTATE_IGNORE;

        // force redrawing of this listviewitem
        this->repaint();
    }

    virtual void leftClicked( TagTree* tagTree, int column);
    virtual void rightClicked(TagTree* tagTree, int column);

    virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment);

    /**
     * sets the selected child to src
     */
    void setSelectedTag(TagTreeNodeRadio* src);

    /**
     * updates the filterstate of this group depending on its children states
     */
    void updateFilterState();

private:
    int m_filterState;
};

#endif
