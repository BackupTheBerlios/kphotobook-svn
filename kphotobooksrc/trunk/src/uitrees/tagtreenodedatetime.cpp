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

#include "tagtree.h"
#include "treehelper.h"
#include "../dialogs/dialogdatetimechooser.h"
#include "../dialogs/dialogdatetimefilter.h"
#include "../engine/file.h"
#include "../engine/filetagnodeassocdatetime.h"
#include "../engine/filternodetagdatetime.h"
#include "../engine/tagnodedatetime.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"
#include "../settings/settings.h"

#include <kdatepicker.h>
#include <kdatetimewidget.h>
#include <kfileiconview.h>
#include <kglobal.h>
#include <klocale.h>


Tracer* TagTreeNodeDateTime::tracer = Tracer::getInstance("kde.kphotobook.uitrees", "TagTreeNodeDateTime");


TagTreeNodeDateTime::TagTreeNodeDateTime(TagTree* parent, TagNodeDateTime* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu),
    m_data(new DateTimeFilterData(""))
{
}


TagTreeNodeDateTime::TagTreeNodeDateTime(TagTreeNode* parent, TagNodeDateTime* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu),
    m_data(new DateTimeFilterData(""))
{
}


TagTreeNodeDateTime::~TagTreeNodeDateTime()
{
    delete m_data;
}


FilterNode* TagTreeNodeDateTime::filter()
{
    if (m_data->getState() == DateTimeFilterData::INVALID ||
        m_data->getState() == DateTimeFilterData::NO_FILTER_SET) {

        return 0;
    }

    return new FilterNodeTagDateTime(static_cast<TagNodeDateTime*>(m_tagNode), m_data);
}


void TagTreeNodeDateTime::deselectFilter()
{
    m_data->setFilterNoDateSet();
    setText(TagTree::COLUMN_FILTER, m_data->toString());

    // force redrawing of this listviewitem
    this->repaint();
}


void TagTreeNodeDateTime::resetFilter()
{
    m_data->setFilterNoFilterSet();
    setText(TagTree::COLUMN_FILTER, m_data->toString());

    // force redrawing of this listviewitem
    this->repaint();
}


QString TagTreeNodeDateTime::filterString()
{
    tracer->invoked(__func__);

    return m_data->marshal();
}


void TagTreeNodeDateTime::applyFilterString(QString filter)
{
    tracer->sinvoked(__func__) << "filter: " << filter << endl;

    m_data->setFilterString(filter);
    setText(TagTree::COLUMN_FILTER, m_data->toString());
}


void TagTreeNodeDateTime::leftClicked(__attribute__((unused)) TagTree* tagTree, int column)
{
    tracer->invoked(__func__);

    TagNodeDateTime* tagNode = dynamic_cast<TagNodeDateTime*>(m_tagNode);

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

                QDateTime* commonValue = getCommonValue(selectedFiles);

                // let the user enter a new value
                DialogDateTimeChooser* dateTimeChooser = new DialogDateTimeChooser(0, "DialogDateTimeChooser", commonValue ? *commonValue : QDateTime());
                if (dateTimeChooser->exec() == QDialog::QDialog::Accepted) {

                    bool noDateTimeSet = dateTimeChooser->noDateTimeSet();
                    QDateTime value = dateTimeChooser->dateTime();
                    if (noDateTimeSet || value.isValid()) {

                        // get all selected files
                        const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

                        if (selectedFiles->count()) {

                            // loop over all selected files and change their state
                            QPtrListIterator<KFileItem> it(*m_photobook->view()->fileView()->selectedItems());
                            it.toFirst();
                            for (; it.current(); ++it) {
                                File* selectedFile = dynamic_cast<File*>(it.current());

                                if (noDateTimeSet) {
                                    // remove the assoc to this file
                                    FileTagNodeAssoc* assoc = tagNode->getAssocToFile(selectedFile);
                                    if (assoc) {
                                        tagNode->removeAssoc(assoc);
                                    }
                                } else {
                                    // set the (new) value
                                    tagNode->setTagged(selectedFile, value);
                                }
                            }

                            m_photobook->dirtyfy();

                            // force redrawing of this listviewitem
                            this->repaint();
                        }
                    }
                }
                delete dateTimeChooser;
                delete commonValue;
            }
            break;
        }
        case TagTree::COLUMN_FILTER: {
            // let the user enter a new value
            DialogDateTimeFilter* dateTimeFilter = new DialogDateTimeFilter(0, "DialogDateTimeFilter", m_data);
            //createDialogDateTimeFilter(text(TagTree::COLUMN_FILTER));
            if (dateTimeFilter->exec() == QDialog::Accepted) {
                setText(TagTree::COLUMN_FILTER, m_data->toString());
                // force redrawing of this listviewitem
                this->repaint();

                m_photobook->autoRefreshView();
            }
            break;
        }
    }
}


