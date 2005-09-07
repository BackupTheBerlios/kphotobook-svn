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

#include "sourcedirtree.h"

#include "../constants.h"
#include "../engine/file.h"
#include "../engine/folder.h"
#include "../kphotobook.h"
#include "../settings/settings.h"
#include "../uitrees/sourcedirtreenode.h"
#include "../utils/menuprovider.h"
#include "../tracer/tracer.h"

#include <kfileitem.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstdaccel.h>

#include <qcursor.h>
#include <qdir.h>
#include <qheader.h>
#include <qintdict.h>
#include <qobjectlist.h>
#include <qwidget.h>

#include <typeinfo>


Tracer* SourceDirTree::tracer = Tracer::getInstance("kde.kphotobook.uitrees", "SourceDirTree");


SourceDirTree::SourceDirTree( QWidget* parent, KPhotoBook* photobook, const char* name ) :
    KListView( parent, name ),
    m_photobook(photobook),
    m_sourceDirNodeDict(new QIntDict<SourceDirTreeNode>)
{
    setFont(Settings::sourceDirTreeFont());

    // create columns
    addColumn(i18n("Source directories"));
    addColumn(i18n("Selected"));
    addColumn(i18n("Include"));

    // we set the alignment to center to force redrawing the wohle cell always
    setColumnAlignment(COLUMN_TEXT, Qt::AlignLeft);
    setColumnAlignment(COLUMN_SELECTED, Qt::AlignCenter);
    setColumnAlignment(COLUMN_INCLUDED, Qt::AlignCenter);

    // we want that the first column gets as big as possible
    header()->setStretchEnabled(true, COLUMN_TEXT);

    // we cannot select a row
    setSelectionMode(QListView::NoSelection);

    // the root node must be closeable
    setRootIsDecorated(true);

    // append a listener for mouseclicks... ;-)
    QObject::connect(this, SIGNAL(mouseButtonClicked(int, QListViewItem*, const QPoint&, int)),
                     this, SLOT(slotListViewClicked(int, QListViewItem*, const QPoint&, int)));
    QObject::connect(this, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)),
                     this, SLOT(slotListViewDoubleClicked(QListViewItem*, const QPoint&, int)));
}


SourceDirTree::~SourceDirTree()
{
    delete m_sourceDirNodeDict;
}


void SourceDirTree::expandCurrent(bool recursive)
{
    if (recursive) {
        dynamic_cast<SourceDirTreeNode*>(currentItem())->setOpenRecursive(true);
    } else {
        currentItem()->setOpen(true);
    }
}


void SourceDirTree::collapseCurrent(bool recursive)
{
    if (recursive) {
        dynamic_cast<SourceDirTreeNode*>(currentItem())->setOpenRecursive(false);
    } else {
        currentItem()->setOpen(false);
    }
}


void SourceDirTree::expandAll()
{
    QListViewItemIterator it(this);
    while (it.current()) {
        SourceDirTreeNode* item = dynamic_cast<SourceDirTreeNode*>(it.current());

        item->setOpenRecursive(true);

        ++it;
    }
}


void SourceDirTree::collapseAll()
{
    QListViewItemIterator it(this);
    while (it.current()) {
        SourceDirTreeNode* item = dynamic_cast<SourceDirTreeNode*>(it.current());

        item->setOpenRecursive(false);

        ++it;
    }
}


void SourceDirTree::includeWholeSourceDir()
{
    dynamic_cast<SourceDirTreeNode*>(currentItem())->setIncludedRecursive(true);
}


void SourceDirTree::excludeWholeSourceDir()
{
    dynamic_cast<SourceDirTreeNode*>(currentItem())->setIncludedRecursive(false);
}


void SourceDirTree::invertSourceDir()
{
    dynamic_cast<SourceDirTreeNode*>(currentItem())->invertInclusionRecursive();
}


void SourceDirTree::includeAllSourceDirs()
{
    QListViewItemIterator it(this);
    while (it.current()) {
        SourceDirTreeNode* item = dynamic_cast<SourceDirTreeNode*>(it.current());

        item->setIncluded(true);

        ++it;
    }
}


void SourceDirTree::excludeAllSourceDirs()
{
    QListViewItemIterator it(this);
    while (it.current()) {
        SourceDirTreeNode* item = dynamic_cast<SourceDirTreeNode*>(it.current());

        item->setIncluded(false);

        ++it;
    }
}


void SourceDirTree::invertAllSourceDirs()
{
    QListViewItemIterator it(this);
    while (it.current()) {
        SourceDirTreeNode* item = dynamic_cast<SourceDirTreeNode*>(it.current());

        item->invertInclusion();

        ++it;
    }
}


void SourceDirTree::addSourceDirs(QPtrList<Folder>* rootNodeList)
{
    Folder* rootNode;
    for ( rootNode = rootNodeList->first(); rootNode; rootNode = rootNodeList->next() ) {
        addSourceDir(rootNode);
    }
}


void SourceDirTree::removeSourceDir(SourceDirTreeNode* node)
{
    m_sourceDirNodeDict->remove(node->sourceDir()->id());
    delete node;
}


void SourceDirTree::reflectSelectedFiles(const KFileItemList* selectedFiles)
{
    // reset the number of selected files on all sourcedirnodes to 0
    QListViewItemIterator it(this);
    while (it.current()) {
        SourceDirTreeNode* item = dynamic_cast<SourceDirTreeNode*>(it.current());

        item->setSelectedFilesCount(0);

        ++it;
    }

    // loop over all selected files
    QPtrListIterator<KFileItem> it1(*selectedFiles);
    for (; it1.current(); ++it1) {
        File* selectedFile = dynamic_cast<File*>(it1.current());

        // get the sourcedirnode we belong to and increase its number of selected files
        SourceDirTreeNode* item = m_sourceDirNodeDict->find(selectedFile->sourceDir()->id());

        item->increaseSelectedFilesCount(1);
        item->repaint();
    }
}


