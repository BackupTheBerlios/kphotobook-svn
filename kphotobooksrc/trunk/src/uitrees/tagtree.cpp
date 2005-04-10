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

#include "../constants.h"
#include "../settings/settings.h"

#include "tagtreenode.h"
#include "../kphotobook.h"
#include "../engine/tagnode.h"
#include "../engine/tagnodetitle.h"
#include "../engine/tagnodeboolean.h"
#include "../engine/tagnodestring.h"
#include "../engine/tagnoderadiogroup.h"
#include "../engine/tagnoderadio.h"
#include "tagtreenodetitle.h"
#include "tagtreenodeboolean.h"
#include "tagtreenodestring.h"
#include "tagtreenoderadiogroup.h"
#include "tagtreenoderadio.h"

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
    , m_photobook(photobook)
    , m_toolTip(this) {

    setFont(Settings::tagTreeFont());

    // create columns
    addColumn(i18n("Tags"));
    addColumn(i18n("Value"));
    addColumn(i18n("Filter"));

    // we set the alignment to center to force redrawing the wohle cell always
    setColumnAlignment (1, Qt::AlignCenter);
    setColumnAlignment (2, Qt::AlignCenter);

    // we want that the first column gets as big as possible
    header()->setStretchEnabled( true, 0);

    setSelectionMode(QListView::NoSelection);

    // the root node must be closeable
    setRootIsDecorated(true);

    // append a listener for mouseclicks...
    QObject::connect(this, SIGNAL(mouseButtonClicked(int, QListViewItem*, const QPoint&, int)),
                     this, SLOT(slotListViewClicked(int, QListViewItem*, const QPoint&, int)));
    QObject::connect(this, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)),
                     this, SLOT(slotListViewDoubleClicked(QListViewItem*, const QPoint&, int)));

    // append the listener for renaming items
    QObject::connect(this, SIGNAL(itemRenamed(QListViewItem*, int, const QString&)),
                     this, SLOT(slotItemRenamed(QListViewItem*, int, const QString&)));

    //TODO choenig: I'd like to implement such thing so the data is accepted on loosing focus in the input
    // fields. But simply adding the DefaultAction is not enough, since there comes is a problem
    // when clicking on another image: the new image is selected, and afterwards, the data is saved
    // (accepted) which leads to the point, that the newly selected Item is maked with the setting.
    //setDefaultRenameAction(QListView::Accept);
}


void TagTree::expandCurrent(bool recursive) {

    if (recursive) {
        dynamic_cast<TagTreeNode*>(currentItem())->setOpenRecursive(true);
    } else {
        currentItem()->setOpen(true);
    }
}
void TagTree::collapseCurrent(bool recursive) {

    if (recursive) {
        dynamic_cast<TagTreeNode*>(currentItem())->setOpenRecursive(false);
    } else {
        currentItem()->setOpen(false);
    }
}
void TagTree::expandAll() {

    QListViewItemIterator it(this);
    while (it.current()) {
        TagTreeNode* item = dynamic_cast<TagTreeNode*>(it.current());

        item->setOpenRecursive(true);

        ++it;
    }
}
void TagTree::collapseAll() {

    QListViewItemIterator it(this);
    while (it.current()) {
        TagTreeNode* item = dynamic_cast<TagTreeNode*>(it.current());

        item->setOpenRecursive(false);

        ++it;
    }
}


void TagTree::addTagNodes(QPtrList<TagNode>* rootNodeList) {

    TagNode* rootNode;
    for ( rootNode = rootNodeList->first(); rootNode; rootNode = rootNodeList->next() ) {
        addTagNode(rootNode);
    }
}


