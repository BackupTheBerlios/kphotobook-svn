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

#include "tagtreenodeboolean.h"

#include "tagnodeboolean.h"
#include "tagtree.h"


TagTreeNodeBoolean::TagTreeNodeBoolean(KListView* parent, TagNodeBoolean* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, *tagNode->text(), photobook, tagNode->icon(), contextMenu), m_tagNode(tagNode) {
}


TagTreeNodeBoolean::TagTreeNodeBoolean(KListViewItem* parent, TagNodeBoolean* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, *tagNode->text(), photobook, tagNode->icon(), contextMenu), m_tagNode(tagNode) {
}


TagTreeNodeBoolean::~TagTreeNodeBoolean() {
}


TagNode* TagTreeNodeBoolean::tagNode() {
    return m_tagNode;
}


void TagTreeNodeBoolean::columnClicked(__attribute__((unused)) TagTree* tagTree, int column) {

    switch (column) {
    case TagTree::COLUMN_TEXT:
        break;

    case TagTree::COLUMN_VALUE:
        // TODO: to something
//        m_tagNode->setTagged(!m_tagNode->tagged());
        // force redrawing of this listviewitem
        this->repaint();
        break;

    case TagTree::COLUMN_FILTER:
        m_tagNode->setFiltered(!m_tagNode->filtered());
        // force redrawing of this listviewitem
        this->repaint();
        break;
    }
}

void TagTreeNodeBoolean::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {

    if (column == TagTree::COLUMN_TEXT) {
        KListViewItem::paintCell(p, cg, column, width, alignment);
    } else {

        // paint the cell with the alternating background color
        p->fillRect(0, 0, width, this->height(), backgroundColor());

        // draw the checkbox in the center of the cell
        QRect rect((width-this->height()+4)/2, 2, this->height()-4, this->height()-4);

        if (column == TagTree::COLUMN_VALUE) {
            // TODO: to something
//            drawCheckBox(p, cg, rect, m_tagNode->tagged());
            drawCheckBox(p, cg, rect, true);
        } else {
            drawCheckBox(p, cg, rect, m_tagNode->filtered());
        }
    }
}