SourceDirTreeNode* SourceDirTree::selectedSourceDir()
{
    if (typeid(*currentItem()) != typeid(SourceDirTreeNode)) {
        tracer->swarning(__func__) << "sourcedirTree contains a node of other type than 'SourceDirTreeNode'" << endl;
        return 0;
    }

    return dynamic_cast<SourceDirTreeNode*>(currentItem());
}


void SourceDirTree::doRepaintAll()
{
    QListViewItemIterator it(this);
    while (it.current()) {

        dynamic_cast<SourceDirTreeNode*>(it.current())->refreshIcon();
        this->repaintItem(it.current());

        ++it;
    }
}


QStringList* SourceDirTree::getOpenNodes()
{
    QStringList* openNodes = new QStringList();

    // loop over *all* nodes in the tree
    QListViewItemIterator it(this);
    while (it.current()) {

        SourceDirTreeNode* node = dynamic_cast<SourceDirTreeNode*>(it.current());
        // add the node to the list if it is open
        if (node->isOpen()) {
            openNodes->append(QString::number(node->sourceDir()->id()));
        }

        ++it;
    }

    return openNodes;
}


void SourceDirTree::openNodes(QStringList* openNodes)
{
    // loop over *all* nodes in the tree
    QListViewItemIterator it(this);
    while (it.current()) {

        SourceDirTreeNode* node = dynamic_cast<SourceDirTreeNode*>(it.current());

        // open the current node if it is in the list
        QString nodeIdStr = QString::number(node->sourceDir()->id());
        uint removedItems = openNodes->remove(nodeIdStr);
        node->setOpen(removedItems > 0);

        ++it;
    }
}


QIntDict<QString>* SourceDirTree::getFilter()
{
    QIntDict<QString>* filterList = new QIntDict<QString>();

    // loop over *all* nodes in the tree
    QListViewItemIterator it(this);
    while (it.current()) {

        SourceDirTreeNode* node = dynamic_cast<SourceDirTreeNode*>(it.current());
        // add the filterstring to the filter list
        QString filter = node->getFilterString();
        if (filter != QString::null) {
            filterList->insert(node->sourceDir()->id(), new QString(filter));
        }

        ++it;
    }

    return filterList;
}


void SourceDirTree::applyFilter(QIntDict<QString>* filterList)
{
    // loop over *all* nodes in the tree
    QListViewItemIterator it(this);
    while (it.current()) {

        SourceDirTreeNode* node = dynamic_cast<SourceDirTreeNode*>(it.current());
        // find the filter and apply it to the node
        QString* filter = filterList->find(node->sourceDir()->id());
        if (filter != 0) {
            node->applyFilterString(*filter);
        }

        ++it;
    }
}


//
// public slots
//
void SourceDirTree::slotLoadSettings()
{
    setFont(Settings::sourceDirTreeFont());
    doRepaintAll();
}


void SourceDirTree::slotAddFolder(Folder* folder)
{
    tracer->invoked(__func__);
    if (!folder) {
        tracer->swarning(__func__) << "called with 0!" << endl;
        return;
    }

    // this is a root node...
    addSourceDir(folder);
}


//
// private slots
//
void SourceDirTree::slotListViewDoubleClicked(QListViewItem* item, const QPoint& /* point */, int column)
{
    if (column == SourceDirTree::COLUMN_TEXT) {
        setOpen(item, !isOpen(item));
    }
}


void SourceDirTree::slotListViewClicked(int button, QListViewItem* item, const QPoint& /* point */, int column)
{
    if (button == Qt::RightButton && item == 0) {
        // show contextMenu if right clicked on no item
        m_photobook->menus()->contextMenuSourceDirTree()->exec(QCursor::pos());

        // info: we do not use SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)
        // because we want the contextMenu only displayed on column 0
    }

    if (button == Qt::RightButton && item != 0) {
        dynamic_cast<SourceDirTreeNode*>(item)->rightClicked(0, column);
        // info: we do not use SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)
        // because we want the contextMenu only displayed on column 0
    }

    // inform the tagtreenode about the click
    if (button == Qt::LeftButton && item != 0) {
        dynamic_cast<SourceDirTreeNode*>(item)->leftClicked(0, column);
    }
}


//
// private methods
//
void SourceDirTree::addSourceDir(Folder* rootNode)
{
    tracer->sinvoked(__func__) << " with folder: '" << rootNode->dir()->absPath() << "'..." << endl;

    SourceDirTreeNode* sourceDirTreeNode = new SourceDirTreeNode(this, m_photobook, rootNode, m_photobook->menus()->contextMenuSourceDir());

    // insert the just created node into the dictionary
    m_sourceDirNodeDict->insert(rootNode->id(), sourceDirTreeNode);

    // build the whole tree
    buildSourceDirTree(sourceDirTreeNode, rootNode->children());
}


void SourceDirTree::buildSourceDirTree(SourceDirTreeNode* parent, QPtrList<Folder>* children)
{
    // test if there are children
    if (!children || !children->count()) {
        return;
    }

    // build the subtree of this node
    Folder* child;
    for (child = children->first(); child; child = children->next() ) {

        SourceDirTreeNode* sourceDirTreeNode = new SourceDirTreeNode(parent, m_photobook, child, m_photobook->menus()->contextMenuSubDir());

        // insert the just created node into the dictionary
        m_sourceDirNodeDict->insert(child->id(), sourceDirTreeNode);

        // build the whole tree
        buildSourceDirTree(sourceDirTreeNode, child->children());
    }
}


#include "sourcedirtree.moc"
