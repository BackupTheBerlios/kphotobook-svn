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

#include "../settings/settings.h"
#include "../engine/tagnodestring.h"
#include "tagtree.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"
#include "../engine/file.h"
#include "treehelper.h"

#include <kfileitem.h>

TagTreeNodeString::TagTreeNodeString(TagTree* parent, TagNodeString* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu)
    , m_filterValue(QString::null) {

    setRenameEnabled(TagTree::COLUMN_VALUE, true);
    setRenameEnabled(TagTree::COLUMN_FILTER, true);
}


TagTreeNodeString::TagTreeNodeString(TagTreeNode* parent, TagNodeString* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu)
    , m_filterValue(QString::null) {

    setRenameEnabled(TagTree::COLUMN_VALUE, true);
    setRenameEnabled(TagTree::COLUMN_FILTER, true);
}


TagTreeNodeString::~TagTreeNodeString() {
}


QString TagTreeNodeString::filter() {

    if (m_filterValue.isEmpty()) {
        return QString::null;
    }

    return QString("%1==%2").arg(m_tagNode->id()).arg(m_filterValue);
}

    
QString TagTreeNodeString::getFilterString() {

    return m_filterValue;
}
    
    
void TagTreeNodeString::applyFilterString(QString filter) {

    m_filterValue = filter;
}


void TagTreeNodeString::leftClicked(__attribute__((unused)) TagTree* tagTree, int column) {

//    TagNodeString* tagNode = dynamic_cast<TagNodeString*>(m_tagNode);

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


void TagTreeNodeString::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {

    KListViewItem::paintCell(p, cg, column, width, alignment);
/*
    TagNodeBoolean* tagNode = dynamic_cast<TagNodeBoolean*>(m_tagNode);

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

            TreeHelper::drawCheckBox(p, cg, rect, tristate, !Settings::tagTreeLocked());
        } else {
            // no file is selected -> draw a disabled checkbox
            TreeHelper::drawCheckBox(p, cg, rect, false, false);
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
*/
}
