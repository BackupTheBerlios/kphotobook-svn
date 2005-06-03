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

private:
    static Tracer* tracer;

public:
    TagTreeNodeRadio(TagTree* parent, TagNodeRadio* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    TagTreeNodeRadio(TagTreeNode* parent, TagNodeRadio* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    virtual ~TagTreeNodeRadio();

    /**
     * Returns an FilterNode used to describe this filter.
     */
    FilterNode* subfilter();

    virtual void leftClicked(TagTree* tagTree, int column);
    virtual void rightClicked(TagTree* tagTree, int column);

    virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment);

    void setState(bool selected) {
        m_tagCurrentMatch = (selected ? TagTreeNode::TAGGED : TagTreeNode::UNTAGGED);
    }
};

#endif
