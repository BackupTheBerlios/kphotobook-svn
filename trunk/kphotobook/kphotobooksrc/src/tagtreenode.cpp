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
#include "settings.h"
#include "treehelper.h"

#include "tagtree.h"
#include "tagnode.h"

#include <kglobal.h>
#include <kiconloader.h>

#include <qstyle.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qcursor.h>

#include <typeinfo>


TagTreeNode::TagTreeNode(TagTree* parent, KPhotoBook* photobook, TagNode* tagNode, KPopupMenu* contextMenu)
    : KListViewItem(parent)//, text.prepend(" "))
    , m_photobook(photobook)
    , m_tagNode(tagNode)
    , m_contextMenu(contextMenu) {

    this->setText(TagTree::COLUMN_VALUE, "");
    this->setText(TagTree::COLUMN_FILTER, "");

    refresh();
}


TagTreeNode::TagTreeNode(TagTreeNode* parent, KPhotoBook* photobook, TagNode* tagNode, KPopupMenu* contextMenu)
    : KListViewItem(parent)//, text.prepend(" "))
    , m_photobook(photobook)
    , m_tagNode(tagNode)
    , m_contextMenu(contextMenu) {

    this->setText(TagTree::COLUMN_VALUE, "");
    this->setText(TagTree::COLUMN_FILTER, "");

    refresh();
}


void TagTreeNode::refresh() {

    if (Settings::tagTreeShowIcons()) {
        QIconSet iconSet = KGlobal::iconLoader()->loadIconSet(*m_tagNode->iconName(), KIcon::Small, Settings::tagTreeIconSize(), true);
        if (iconSet.isNull()) {
            if (!(*m_tagNode->iconName()).isEmpty()) {
                kdDebug() << "[TagTreeNode::refresh] Could not load iconset with iconname: '" << *m_tagNode->iconName() << "'" << endl;
            }
            this->setPixmap(TagTree::COLUMN_TEXT, 0);
        } else {
            this->setPixmap(TagTree::COLUMN_TEXT, iconSet.pixmap());
        }
    } else {
        this->setPixmap(TagTree::COLUMN_TEXT, 0);
    }

    this->setText(TagTree::COLUMN_TEXT, QString(*m_tagNode->text()).prepend(" "));
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