void TagTree::addTagNode(TagNode* rootNode) {

    kdDebug() << "[TagTree::addTagNode] Converting subtree with root node: " << *rootNode->text() << "..." << endl;

    TagTreeNode* tagTreeNode = 0;

    // create the concrete tagtreenode
    if (typeid(*rootNode) == typeid(TagNodeTitle)) {
        TagNodeTitle* node = dynamic_cast<TagNodeTitle*>(rootNode);
        tagTreeNode = new TagTreeNodeTitle(this, node, m_photobook, m_photobook->contextMenuTagTreeItem());
    } else if (typeid(*rootNode) == typeid(TagNodeBoolean)) {
        TagNodeBoolean* node = dynamic_cast<TagNodeBoolean*>(rootNode);
        tagTreeNode = new TagTreeNodeBoolean(this, node, m_photobook, m_photobook->contextMenuTagTreeItem());
    } else if (typeid(*rootNode) == typeid(TagNodeString)) {
        TagNodeString* node = dynamic_cast<TagNodeString*>(rootNode);
        tagTreeNode = new TagTreeNodeString(this, node, m_photobook, m_photobook->contextMenuTagTreeItemLeaf());
    } else if (typeid(*rootNode) == typeid(TagNodeRadioGroup)) {
        TagNodeRadioGroup* node = dynamic_cast<TagNodeRadioGroup*>(rootNode);
        tagTreeNode = new TagTreeNodeRadioGroup(this, node, m_photobook, m_photobook->contextMenuTagTreeItem());
    } else if (typeid(*rootNode) == typeid(TagNodeRadio)) {
        TagNodeRadio* node = dynamic_cast<TagNodeRadio*>(rootNode);
        tagTreeNode = new TagTreeNodeRadio(this, node, m_photobook, m_photobook->contextMenuTagTreeItemLeaf());
    } else {
        kdWarning() << "[TagTree::addTagNode] unknown root tagtype received: " << rootNode->type() << "!"<< endl;
    }

    // build the whole tree
    buildTagNodeTree(tagTreeNode, rootNode->children());
}


void TagTree::addTagNode(TagTreeNode* parent, TagNode* child) {

    kdDebug() << "[TagTree::addTagNode] Converting node: " << *child->text() << "..." << endl;

    TagTreeNode* tagTreeNode = 0;

    // create the concrete tagtreenode
    if (typeid(*child) == typeid(TagNodeTitle)) {
        TagNodeTitle* node = dynamic_cast<TagNodeTitle*>(child);
        tagTreeNode = new TagTreeNodeTitle(parent, node, m_photobook, m_photobook->contextMenuTagTreeItem());
    } else if (typeid(*child) == typeid(TagNodeBoolean)) {
        TagNodeBoolean* node = dynamic_cast<TagNodeBoolean*>(child);
        tagTreeNode = new TagTreeNodeBoolean(parent, node, m_photobook, m_photobook->contextMenuTagTreeItem());
    } else if (typeid(*child) == typeid(TagNodeString)) {
        TagNodeString* node = dynamic_cast<TagNodeString*>(child);
        tagTreeNode = new TagTreeNodeString(parent, node, m_photobook, m_photobook->contextMenuTagTreeItemLeaf());
    } else if (typeid(*child) == typeid(TagNodeRadioGroup)) {
        TagNodeRadioGroup* node = dynamic_cast<TagNodeRadioGroup*>(child);
        tagTreeNode = new TagTreeNodeRadioGroup(parent, node, m_photobook, m_photobook->contextMenuTagTreeItem());
    } else if (typeid(*child) == typeid(TagNodeRadio)) {
        TagNodeRadio* node = dynamic_cast<TagNodeRadio*>(child);
        tagTreeNode = new TagTreeNodeRadio(parent, node, m_photobook, m_photobook->contextMenuTagTreeItemLeaf());
    } else {
        kdWarning() << "[TagTree::addTagNode] unknown sub tagtype received: " << child->type() << "!"<< endl;
    }

    // build the whole tree
    buildTagNodeTree(tagTreeNode, child->children());
}


void TagTree::deselectFilter() {

    QListViewItemIterator it(this);
    while (it.current()) {
        TagTreeNode* item = dynamic_cast<TagTreeNode*>(it.current());

        item->deselectFilter();

        ++it;
    }
}


void TagTree::resetFilter() {

    QListViewItemIterator it(this);
    while (it.current()) {
        TagTreeNode* item = dynamic_cast<TagTreeNode*>(it.current());

        item->resetFilter();

        ++it;
    }
}


void TagTree::doRepaintAll() {
    QListViewItemIterator it(this);
    while (it.current()) {

        dynamic_cast<TagTreeNode*>(it.current())->refresh();
        this->repaintItem(it.current());

        ++it;
    }
}


