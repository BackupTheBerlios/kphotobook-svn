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

#include "tagtree.h"

#include "constants.h"
#include "tagtreenode.h"
#include "kphotobook.h"
#include "tagnode.h"
#include "tagnodetitle.h"
#include "tagnodeboolean.h"
#include "tagtreenodetitle.h"
#include "tagtreenodeboolean.h"

#include <klocale.h>
#include <kstdaccel.h>
#include <kiconloader.h>

#include <kdebug.h>

#include <qwidget.h>
#include <qheader.h>
#include <qcursor.h>
#include <qobjectlist.h>

#include <typeinfo>


TagTree::TagTree( QWidget* parent, KPhotoBook* photobook, const char* name )
    : KListView( parent, name )
    , m_photobook(photobook) {

    // create columns
    addColumn(i18n("Tags"));
    addColumn(i18n("Value"));
    addColumn(i18n("Selected"));

    // we set the alignment to center to force redrawing the wohle cell always
    setColumnAlignment (1, Qt::AlignCenter);
    setColumnAlignment (2, Qt::AlignCenter);

    // we want that the first column gets as big as possible
    header()->setStretchEnabled( true, 0);

    setSelectionMode(QListView::NoSelection);

    // the root node must be closeable
    setRootIsDecorated(true);

    // append a listener for mouseclicks... ;-)
    QObject::connect(this, SIGNAL(mouseButtonClicked(int, QListViewItem*, const QPoint&, int)),
                     this, SLOT(slotListViewClicked(int, QListViewItem*, const QPoint&, int)));
    QObject::connect(this, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)),
                     this, SLOT(slotListViewDoubleClicked(QListViewItem*, const QPoint&, int)));
}


void TagTree::addTagNodes(QPtrList<TagNode>* rootNodeList) {

    TagNode* rootNode;
    for ( rootNode = rootNodeList->first(); rootNode; rootNode = rootNodeList->next() ) {
        addTagNode(rootNode);
    }
}


void TagTree::addTagNode(TagNode* rootNode) {

    kdDebug() << "Converting subtree with root node: " << *rootNode->text() << "..." << endl;

    TagTreeNode* tagTreeNode;

    // create the concrete tagtreenode
    if (typeid(*rootNode) == typeid(TagNodeTitle)) {
        TagNodeTitle* node = dynamic_cast<TagNodeTitle*>(rootNode);
        tagTreeNode = new TagTreeNodeTitle(this, node, m_photobook, m_photobook->contextMenuTagTreeItem());
    } else if (typeid(*rootNode) == typeid(TagNodeBoolean)) {
        TagNodeBoolean* node = dynamic_cast<TagNodeBoolean*>(rootNode);
        tagTreeNode = new TagTreeNodeBoolean(this, node, m_photobook, m_photobook->contextMenuTagTreeItem());
    } // else if....


    // build the whole tree
    buildTagNodeTree(tagTreeNode, rootNode->children());
}


void TagTree::addTagNode(TagTreeNode* parent, TagNode* child) {

    kdDebug() << "Converting node: " << *child->text() << "..." << endl;

    TagTreeNode* tagTreeNode;

    // create the concrete tagtreenode
    if (typeid(*child) == typeid(TagNodeTitle)) {
        TagNodeTitle* node = dynamic_cast<TagNodeTitle*>(child);
        tagTreeNode = new TagTreeNodeTitle(parent, node, m_photobook, m_photobook->contextMenuTagTreeItem());
    } else if (typeid(*child) == typeid(TagNodeBoolean)) {
        TagNodeBoolean* node = dynamic_cast<TagNodeBoolean*>(child);
        tagTreeNode = new TagTreeNodeBoolean(parent, node, m_photobook, m_photobook->contextMenuTagTreeItem());
    } // else if....

    // build the whole tree
    buildTagNodeTree(tagTreeNode, child->children());
}


//
// private slots
//
void TagTree::slotListViewDoubleClicked(QListViewItem* item,__attribute__((unused)) const QPoint& point, int column) {

    if (column == TagTree::COLUMN_TEXT) {
        setOpen(item, !isOpen(item));
    }
}


void TagTree::slotListViewClicked(int button, QListViewItem* item,__attribute__((unused)) const QPoint& point, int column) {

    if (button == Qt::RightButton && item == 0) {
        // show contextMenu if right clicked on no item
        m_photobook->contextMenuTagTree()->exec(QCursor::pos());

        // info: we do not use SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)
        // because we want the contextMenu only displayed on column 0
    }

    if (button == Qt::RightButton && item != 0 && column == 0) {

        // show contextMenu if right clicked in the first column
        dynamic_cast<TagTreeNode*>(item)->showContextMenu();

        // info: we do not use SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)
        // because we want the contextMenu only displayed on column 0
    }

    // inform the tagtreenode about the click
    if (button == Qt::LeftButton && item != 0) {
        dynamic_cast<TagTreeNode*>(item)->columnClicked(this, column);
    }
}


//
// private
//
void TagTree::buildTagNodeTree(TagTreeNode* parent, QPtrList<TagNode>* children) {

    // test if there are children
    if (!children) {
        return;
    }

    // build the subtree of this node
    TagNode* child;
    for (child = children->first(); child; child = children->next() ) {

        this->addTagNode(parent, child);
    }
}



