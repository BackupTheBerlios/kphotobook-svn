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

#include "tagtree.h"
#include "treehelper.h"
#include "../engine/file.h"
#include "../engine/filternodetagboolean.h"
#include "../engine/tagnodeboolean.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"
#include "../settings/settings.h"

#include <kfileiconview.h>
#include <kfileitem.h>


TagTreeNodeBoolean::TagTreeNodeBoolean(TagTree* parent, TagNodeBoolean* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu)
{
}


TagTreeNodeBoolean::TagTreeNodeBoolean(TagTreeNode* parent, TagNodeBoolean* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu)
{
}


TagTreeNodeBoolean::~TagTreeNodeBoolean()
{
}


FilterNode* TagTreeNodeBoolean::filter() const
{
    TagNodeBoolean* tagNode = dynamic_cast<TagNodeBoolean*>(m_tagNode);

    FilterNode* filter = 0;

    switch (m_filterState) {
        case TagTreeNode::FILTERSTATE_EXCLUDE: {
            filter = new FilterNodeTagBoolean(tagNode, false);
            break;
        }
        case TagTreeNode::FILTERSTATE_IGNORE: {
            filter = 0;
            break;
        }
        case TagTreeNode::FILTERSTATE_INCLUDE: {
            filter = new FilterNodeTagBoolean(tagNode, true);
            break;
        }
    }

    return filter;
}


void TagTreeNodeBoolean::leftClicked(TagTree* tagTree, int column)
{
    TagNodeBoolean* tagNode = dynamic_cast<TagNodeBoolean*>(m_tagNode);

    switch (column) {
        case TagTree::COLUMN_TEXT:
            break;
        case TagTree::COLUMN_VALUE: {
            // do nothing when tagging is locked
            if (Settings::tagTreeLocked()) {
                return;
            }

            // editing of the value is not allowed if tagnode is readonly
            if (m_tagNode->readonly()) {
                return;
            }

            // if not all are currently tagged, tag them, otherwise untag them
            bool newState = (m_tagCurrentMatch != TagTreeNode::TAGGED);

            //now set our new state
            m_tagCurrentMatch = newState ? TagTreeNode::TAGGED : TagTreeNode::UNTAGGED;

            // loop over all selected files and change their state
            QPtrListIterator<KFileItem> it(*m_photobook->view()->fileView()->selectedItems());
            for (it.toFirst(); it.current(); ++it) {
                File* selectedFile = dynamic_cast<File*>(it.current());
                tagNode->setTagged(selectedFile, newState);
            }

            //now loop through all of our parents to set their state according to my changes
            TagTreeNode* node = this;
            while (node) {
                node->recursiveFindTagged();
                node->repaint();
                node = dynamic_cast<TagTreeNode*>(node->parent());
            }

            m_photobook->dirtyfy();
            break;
        }
        case TagTree::COLUMN_FILTER: {
            TagTreeNode::leftClicked(tagTree, column);

            // force redrawing of this listviewitem
            this->repaint();

            m_photobook->autoRefreshView();
            break;
        }
    }
}


void TagTreeNodeBoolean::rightClicked(TagTree* tagTree, int column)
{
    switch (column) {
        case TagTree::COLUMN_TEXT: {
            TagTreeNode::rightClicked(tagTree, column);
            break;
        }
        case TagTree::COLUMN_FILTER: {
            TagTreeNode::rightClicked(tagTree, column);

            // force redrawing of this listviewitem
            this->repaint();

            m_photobook->autoRefreshView();
            break;
        }
    }
}


void TagTreeNodeBoolean::paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment)
{
    switch (column) {
        case TagTree::COLUMN_TEXT: {
            TagTreeNode::paintCell(p, cg, column, width, alignment);
            break;
        }
        case TagTree::COLUMN_VALUE: {
            if (m_tagCurrentMatch == TagTreeNode::NOSELECT) {
                // no file is selected -> draw a disabled checkbox
                TreeHelper::drawCheckBox(p, cg, backgroundColor(TagTree::COLUMN_VALUE), width, this->height(), false, TreeHelper::NOT_CHECKED);
            } else {
                TreeHelper::drawCheckBox(p, cg, backgroundColor(TagTree::COLUMN_VALUE), width, this->height(), !Settings::tagTreeLocked(), (TreeHelper::TRISTATE)m_tagCurrentMatch);
            }
            break;
        }
        case TagTree::COLUMN_FILTER: {
            TreeHelper::drawCheckBox(p, cg, backgroundColor(TagTree::COLUMN_FILTER), width, this->height(), true, (TreeHelper::TRISTATE)m_filterState);
            break;
        }
    }
}
