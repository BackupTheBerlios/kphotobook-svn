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

#include "tagtreenodedatetime.h"

#include "../engine/tagnodedatetime.h"
#include "../engine/filternodetagdatetime.h"
#include "../engine/filetagnodeassocdatetime.h"
#include "tagtree.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"
#include "../engine/file.h"
#include "treehelper.h"

#include <kdatepicker.h>
#include <kdatetimewidget.h>
#include <kfileitem.h>

TagTreeNodeDateTime::TagTreeNodeDateTime(TagTree* parent, TagNodeDateTime* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu)
    , m_filterValue(QString::null)
{
    // enable editing of value and filter
    setRenameEnabled(TagTree::COLUMN_VALUE, true);
    setRenameEnabled(TagTree::COLUMN_FILTER, true);
}


TagTreeNodeDateTime::TagTreeNodeDateTime(TagTreeNode* parent, TagNodeDateTime* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu)
    , m_filterValue(QString::null)
{
    // enable editing of value and filter
    setRenameEnabled(TagTree::COLUMN_VALUE, true);
    setRenameEnabled(TagTree::COLUMN_FILTER, true);
}


TagTreeNodeDateTime::~TagTreeNodeDateTime()
{
}


FilterNode* TagTreeNodeDateTime::filter()
{
    if (m_filterValue.isEmpty()) {
        return 0;
    }

    TagNodeDateTime* tagNode = dynamic_cast<TagNodeDateTime*>(m_tagNode);
    return new FilterNodeTagDateTime(tagNode, m_filterValue);
}


void TagTreeNodeDateTime::deselectFilter()
{
    m_filterValue = QString("()");
    setText(TagTree::COLUMN_FILTER, m_filterValue);

    // force redrawing of this listviewitem
    this->repaint();
}


void TagTreeNodeDateTime::resetFilter()
{
    m_filterValue = QString::null;
    setText(TagTree::COLUMN_FILTER, m_filterValue);

    // force redrawing of this listviewitem
    this->repaint();
}


QString TagTreeNodeDateTime::getFilterString()
{
    return m_filterValue;
}


void TagTreeNodeDateTime::applyFilterString(QString filter)
{
    m_filterValue = filter;
    setText(TagTree::COLUMN_FILTER, filter);
}


void TagTreeNodeDateTime::leftClicked(__attribute__((unused)) TagTree* tagTree, int column)
{
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
//            KDateTimeWidget* x = new KDateTimeWidget(m_photobook->view(), "KDateTimeWidget");
//            KDatePicker* x = new KDatePicker(m_photobook->view(), "KDateTimeWidget");
//            x->show();

            startRename(TagTree::COLUMN_VALUE);
        }
        break;
    }

    case TagTree::COLUMN_FILTER :
        startRename(TagTree::COLUMN_FILTER);
        break;
    }
}


void TagTreeNodeDateTime::rightClicked(__attribute__((unused)) TagTree* tagTree, int column)
{
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


void TagTreeNodeDateTime::handleRenaming(int column, const QString& text)
{
    QDateTime value = QDateTime::fromString(text, Qt::ISODate);
    if (!value.isValid()) {

        // TODO: do something useful!!!
        return;
    }

    TagNodeDateTime* tagNode = dynamic_cast<TagNodeDateTime*>(m_tagNode);

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

                if (tagNode->tagged(selectedFile, text)) {
                    taggedFilesCount++;
                } else {
                    untaggedFilesCount++;
                }

                // we can abort the loop if we found a tagged and untagged file
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

                tagNode->setTagged(selectedFile, value);
            }

            m_photobook->dirtyfy();

            // force redrawing of this listviewitem
            this->repaint();
        }
        break;
    }
    case TagTree::COLUMN_FILTER :
        // filter has changed --> update the text in the node and auto refresh view
        setFilterValue(text);

        m_photobook->autoRefreshView();
        break;
    }
}


void TagTreeNodeDateTime::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    TagNodeDateTime* tagNode = dynamic_cast<TagNodeDateTime*>(m_tagNode);

    switch (column) {
    case TagTree::COLUMN_TEXT :
    case TagTree::COLUMN_FILTER :

        TagTreeNode::paintCell(p, cg, column, width, alignment);
        break;

    case TagTree::COLUMN_VALUE : {

        // get all selected files
        const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

        if (selectedFiles->count()) {

            QString text = QString::null;

            QPtrListIterator<KFileItem> it(*m_photobook->view()->fileView()->selectedItems());
            for (; it.current(); ++it) {
                File* selectedFile = dynamic_cast<File*>(it.current());

                FileTagNodeAssocDateTime* assoc = dynamic_cast<FileTagNodeAssocDateTime*>(tagNode->getAssocToFile(selectedFile));
                if (assoc) {
                    // ok we got an assoc remember it

                    if (!text.isNull() && assoc->valueAsString() != text) {
                        // abort if not all selected images have the same tagvalue!
                        text = "---";
                        break;
                    }
                    text = assoc->valueAsString();
                } else {
                    // no assoc found -> remove the text

                    if (!text.isEmpty()) {
                        // abort if not all selected images have the same tagvalue!
                        text = "---";
                        break;
                    }
                    text = "";
                }
            }

            setText(TagTree::COLUMN_VALUE, text);
        } else {
            // no file is selected -> remove the text
            setText(TagTree::COLUMN_VALUE, "");
        }

        KListViewItem::paintCell(p, cg, column, width, alignment);
        break;
    }
    }
}
