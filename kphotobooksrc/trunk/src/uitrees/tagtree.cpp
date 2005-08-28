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

#include "tagtreenode.h"
#include "tagtreenodeboolean.h"
#include "tagtreenodedatetime.h"
#include "tagtreenoderadiogroup.h"
#include "tagtreenoderadio.h"
#include "tagtreenodestring.h"
#include "tagtreenodetitle.h"
#include "../constants.h"
#include "../engine/engine.h"
#include "../engine/tagnode.h"
#include "../engine/tagnodeboolean.h"
#include "../engine/tagnodedatetime.h"
#include "../engine/tagnoderadiogroup.h"
#include "../engine/tagnoderadio.h"
#include "../engine/tagnodestring.h"
#include "../engine/tagnodetitle.h"
#include "../kphotobook.h"
#include "../settings/settings.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kstdaccel.h>

#include <qcursor.h>
#include <qheader.h>
#include <qobjectlist.h>
#include <qwidget.h>

#include <typeinfo>


Tracer* TagTree::tracer = Tracer::getInstance("kde.kphotobook.uitrees", "TagTree");


TagTree::TagTree(QWidget* parent, KPhotoBook* photobook, const char* name) :
    KListView(parent, name),
    m_photobook(photobook),
    m_toolTip(this)
{
    setFont(Settings::tagTreeFont());

    // create columns
    addColumn(i18n("Tags"));
    addColumn(i18n("Value"));
    addColumn(i18n("Filter"));

    // we set the alignment to center to force redrawing the whole cell always
    setColumnAlignment(COLUMN_VALUE, Qt::AlignCenter);
    setColumnAlignment(COLUMN_FILTER, Qt::AlignCenter);

    // do never automatically change width of columns
    setColumnWidthMode(COLUMN_VALUE, QListView::Manual);
    setColumnWidthMode(COLUMN_FILTER, QListView::Manual);

    // we want that the first column gets as big as possible
    header()->setStretchEnabled(true, COLUMN_TEXT);

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

    ///@todo choenig: I'd like to implement such thing so the data is accepted on loosing focus in the input
    /// fields. But simply adding the DefaultAction is not enough, since there comes is a problem
    /// when clicking on another image: the new image is selected, and afterwards, the data is saved
    /// (accepted) which leads to the point, that the newly selected Item is maked with the setting.
//     setDefaultRenameAction(QListView::Accept);
}


void TagTree::expandCurrent(bool recursive)
{
    if (recursive) {
        dynamic_cast<TagTreeNode*>(currentItem())->setOpenRecursive(true);
    } else {
        currentItem()->setOpen(true);
    }
}


void TagTree::collapseCurrent(bool recursive)
{
    if (recursive) {
        dynamic_cast<TagTreeNode*>(currentItem())->setOpenRecursive(false);
    } else {
        currentItem()->setOpen(false);
    }
}


