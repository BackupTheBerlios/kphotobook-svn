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

#ifndef TAGTREENODETITLE_H
#define TAGTREENODETITLE_H

#include "tagtreenode.h"

class TagNodeTitle;

/**
 * Concrete tagtreenode for displaying a title.
 *
 * CVS-ID $Id: tagtreenodetitle.h,v 1.1 2004/03/07 18:52:19 starcube Exp $
 */
class TagTreeNodeTitle : public TagTreeNode {

public:
    TagTreeNodeTitle(KListView* parent, TagNodeTitle* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    TagTreeNodeTitle(KListViewItem* parent, TagNodeTitle* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    virtual ~TagTreeNodeTitle();

    TagNode* tagNode();

    virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment) {
        KListViewItem::paintCell(p, cg, column, width, alignment);
    }

private:
    TagNodeTitle* m_tagNode;
};

#endif
