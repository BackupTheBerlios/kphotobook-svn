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

#include "settings.h"
#include "tagnodeboolean.h"
#include "tagtree.h"
#include "kphotobook.h"
#include "kphotobookview.h"
#include "file.h"
#include "treehelper.h"

#include <kfileitem.h>

TagTreeNodeBoolean::TagTreeNodeBoolean(TagTree* parent, TagNodeBoolean* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu)
    , m_filterState(FILTERSTATE_IGNORE) {
}


TagTreeNodeBoolean::TagTreeNodeBoolean(TagTreeNode* parent, TagNodeBoolean* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu)
    , m_filterState(FILTERSTATE_IGNORE) {
}


TagTreeNodeBoolean::~TagTreeNodeBoolean() {
}


QString TagTreeNodeBoolean::filter() {

    QString filter;

    switch (m_filterState) {
    case FILTERSTATE_EXCLUDE:
        filter = QString("!%1").arg(m_tagNode->id());
        break;
    case FILTERSTATE_IGNORE:
        filter = QString::null;
        break;
    case FILTERSTATE_INCLUDE:
        filter = QString::number(m_tagNode->id());
        break;
    }

    return filter;
}


void TagTreeNodeBoolean::leftClicked(__attribute__((unused)) TagTree* tagTree, int column) {

    TagNodeBoolean* tagNode = dynamic_cast<TagNodeBoolean*>(m_tagNode);

    switch (column) {
    case TagTree::COLUMN_TEXT :
        break;

    case TagTree::COLUMN_VALUE : {
    
        // so nothing when tagging is locked
        if (Settings::tagTreeLocked()) {
            return;
        }

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

            // if not all files are tagged, we tag all files
            bool tagged = true;
            // all files are tagged
            if (taggedFilesCount && !untaggedFilesCount) {
                tagged = false;
            }

            // loop over all selected files and change their state
            it.toFirst();
            for (; it.current(); ++it) {
                File* selectedFile = dynamic_cast<File*>(it.current());

                tagNode->setTagged(selectedFile, tagged);
            }

            m_photobook->dirtyfy();

            // force redrawing of this listviewitem
            this->repaint();
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


void TagTreeNodeBoolean::rightClicked(__attribute__((unused)) TagTree* tagTree, int column) {

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


void TagTreeNodeBoolean::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {

    TagNodeBoolean* tagNode = dynamic_cast<TagNodeBoolean*>(m_tagNode);

    switch (column) {
    case TagTree::COLUMN_TEXT :
        KListViewItem::paintCell(p, cg, column, width, alignment);
        break;

    case TagTree::COLUMN_VALUE : {
        // paint the cell with the alternating background color
        p->fillRect(0, 0, width, this->height(), backgroundColor());

        // draw the checkbox in the center of the cell
        QRect rect((width-this->height()+4)/2, 2, this->height()-4, this->height()-4);

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

            TreeHelper::drawCheckBox(p, cg, rect, tristate, !Settings::tagTreeLocked());
        } else {
            // no file is selected -> draw a disabled checkbox
            TreeHelper::drawCheckBox(p, cg, rect, false, false);
        }
        break;
    }
    case TagTree::COLUMN_FILTER :
        // paint the cell with the alternating background color
        p->fillRect(0, 0, width, this->height(), backgroundColor());

        // draw the checkbox in the center of the cell
        QRect rect((width-this->height()+4)/2, 2, this->height()-4, this->height()-4);

        TreeHelper::drawCheckBox(p, cg, rect, m_filterState, true);

        break;
    }
}