void TagTree::expandAll()
{
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


void TagTree::addTagNodes(QPtrList<TagNode>* rootNodeList)
{
    TagNode* rootNode;
    for ( rootNode = rootNodeList->first(); rootNode; rootNode = rootNodeList->next() ) {
        addTagNode(rootNode);
    }
}


void TagTree::deselectFilter()
{
    QListViewItemIterator it(this);
    while (it.current()) {
        TagTreeNode* item = dynamic_cast<TagTreeNode*>(it.current());

        item->deselectFilter();

        ++it;
    }

    // ignore EXIF tags!
    QListViewItem * child = m_exifTagTreeNode->firstChild();
    while (child) {
        static_cast<TagTreeNode*>(child)->resetFilter();
        child = child->nextSibling();
    }
}


void TagTree::resetFilter()
{
    QListViewItemIterator it(this);
    while (it.current()) {
        TagTreeNode* item = dynamic_cast<TagTreeNode*>(it.current());

        item->resetFilter();

        ++it;
    }
}


void TagTree::doRepaintAll()
{
    QListViewItemIterator it(this);
    while (it.current()) {

        dynamic_cast<TagTreeNode*>(it.current())->refresh();
        this->repaintItem(it.current());

        ++it;
    }
}


QStringList* TagTree::getOpenNodes()
{
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


void TagTree::openNodes(QStringList* openNodes)
{
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


QIntDict<QString>* TagTree::getFilter()
{
    QIntDict<QString>* filterList = new QIntDict<QString>();

    // loop over *all* nodes in the tree
    QListViewItemIterator it(this);
    while (it.current()) {

        TagTreeNode* node = dynamic_cast<TagTreeNode*>(it.current());
        // add the filterstring to the filter list
        QString filter = node->filterString();
        if (filter != QString::null) {
            filterList->insert(node->tagNode()->id(), new QString(filter));
        }

        ++it;
    }

    return filterList;
}


void TagTree::applyFilter(QIntDict<QString>* filterList)
{
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
void TagTree::slotLoadSettings()
{
    setFont(Settings::tagTreeFont());
    doRepaintAll();
}


void TagTree::slotAddTagNode(TagNode* tagNode)
{
    if (!tagNode) {
        tracer->swarning(__func__) << "called with 0!" << endl;
        return;
    }

    if (tagNode->parent()) {
        // find the TagTreeNode for the parentTagNode of the given tagNode
        TagTreeNode* parentTagTreeNode = 0;
        
        // loop over *all* nodes in the tree
        QListViewItemIterator it(this);
        while (it.current()) {
            TagTreeNode* node = dynamic_cast<TagTreeNode*>(it.current());
            if (node->tagNode() == tagNode->parent()) {
                parentTagTreeNode = node;
                break;
            }
            ++it;
        }

        // parent tagtreenode not found... abort
        if (!parentTagTreeNode) {
            tracer->swarning(__func__) << "parent TagTreeNode not found!" << endl;
            return;
        }

        // add the given tagnode as child to ots parent...
        addTagNode(parentTagTreeNode, tagNode);
        parentTagTreeNode->setOpen(true);
    } else {
        // this is a root node...
        addTagNode(tagNode);
    }
}


//
// protected slots
//
void TagTree::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Control) {
        m_photobook->startTemporaryUnlockTagging();
    }

    e->ignore();
}


void TagTree::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Control) {
        m_photobook->stopTemporaryUnlockTagging();
    }

    e->ignore();
}


void TagTree::focusOutEvent(__attribute__((unused)) QFocusEvent *e)
{
    m_photobook->stopTemporaryUnlockTagging();
}


//
// private slots
//
void TagTree::slotListViewDoubleClicked(QListViewItem* item, __attribute__((unused)) const QPoint& point, int column)
{
    if (column == TagTree::COLUMN_TEXT) {
        setOpen(item, !isOpen(item));
    }
}


