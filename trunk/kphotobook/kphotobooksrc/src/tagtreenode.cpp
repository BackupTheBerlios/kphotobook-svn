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

#include "tagtreenode.h"

#include "kphotobook.h"
#include "treehelper.h"

#include "tagtree.h"
#include "tagnode.h"


#include <qstyle.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qcursor.h>

#include <typeinfo>


TagTreeNode::TagTreeNode(TagTree* parent, QString text, KPhotoBook* photobook, QPixmap* icon, KPopupMenu* contextMenu)
    : KListViewItem(parent, text.prepend(" ")), m_photobook(photobook), m_contextMenu(contextMenu) {

    // set the icon if specified
    if (icon) {
        this->setPixmap(TagTree::COLUMN_TEXT, *icon);
    }

    this->setText(TagTree::COLUMN_VALUE, "");
    this->setText(TagTree::COLUMN_FILTER, "");
}


TagTreeNode::TagTreeNode(TagTreeNode* parent, QString text, KPhotoBook* photobook, QPixmap* icon, KPopupMenu* contextMenu)
    : KListViewItem(parent, text.prepend(" ")), m_photobook(photobook), m_contextMenu(contextMenu) {

    // set the icon if specified
    if (icon) {
        this->setPixmap(TagTree::COLUMN_TEXT, *icon);
    }

    this->setText(TagTree::COLUMN_VALUE, "");
    this->setText(TagTree::COLUMN_FILTER, "");
}


void TagTreeNode::refresh() {

    if (tagNode()->icon()) {
        this->setPixmap(TagTree::COLUMN_TEXT, *tagNode()->icon());
    } else {
        this->setPixmap(TagTree::COLUMN_TEXT, 0);
    }

    this->setText(TagTree::COLUMN_TEXT, QString(*tagNode()->text()).prepend(" "));
}


void TagTreeNode::setOpenRecursive(bool open) {
    setOpen(open);

    // do recursive call on every child
    TagTreeNode* child = dynamic_cast<TagTreeNode*>(this->firstChild());
    while(child) {
        child->setOpen(open);
        child = dynamic_cast<TagTreeNode*>(child->nextSibling());
    }
}


void TagTreeNode::rightClicked(__attribute__((unused)) TagTree* tagTree, __attribute__((unused)) int column) {

    if (column == TagTree::COLUMN_TEXT && m_contextMenu) {
        m_contextMenu->exec(QCursor::pos());
    }
}
