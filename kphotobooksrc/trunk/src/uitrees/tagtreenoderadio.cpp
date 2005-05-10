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

#include "../settings/settings.h"

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

    FilterNode* filter;

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
            grp->setSelectedTag(this);
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

        // force redrawing of this listviewitem
        this->repaint();

        m_photobook->autoRefreshView();
        break;
    }
}


void TagTreeNodeRadio::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {

    TagNodeRadio* tagNode = dynamic_cast<TagNodeRadio*>(m_tagNode);

    switch (column) {
    case TagTree::COLUMN_TEXT :
        KListViewItem::paintCell(p, cg, column, width, alignment);
        break;

    case TagTree::COLUMN_VALUE : {
        // paint the cell with the alternating background color
        p->fillRect(0, 0, width, this->height(), backgroundColor(0));

        // draw the checkbox in the center of the cell in the size of the font
        int size = p->fontInfo().pixelSize()+2;
        QRect rect((width - size + 4)/2, (  this->height()-size )/2, size, size);

        // get all selected files
        const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

        if (selectedFiles->count()) {
            int taggedFilesCount = 0;
            int untaggedFilesCount = 0;

            // loop over all selected files and determine their state
            QPtrListIterator<KFileItem> it(*m_photobook->view()->fileView()->selectedItems());
            for (; it.current(); ++it) {
                File* selectedFile = dynamic_cast<File*>(it.current());

                if (tagNode->tagged(selectedFile)) {
                    taggedFilesCount++;
                } else {
                    untaggedFilesCount++;
                }

                // we can abort the loop if we found a tagged and utagged file
                if (taggedFilesCount && untaggedFilesCount) {
                    break;
                }
            }

            // we assume that some files are tagged, some not
            int tristate = 0;
            // no file is tagged
            if (!taggedFilesCount && untaggedFilesCount) {
                tristate = -1;
            } else
            // all files are tagged
            if (taggedFilesCount && !untaggedFilesCount) {
                tristate = 1;
            }

            TreeHelper::drawRadioButton(p, cg, rect, tristate, !Settings::tagTreeLocked());
        } else {
            // no file is selected -> draw a disabled checkbox
            TreeHelper::drawRadioButton(p, cg, rect, false, false);
        }
        break;
    }
    case TagTree::COLUMN_FILTER :
        // paint the cell with the alternating background color
        p->fillRect(0, 0, width, this->height(), backgroundColor(0));

        // draw the checkbox in the center of the cell in the size of the font
        int size = p->fontInfo().pixelSize()+2;
        QRect rect((width - size + 4)/2, (  this->height()-size )/2, size, size);

        TreeHelper::drawCheckBox(p, cg, rect, m_filterState, true);

        break;
    }
}