void TagTree::slotListViewClicked(int button, QListViewItem* item, __attribute__((unused)) const QPoint& point, int column)
{
    if (button == Qt::RightButton && item == 0) {
        // show contextMenu if right clicked on no item
        m_photobook->menus()->contextMenuTagTree()->exec(QCursor::pos());

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


void TagTree::slotItemRenamed(QListViewItem* item, int column, const QString& text)
{
    tracer->sinvoked(__func__) << "with text: >" << text << "<" << endl;

    if (typeid(*item) == typeid(TagTreeNodeString)) {
        TagTreeNodeString* tagTreeNode = dynamic_cast<TagTreeNodeString*>(item);
        tagTreeNode->handleRenaming(column, text);
    } else {
        tracer->swarning(__func__) << "unknown item received: " << item->text(0) << endl;
    }
}


//
// private
//
void TagTree::addTagNode(TagNode* rootNode)
{
    tracer->sdebug(__func__) << "Converting subtree with root node: " << *rootNode->text() << "..." << endl;

    TagTreeNode* tagTreeNode = 0;

    // create the concrete tagtreenode
    if (typeid(*rootNode) == typeid(TagNodeTitle)) {
        TagNodeTitle* node = dynamic_cast<TagNodeTitle*>(rootNode);
        tagTreeNode = new TagTreeNodeTitle(this, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItem());
    } else if (typeid(*rootNode) == typeid(TagNodeBoolean)) {
        TagNodeBoolean* node = dynamic_cast<TagNodeBoolean*>(rootNode);
        tagTreeNode = new TagTreeNodeBoolean(this, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItem());
    } else if (typeid(*rootNode) == typeid(TagNodeString)) {
        TagNodeString* node = dynamic_cast<TagNodeString*>(rootNode);
        tagTreeNode = new TagTreeNodeString(this, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItemLeaf());
    } else if (typeid(*rootNode) == typeid(TagNodeRadioGroup)) {
        TagNodeRadioGroup* node = dynamic_cast<TagNodeRadioGroup*>(rootNode);
        tagTreeNode = new TagTreeNodeRadioGroup(this, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItem());
    } else if (typeid(*rootNode) == typeid(TagNodeRadio)) {
        TagNodeRadio* node = dynamic_cast<TagNodeRadio*>(rootNode);
        tagTreeNode = new TagTreeNodeRadio(this, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItemLeaf());
    } else if (typeid(*rootNode) == typeid(TagNodeDateTime)) {
        TagNodeDateTime* node = dynamic_cast<TagNodeDateTime*>(rootNode);
        tagTreeNode = new TagTreeNodeDateTime(this, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItemLeaf());
    } else {
        tracer->swarning(__func__) << "unknown root tagtype received: " << rootNode->type() << "!"<< endl;
    }

    // remember the exif title tagnode
    if (m_photobook->engine()->exifTagNodeTitle() == tagTreeNode->tagNode()) {
        tracer->sdebug(__func__) << "exif title tag found: " << tagTreeNode->text(0) << endl;
        m_exifTagTreeNode = tagTreeNode;
    }
    
    // build the whole tree
    buildTagNodeTree(tagTreeNode, rootNode->children());
}


void TagTree::addTagNode(TagTreeNode* parent, TagNode* child)
{
    tracer->sdebug(__func__) << "Converting node: " << *child->text() << "..." << endl;

    TagTreeNode* tagTreeNode = 0;

    // create the concrete tagtreenode
    if (typeid(*child) == typeid(TagNodeTitle)) {
        TagNodeTitle* node = dynamic_cast<TagNodeTitle*>(child);
        tagTreeNode = new TagTreeNodeTitle(parent, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItem());
    } else if (typeid(*child) == typeid(TagNodeBoolean)) {
        TagNodeBoolean* node = dynamic_cast<TagNodeBoolean*>(child);
        tagTreeNode = new TagTreeNodeBoolean(parent, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItem());
    } else if (typeid(*child) == typeid(TagNodeString)) {
        TagNodeString* node = dynamic_cast<TagNodeString*>(child);
        tagTreeNode = new TagTreeNodeString(parent, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItemLeaf());
    } else if (typeid(*child) == typeid(TagNodeRadioGroup)) {
        TagNodeRadioGroup* node = dynamic_cast<TagNodeRadioGroup*>(child);
        tagTreeNode = new TagTreeNodeRadioGroup(parent, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItem());
    } else if (typeid(*child) == typeid(TagNodeRadio)) {
        TagNodeRadio* node = dynamic_cast<TagNodeRadio*>(child);
        tagTreeNode = new TagTreeNodeRadio(parent, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItemLeaf());
    } else if (typeid(*child) == typeid(TagNodeDateTime)) {
        TagNodeDateTime* node = dynamic_cast<TagNodeDateTime*>(child);
        tagTreeNode = new TagTreeNodeDateTime(parent, node, m_photobook, m_photobook->menus()->contextMenuTagTreeItemLeaf());
    } else {
        tracer->swarning(__func__) << "unknown sub tagtype received: " << child->type() << "!"<< endl;
    }

    // build the whole tree
    buildTagNodeTree(tagTreeNode, child->children());
}


void TagTree::buildTagNodeTree(TagTreeNode* parent, QPtrList<TagNode>* children)
{
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
void TreeToolTip::maybeTip(const QPoint &point)
{
    QListViewItem *item = m_view->itemAt( point );
    if ( item ) {
        int column = m_view->header()->sectionAt(point.x());
        QString text("");
        switch (column) {
            case TagTree::COLUMN_TEXT: {
                text = static_cast<TagTreeNode*>( item )->toolTipText();
                break;
            }
            case TagTree::COLUMN_VALUE: {
                text = static_cast<TagTreeNode*>( item )->toolTipValue();
                break;
            }
            case TagTree::COLUMN_FILTER: {
                text = static_cast<TagTreeNode*>( item )->toolTipFilter();
                break;
            }
        }
        if (!text.isEmpty()) {
            tip(m_view->itemRect(item), text);
        }
    }
}
