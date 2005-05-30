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

#include "tagtreenodestring.h"

#include "../engine/tagnodestring.h"
#include "../engine/filternodetagstring.h"
#include "../engine/filetagnodeassocstring.h"
#include "tagtree.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"
#include "../engine/file.h"
#include "treehelper.h"

#include <kfileitem.h>

TagTreeNodeString::TagTreeNodeString(TagTree* parent, TagNodeString* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu)
    , m_filterValue(QString::null) {

    // enable editing of value and filter
    setRenameEnabled(TagTree::COLUMN_VALUE, true);
    setRenameEnabled(TagTree::COLUMN_FILTER, true);
}


TagTreeNodeString::TagTreeNodeString(TagTreeNode* parent, TagNodeString* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu)
    , m_filterValue(QString::null) {

    // enable editing of value and filter
    setRenameEnabled(TagTree::COLUMN_VALUE, true);
    setRenameEnabled(TagTree::COLUMN_FILTER, true);
}


TagTreeNodeString::~TagTreeNodeString() {
}


FilterNode* TagTreeNodeString::filter() {

    if (m_filterValue.isEmpty()) {
        return 0;
    }

    TagNodeString* tagNode = dynamic_cast<TagNodeString*>(m_tagNode);
    return new FilterNodeTagString(tagNode, m_filterValue);
}


void TagTreeNodeString::deselectFilter() {

    m_filterValue = QString("()");
    setText(TagTree::COLUMN_FILTER, m_filterValue);

    // force redrawing of this listviewitem
    this->repaint();
}


void TagTreeNodeString::resetFilter() {

    m_filterValue = QString::null;
    setText(TagTree::COLUMN_FILTER, m_filterValue);

    // force redrawing of this listviewitem
    this->repaint();
}


QString TagTreeNodeString::getFilterString() {

    return m_filterValue;
}


void TagTreeNodeString::applyFilterString(QString filter) {

    m_filterValue = filter;
    setText(TagTree::COLUMN_FILTER, filter);
}


void TagTreeNodeString::leftClicked(__attribute__((unused)) TagTree* tagTree, int column) {

    switch (column) {
    case TagTree::COLUMN_TEXT :
        break;

    case TagTree::COLUMN_VALUE : {

        // do nothing when tagging is locked
        if (Settings::tagTreeLocked()) {
            return;
        }

        // get all selected files
        const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

        if (selectedFiles->count()) {
            // let the user enter a new value
            startRename(TagTree::COLUMN_VALUE);
        }
        break;
    }

    case TagTree::COLUMN_FILTER :
        startRename(TagTree::COLUMN_FILTER);
        break;
    }
}


void TagTreeNodeString::rightClicked(__attribute__((unused)) TagTree* tagTree, int column) {

    switch (column) {
    case TagTree::COLUMN_TEXT :
        if (m_contextMenu) {
            m_contextMenu->exec(QCursor::pos());
        }
        break;

    case TagTree::COLUMN_FILTER :
        break;
    }
}


void TagTreeNodeString::handleRenaming(int column, const QString& text) {

    TagNodeString* tagNode = dynamic_cast<TagNodeString*>(m_tagNode);

    switch (column) {
    case TagTree::COLUMN_TEXT :
        break;

    case TagTree::COLUMN_VALUE : {


        QPtrListIterator<KFileItem> it(*m_photobook->view()->fileView()->selectedItems());

        // loop over all selected files and change their state
        it.toFirst();
        for (; it.current(); ++it) {
            File* selectedFile = dynamic_cast<File*>(it.current());

            tagNode->setTagged(selectedFile, text);
        }

        //update my internal state...
        m_tagCurrentMatch = text.isEmpty() ? TagTreeNode::UNTAGGED : TagTreeNode::TAGGED;

        //and then update all of my parents
        TagTreeNode* node = this;
        while (node) {
            node->recursiveFindTagged();
            node->repaint();
            node = dynamic_cast<TagTreeNode*>(node->parent());
        }

        m_photobook->dirtyfy();
        break;
    }
    case TagTree::COLUMN_FILTER :
        // filter has changed --> update the text in the node and auto refresh view
        setFilterValue(text);

        m_photobook->autoRefreshView();
        break;
    }
}


void TagTreeNodeString::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {

    TagNodeString* tagNode = dynamic_cast<TagNodeString*>(m_tagNode);

    switch (column) {
    case TagTree::COLUMN_TEXT :
    case TagTree::COLUMN_FILTER :
        TagTreeNode::paintCell(p, cg, column, width, alignment);
        break;

    case TagTree::COLUMN_VALUE : {

        QString text = "";
        if (m_tagCurrentMatch == TagTreeNode::MIXTAGGED) {
            text = "---";
        } else {

            // get all selected files
            const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

            if (selectedFiles->count()) {

                QPtrListIterator<KFileItem> it(*m_photobook->view()->fileView()->selectedItems());
                for (; it.current(); ++it) {
                    File* selectedFile = dynamic_cast<File*>(it.current());

                    FileTagNodeAssocString* assoc = dynamic_cast<FileTagNodeAssocString*>(tagNode->getAssocToFile(selectedFile));
                    if (assoc) {
                        // ok we got an assoc remember it

                        if (!text.isEmpty() && assoc->valueAsString() != text) {
                            // abort is not all selected images have the same tagvalue!
                            text = "---";
                            break;
                        }
                        text = assoc->valueAsString();
                    } else {
                        // no assoc found -> remove the text
                        if (!text.isEmpty()) {
                        // abort is not all selected images have the same tagvalue!
                            text = "---";
                            break;
                        }
                        text = "";
                    }
                }
            }
        }

        setText(TagTree::COLUMN_VALUE, text);

        KListViewItem::paintCell(p, cg, column, width, alignment);
        break;
    }
    }
}
