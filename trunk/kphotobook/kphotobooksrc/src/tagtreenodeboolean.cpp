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

#include "tagnodeboolean.h"
#include "tagtree.h"
#include "kphotobook.h"
#include "kphotobookview.h"
#include "file.h"

#include <kfileitem.h>

TagTreeNodeBoolean::TagTreeNodeBoolean(KListView* parent, TagNodeBoolean* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, *tagNode->text(), photobook, tagNode->icon(), contextMenu), m_tagNode(tagNode) {
}


TagTreeNodeBoolean::TagTreeNodeBoolean(KListViewItem* parent, TagNodeBoolean* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, *tagNode->text(), photobook, tagNode->icon(), contextMenu), m_tagNode(tagNode) {
}


TagTreeNodeBoolean::~TagTreeNodeBoolean() {
}


TagNode* TagTreeNodeBoolean::tagNode() {
    return m_tagNode;
}


void TagTreeNodeBoolean::columnClicked(__attribute__((unused)) TagTree* tagTree, int column) {

    switch (column) {
    case TagTree::COLUMN_TEXT :
        break;

    case TagTree::COLUMN_VALUE : {

        // get all selected files
        const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

        if (selectedFiles->count()) {
            int taggedFilesCount = 0;
            int untaggedFilesCount = 0;

            // loop over all selected files and determine their state
            QPtrListIterator<KFileItem> it(*m_photobook->view()->fileView()->selectedItems());
            for (; it.current(); ++it) {
                File* selectedFile = dynamic_cast<File*>(it.current());

                if (m_tagNode->tagged(selectedFile)) {
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

                m_tagNode->setTagged(selectedFile, tagged);
            }

            m_photobook->dirtyfy();

            // force redrawing of this listviewitem
            this->repaint();
        }
        break;
    }
    case TagTree::COLUMN_FILTER :
        m_tagNode->setFiltered(!m_tagNode->filtered());
        // force redrawing of this listviewitem
        this->repaint();
        break;
    }
}

void TagTreeNodeBoolean::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {

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

                if (m_tagNode->tagged(selectedFile)) {
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

            drawCheckBox(p, cg, rect, tristate);
        } else {
            // no file is selected -> dtaw a disabled checkbox
            drawCheckBox(p, cg, rect);
        }

        break;
    }
    case TagTree::COLUMN_FILTER :
        // paint the cell with the alternating background color
        p->fillRect(0, 0, width, this->height(), backgroundColor());

        // draw the checkbox in the center of the cell
        QRect rect((width-this->height()+4)/2, 2, this->height()-4, this->height()-4);

        drawCheckBox(p, cg, rect, m_tagNode->filtered());

        break;
    }
}
