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

#include "tagtreenoderadio.h"


#include "tagtree.h"
#include "tagtreenoderadiogroup.h"
#include "treehelper.h"
#include "../engine/file.h"
#include "../engine/filternodetagradio.h"
#include "../engine/tagnoderadio.h"
#include "../engine/tagnoderadio.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"
#include "../settings/settings.h"

#include <kfileitem.h>


Tracer* TagTreeNodeRadio::tracer = Tracer::getInstance("kde.kphotobook.uitrees", "TagTreeNodeRadio");


TagTreeNodeRadio::TagTreeNodeRadio(TagTree* parent, TagNodeRadio* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu)
{

}


TagTreeNodeRadio::TagTreeNodeRadio(TagTreeNode* parent, TagNodeRadio* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu)
{
}


TagTreeNodeRadio::~TagTreeNodeRadio()
{
}


FilterNode* TagTreeNodeRadio::subfilter() const
{
    tracer->invoked(__func__);

    TagNodeRadio* tagNode = dynamic_cast<TagNodeRadio*>(m_tagNode);

    FilterNode* filter = 0;

    switch (m_filterState) {
        case FILTERSTATE_EXCLUDE: {
            filter = new FilterNodeTagRadio(tagNode, false);
            break;
        }
        case FILTERSTATE_IGNORE: {
            filter = 0;
            break;
        }
        case FILTERSTATE_INCLUDE: {
            filter = new FilterNodeTagRadio(tagNode, true);
            break;
        }
    }

    return filter;
}


void TagTreeNodeRadio::leftClicked(TagTree* tagTree, int column)
{
    tracer->invoked(__func__);

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

            // otherwise tell my parent (which is my radiogroup managing item) that i have been selected
            TagTreeNodeRadioGroup* grp = dynamic_cast<TagTreeNodeRadioGroup*>(parent());
            if (grp) {
                // if it's already selected, we unselect all
                grp->setSelectedTag(m_tagCurrentMatch == TagTreeNode::TAGGED ? 0L : this);
            }

            break;
        }
        case TagTree::COLUMN_FILTER: {
            TagTreeNode::leftClicked(tagTree, column);

            //now tell my father, that smth happened to me
            TagTreeNodeRadioGroup* grp = dynamic_cast<TagTreeNodeRadioGroup*>(parent());
            if (grp) {
                grp->updateFilterState();
            }

            // force redrawing of this listviewitem
            this->repaint();

            m_photobook->autoRefreshView();
            break;
        }
    }
}


void TagTreeNodeRadio::rightClicked(TagTree* tagTree, int column)
{
    tracer->invoked(__func__);

    switch (column) {
        case TagTree::COLUMN_TEXT: {
            TagTreeNode::rightClicked(tagTree, column);
            break;
        }
        case TagTree::COLUMN_FILTER: {
            TagTreeNode::rightClicked(tagTree, column);

            //now tell my father, that smth happened to me
            TagTreeNodeRadioGroup* grp = dynamic_cast<TagTreeNodeRadioGroup*>(parent());
            if (grp) {
                grp->updateFilterState();
            }

            // force redrawing of this listviewitem
            this->repaint();

            m_photobook->autoRefreshView();
            break;
        }
    }
}


void TagTreeNodeRadio::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    switch (column) {
        case TagTree::COLUMN_TEXT: {
            TagTreeNode::paintCell(p, cg, column, width, alignment);
            break;
        }
        case TagTree::COLUMN_VALUE: {
            if (m_tagCurrentMatch == TagTreeNode::NOSELECT) {
                TreeHelper::drawRadioButton(p, cg, backgroundColor(TagTree::COLUMN_VALUE), width, this->height(), false, TreeHelper::NOT_CHECKED);
            } else {
                TreeHelper::drawRadioButton(p, cg, backgroundColor(TagTree::COLUMN_VALUE), width, this->height(), !Settings::tagTreeLocked(), (TreeHelper::TRISTATE)m_tagCurrentMatch);
            }
            break;
        }
        case TagTree::COLUMN_FILTER: {
            TreeHelper::drawRadioButton(p, cg, backgroundColor(TagTree::COLUMN_FILTER), width, this->height(), true, (TreeHelper::TRISTATE)m_filterState);
            break;
        }
    }
}