///@todo This derived method may be deleted by time, but as we are in the middle of the tagTreeNode
/// redesign process, I rather don't delete it yet.
void TagTreeNodeDateTime::rightClicked(TagTree* tagTree, int column)
{
    TagTreeNode::rightClicked(tagTree, column);
}


void TagTreeNodeDateTime::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    switch (column) {
        case TagTree::COLUMN_TEXT: {
            TagTreeNode::paintCell(p, cg, column, width, alignment);
            break;
        }
        case TagTree::COLUMN_VALUE: {
            // get all selected files
            const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

            if (selectedFiles->count()) {

                QDateTime* commonValue = getCommonValue(selectedFiles);

                if (commonValue) {

                    if (commonValue->isValid()) {
                        setText(TagTree::COLUMN_VALUE, TreeHelper::formatDateTime(*commonValue));
                        KListViewItem::paintCell(p, cg, column, width, Qt::AlignLeft);
                    } else {
                        // the selected files have different values --> show the third state of a checkbox
                        TreeHelper::drawCheckBox(p, cg, backgroundColor(TagTree::COLUMN_VALUE), width, this->height(), true, TreeHelper::UNDEFINED);
                    }
                } else {
                    // none of the selected file has a value
                    setText(TagTree::COLUMN_VALUE, "");
                    KListViewItem::paintCell(p, cg, column, width, Qt::AlignCenter);
                }
                delete commonValue;
            } else {
                // no file is selected
                setText(TagTree::COLUMN_VALUE, "");
                KListViewItem::paintCell(p, cg, column, width, Qt::AlignCenter);
            }

            break;
        }
        case TagTree::COLUMN_FILTER: {
            // if filtering an empty string (the filter is ignored), we display the third state of the checkbox
            switch(m_data->getState()) {
                case DateTimeFilterData::NO_FILTER_SET: {
                    TreeHelper::drawCheckBox(p, cg, backgroundColor(TagTree::COLUMN_FILTER), width, this->height(), true, TreeHelper::UNDEFINED);
                    break;
                }
                case DateTimeFilterData::NO_DATE_SET: {
                    TreeHelper::drawCheckBox(p, cg, backgroundColor(TagTree::COLUMN_FILTER), width, this->height(), true, TreeHelper::NOT_CHECKED);
                    break;
                }
                default: {
                    setText(TagTree::COLUMN_FILTER, m_data->toString());
                    TagTreeNode::paintCell(p, cg, column, width, Qt::AlignLeft);
                    break;
                }
            }
            break;
        }
    }
}


QDateTime* TagTreeNodeDateTime::getCommonValue(const KFileItemList* selectedFiles)
{
    tracer->invoked(__func__);

    TagNodeDateTime* tagNode = dynamic_cast<TagNodeDateTime*>(m_tagNode);

    QDateTime* commonValue = 0;

    QPtrListIterator<KFileItem> it(*selectedFiles);
    for (; it.current(); ++it) {
        File* selectedFile = dynamic_cast<File*>(it.current());

        FileTagNodeAssocDateTime* assoc = dynamic_cast<FileTagNodeAssocDateTime*>(tagNode->getAssocToFile(selectedFile));

        if (assoc && assoc->value().isValid()) {
            // file with a valid assoc found

            // the value of the first assoc we just remember and continue
            if (!commonValue) {
                commonValue = new QDateTime(assoc->value());
                continue;
            }

            // ok we got an assoc remember it
            if (commonValue->isValid() && assoc->value() != *commonValue) {
                // abort if not all selected images have the same tagvalue!
                delete commonValue;
                commonValue = new QDateTime();
                break;
            }
        } else {
            // file with no assoc found
            if (commonValue) {
                // abort if not all selected images have the same tagvalue!
                delete commonValue;
                commonValue = new QDateTime();
                break;
            }
        }
    }

    tracer->sdebug(__func__) << "common value is: " << (commonValue ? commonValue->toString() : "0") << endl;
    return commonValue;
}
