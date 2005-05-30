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


#include "tagtreenoderadiogroup.h"
#include "tagtree.h"
#include "treehelper.h"
#include "../engine/file.h"
#include "../engine/filternodetagradio.h"
#include "../engine/tagnoderadio.h"
#include "../engine/tagnoderadio.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"

#include <kfileitem.h>

TagTreeNodeRadio::TagTreeNodeRadio(TagTree* parent, TagNodeRadio* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu)
    , m_filterState(TagTreeNodeRadio::FILTERSTATE_IGNORE) {
}


TagTreeNodeRadio::TagTreeNodeRadio(TagTreeNode* parent, TagNodeRadio* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu)
        , m_filterState(TagTreeNodeRadio::FILTERSTATE_IGNORE) {
}


TagTreeNodeRadio::~TagTreeNodeRadio() {
}


FilterNode* TagTreeNodeRadio::subfilter() {

    TagNodeRadio* tagNode = dynamic_cast<TagNodeRadio*>(m_tagNode);

    FilterNode* filter = 0;

    switch (m_filterState) {
        case FILTERSTATE_EXCLUDE:
            filter = new FilterNodeTagRadio(tagNode, false);
            break;
        case FILTERSTATE_IGNORE:
            filter = 0;
            break;
        case FILTERSTATE_INCLUDE:
            filter = new FilterNodeTagRadio(tagNode, true);
            break;
    }

    return filter;
}


QString TagTreeNodeRadio::getFilterString() {

    QString filter;

    switch (m_filterState) {
    case FILTERSTATE_EXCLUDE:
        filter = "exclude";
        break;
    case FILTERSTATE_INCLUDE:
        filter = "include";
        break;
    case FILTERSTATE_IGNORE:
        break;
    }

    return filter;
}


void TagTreeNodeRadio::applyFilterString(QString filter) {

    if (filter == "exclude") {
        m_filterState = FILTERSTATE_EXCLUDE;
    } else if (filter == "include") {
        m_filterState = FILTERSTATE_INCLUDE;
    } else {
        m_filterState = FILTERSTATE_IGNORE;
    }
}


void TagTreeNodeRadio::leftClicked(__attribute__((unused)) TagTree* tagTree, int column) {

    switch (column) {
    case TagTree::COLUMN_TEXT :
        break;

    case TagTree::COLUMN_VALUE : {

        // do nothing when tagging is locked
        if (Settings::tagTreeLocked()) {
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
    case TagTree::COLUMN_FILTER :
        // change state of the filter: exclude -> ignore -> include -> exclude -> ...
        switch (m_filterState) {
        case FILTERSTATE_EXCLUDE:
            m_filterState = FILTERSTATE_IGNORE;
            break;
        case FILTERSTATE_IGNORE:
            m_filterState = FILTERSTATE_INCLUDE;
            break;
        case FILTERSTATE_INCLUDE:
            m_filterState = FILTERSTATE_EXCLUDE;
            break;
        }

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


void TagTreeNodeRadio::rightClicked(__attribute__((unused)) TagTree* tagTree, int column) {

    switch (column) {
    case TagTree::COLUMN_TEXT :
        if (m_contextMenu) {
            m_contextMenu->exec(QCursor::pos());
        }
        break;

    case TagTree::COLUMN_FILTER :
        // change state of the filter: exclude -> include -> ignore -> exclude -> ...
        switch (m_filterState) {
        case FILTERSTATE_EXCLUDE:
            m_filterState = FILTERSTATE_INCLUDE;
            break;
        case FILTERSTATE_IGNORE:
            m_filterState = FILTERSTATE_EXCLUDE;
            break;
        case FILTERSTATE_INCLUDE:
            m_filterState = FILTERSTATE_IGNORE;
            break;
        }

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


void TagTreeNodeRadio::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {

    switch (column) {
    case TagTree::COLUMN_TEXT :
        TagTreeNode::paintCell(p, cg, column, width, alignment);
        break;

    case TagTree::COLUMN_VALUE : {
        // paint the cell with the alternating background color
        p->fillRect(0, 0, width, this->height(), backgroundColor(1));

        // draw the checkbox in the center of the cell in the size of the font
        int size = p->fontInfo().pixelSize()+2;
        QRect rect((width - size + 4)/2, (  this->height()-size )/2, size, size);

        if (m_tagCurrentMatch == TagTreeNode::NOSELECT) {
            TreeHelper::drawRadioButton(p, cg, rect, (int)TagTreeNode::UNTAGGED, false);
        } else {
            TreeHelper::drawRadioButton(p, cg, rect, (int)m_tagCurrentMatch, !Settings::tagTreeLocked());
        }

        break;
    }

    case TagTree::COLUMN_FILTER :
        // paint the cell with the alternating background color
        p->fillRect(0, 0, width, this->height(), backgroundColor(2));

        // draw the checkbox in the center of the cell in the size of the font
        int size = p->fontInfo().pixelSize()+2;
        QRect rect((width - size + 4)/2, (  this->height()-size )/2, size, size);

        TreeHelper::drawCheckBox(p, cg, rect, m_filterState, true);

        break;
    }
}
