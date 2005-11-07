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

#include "tagtree.h"
#include "treehelper.h"
#include "../engine/file.h"
#include "../engine/filetagnodeassocstring.h"
#include "../engine/filternodetagstring.h"
#include "../engine/tagnodestring.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"
#include "../settings/settings.h"
#include "../tracer/tracer.h"

#include <kfileiconview.h>
#include <kfileitem.h>


Tracer* TagTreeNodeString::tracer = Tracer::getInstance("kde.kphotobook.uitrees", "TagTreeNodeSTring");


TagTreeNodeString::TagTreeNodeString(TagTree* parent, TagNodeString* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu),
    m_filterValue(QString::null)
{
    // enable editing of value and filter
    if (!tagNode->readonly()) {
        setRenameEnabled(TagTree::COLUMN_VALUE, true);
    }
    setRenameEnabled(TagTree::COLUMN_FILTER, true);
}


TagTreeNodeString::TagTreeNodeString(TagTreeNode* parent, TagNodeString* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu),
    m_filterValue(QString::null)
{
    // enable editing of value and filter
    if (!tagNode->readonly()) {
        setRenameEnabled(TagTree::COLUMN_VALUE, true);
    }
    setRenameEnabled(TagTree::COLUMN_FILTER, true);
}


TagTreeNodeString::~TagTreeNodeString()
{
}


FilterNode* TagTreeNodeString::filter() const
{
    if (m_filterValue.isEmpty()) {
        return 0;
    }

    TagNodeString* tagNode = dynamic_cast<TagNodeString*>(m_tagNode);
    return new FilterNodeTagString(tagNode, m_filterValue);
}


void TagTreeNodeString::deselectFilter()
{
    m_filterValue = QString("()");
    setText(TagTree::COLUMN_FILTER, m_filterValue);

    // force redrawing of this listviewitem
    this->repaint();
}


void TagTreeNodeString::resetFilter()
{
    m_filterValue = QString::null;
    setText(TagTree::COLUMN_FILTER, m_filterValue);

    // force redrawing of this listviewitem
    this->repaint();
}


void TagTreeNodeString::applyFilterString(QString filter)
{
    m_filterValue = filter;
    setText(TagTree::COLUMN_FILTER, filter);
}


void TagTreeNodeString::leftClicked(TagTree* /* tagTree */, int column)
{
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

            // get all selected files
            const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

            if (selectedFiles->count()) {
                // let the user enter a new value
                startRename(TagTree::COLUMN_VALUE);
            }
            break;
        }
        case TagTree::COLUMN_FILTER: {
            startRename(TagTree::COLUMN_FILTER);
            break;
        }
    }
}


///@todo this derived method may be deleted by time, as it does nothing special
void TagTreeNodeString::rightClicked(TagTree* tagTree, int column)
{
    TagTreeNode::rightClicked(tagTree, column);
}


void TagTreeNodeString::handleRenaming(int column, const QString& text)
{
    TagNodeString* tagNode = dynamic_cast<TagNodeString*>(m_tagNode);

    switch (column) {
        case TagTree::COLUMN_TEXT:
            break;        case TagTree::COLUMN_VALUE: {
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
        case TagTree::COLUMN_FILTER: {
            // filter has changed --> update the text in the node and auto refresh view
            setFilterValue(text);

            m_photobook->autoRefreshView();
            break;
        }
    }
}


void TagTreeNodeString::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    TagNodeString* tagNode = dynamic_cast<TagNodeString*>(m_tagNode);

    switch (column) {
        case TagTree::COLUMN_TEXT: {
            TagTreeNode::paintCell(p, cg, column, width, alignment);
            break;
        }
        case TagTree::COLUMN_VALUE: {
            bool mixed = false;
            QString text = "";
            if (m_tagCurrentMatch == TagTreeNode::MIXTAGGED) {
                mixed = true;
            } else {

                ///@todo what about MIXTAGGED??? and call getCommonValue

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
                                // abort if not all selected images have the same tagvalue!
                                mixed = true;
                                break;
                            }
                            text = assoc->valueAsString();
                        } else {
                            // no assoc found -> remove the text
                            if (!text.isEmpty()) {
                                // abort if not all selected images have the same tagvalue!
                                mixed = true;
                                break;
                            }
                            text = "";
                        }
                    }
                }
            }

            // if mixed is true, we display the third state of the checkbox
            if (mixed) {
                TreeHelper::drawCheckBox(p, cg, backgroundColor(TagTree::COLUMN_VALUE), width, this->height(), true, TreeHelper::UNDEFINED);
            } else {
                setText(TagTree::COLUMN_VALUE, text);

                KListViewItem::paintCell(p, cg, column, width, Qt::AlignLeft);
            }
            break;
        }
        case TagTree::COLUMN_FILTER: {
            // if filtering an empty string (the filter is ignored), we display the third state of the checkbox
            if (m_filterValue.isEmpty()) {
                TreeHelper::drawCheckBox(p, cg, backgroundColor(TagTree::COLUMN_FILTER), width, this->height(), true, TreeHelper::UNDEFINED);
            } else if (m_filterValue == "()") {
                TreeHelper::drawCheckBox(p, cg, backgroundColor(TagTree::COLUMN_FILTER), width, this->height(), true, TreeHelper::NOT_CHECKED);
            } else {
                TagTreeNode::paintCell(p, cg, column, width, Qt::AlignLeft);
            }
            break;
        }
    }
}


QString* TagTreeNodeString::getCommonValue(const KFileItemList* selectedFiles)
{
    tracer->invoked(__func__);

    TagNodeString* tagNode = dynamic_cast<TagNodeString*>(m_tagNode);

    QString* commonValue = 0;

    QPtrListIterator<KFileItem> it(*selectedFiles);
    for (; it.current(); ++it) {
        File* selectedFile = dynamic_cast<File*>(it.current());

        FileTagNodeAssocString* assoc = dynamic_cast<FileTagNodeAssocString*>(tagNode->getAssocToFile(selectedFile));

        if (assoc && !assoc->value().isEmpty()) {
            // file with a valid assoc found

            // the value of the first assoc we just remember and continue
            if (!commonValue) {
                commonValue = new QString(assoc->value());
                continue;
            }

            // ok we got an assoc remember it
            if (!commonValue->isEmpty() && assoc->value() != *commonValue) {
                // abort if not all selected images have the same tagvalue!
                delete commonValue;
                commonValue = new QString(QString::null);
                break;
            }
        } else {
            // file with no assoc found
            if (commonValue) {
                // abort if not all selected images have the same tagvalue!
                delete commonValue;
                commonValue = new QString(QString::null);
                break;
            }
        }
    }

    tracer->sdebug(__func__) << "commnon value is: " << (commonValue ? *commonValue : "0") << endl;
    return commonValue;
}