QStringList* TagTree::getOpenNodes() {

    QStringList* openNodes = new QStringList();

    // loop over *all* nodes in the tree
    QListViewItemIterator it(this);
    while (it.current()) {

        TagTreeNode* node = dynamic_cast<TagTreeNode*>(it.current());
        // add the node to the list if it is open
        if (node->isOpen()) {
            openNodes->append(QString::number(node->tagNode()->id()));
        }

        ++it;
    }
    
    return openNodes;
}


void TagTree::openNodes(QStringList* openNodes) {

    // loop over *all* nodes in the tree
    QListViewItemIterator it(this);
    while (it.current()) {

        TagTreeNode* node = dynamic_cast<TagTreeNode*>(it.current());
        
        // open the current node if it is in the list
        QString nodeIdStr = QString::number(node->tagNode()->id());
        uint removedItems = openNodes->remove(nodeIdStr);
        node->setOpen(removedItems > 0);

        ++it;
    }
}


QIntDict<QString>* TagTree::getFilter() {

    QIntDict<QString>* filterList = new QIntDict<QString>();
    
    // loop over *all* nodes in the tree
    QListViewItemIterator it(this);
    while (it.current()) {

        TagTreeNode* node = dynamic_cast<TagTreeNode*>(it.current());
        // add the filterstring to the filter list
        QString filter = node->getFilterString();
        if (filter != QString::null) {
            filterList->insert(node->tagNode()->id(), new QString(filter));
        }

        ++it;
    }
    
    return filterList;
}


void TagTree::applyFilter(QIntDict<QString>* filterList) {

    // loop over *all* nodes in the tree
    QListViewItemIterator it(this);
    while (it.current()) {

        TagTreeNode* node = dynamic_cast<TagTreeNode*>(it.current());
        // find the filter and apply it to the node
        QString* filter = filterList->find(node->tagNode()->id());
        if (filter != 0) {
            node->applyFilterString(*filter);
        }

        ++it;
    }
}


//
// public slots
//
void TagTree::slotLoadSettings() {

    setFont(Settings::tagTreeFont());
    doRepaintAll();
}


//
// protected slots
//
void TagTree::keyPressEvent(QKeyEvent* e) {

    if (e->key() == Qt::Key_Control) {
        m_photobook->startTemporaryUnlockTagging();
    }
    
    e->ignore();
}

void TagTree::keyReleaseEvent(QKeyEvent *e) {

    if (e->key() == Qt::Key_Control) {
        m_photobook->stopTemporaryUnlockTagging();
    }
    
    e->ignore();
}

void TagTree::focusOutEvent(__attribute__((unused)) QFocusEvent *e) {

    m_photobook->stopTemporaryUnlockTagging();
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

    if (button == Qt::RightButton && item != 0) {
        dynamic_cast<TagTreeNode*>(item)->rightClicked(this, column);
        // info: we do not use SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)
        // because we want the contextMenu only displayed on column 0
    }

    // inform the tagtreenode about the click
    if (button == Qt::LeftButton && item != 0) {
        dynamic_cast<TagTreeNode*>(item)->leftClicked(this, column);
    }
}


void TagTree::slotItemRenamed(QListViewItem* item, int column, const QString& text) {

    kdDebug() << "[TagTree::slotItemRenamed] invoked with text: >" << text << "<" << endl;

    if (typeid(*item) == typeid(TagTreeNodeString)) {

        TagTreeNodeString* tagTreeNode = dynamic_cast<TagTreeNodeString*>(item);
        tagTreeNode->handleRenaming(column, text);
    } else {
        kdWarning() << "[TagTree::slotItemRenamed] unknown item received: " << item->text(0) << endl;
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


//
// responsible for showing the tooltip in the tree
//
void TreeToolTip::maybeTip( const QPoint &point )
{
    QListViewItem *item = m_view->itemAt( point );
    if ( item ) {
        QString text = static_cast<TagTreeNode*>( item )->toolTip();
        if ( !text.isEmpty() ) {
            tip ( m_view->itemRect( item ), text );
        }
    }
}
