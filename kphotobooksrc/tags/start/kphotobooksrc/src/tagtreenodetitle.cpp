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

#include "tagtreenodetitle.h"

#include "tagnodetitle.h"


TagTreeNodeTitle::TagTreeNodeTitle(KListView* parent, TagNodeTitle* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, *tagNode->text(), photobook, tagNode->icon(), contextMenu), m_tagNode(tagNode) {
}


TagTreeNodeTitle::TagTreeNodeTitle(KListViewItem* parent, TagNodeTitle* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, *tagNode->text(), photobook, tagNode->icon(), contextMenu), m_tagNode(tagNode) {
}


TagTreeNodeTitle::~TagTreeNodeTitle() {
}


TagNode* TagTreeNodeTitle::tagNode() {
    return m_tagNode;
}

