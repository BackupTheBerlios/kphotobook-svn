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

#include <kdatepicker.h>
#include <kdatetimewidget.h>
#include <kglobal.h>


Tracer* TagTreeNodeDateTime::tracer = Tracer::getInstance("kde.kphotobook.uitrees", "TagTreeNodeDateTime");


TagTreeNodeDateTime::TagTreeNodeDateTime(TagTree* parent, TagNodeDateTime* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu),
    m_locale(KGlobal::locale()),
    m_filterValue(QString::null),
    m_filterNode(0)
{
}


TagTreeNodeDateTime::TagTreeNodeDateTime(TagTreeNode* parent, TagNodeDateTime* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu),
    m_locale(KGlobal::locale()),
    m_filterValue(QString::null),
    m_filterNode(0)
{
}


TagTreeNodeDateTime::~TagTreeNodeDateTime()
{
    delete m_filterNode;
}


FilterNode* TagTreeNodeDateTime::filter()
{
    ///@todo determine the filternode from the stored text. current implementation cannot restore the datetime filter after restart...
    return m_filterNode;
}


void TagTreeNodeDateTime::deselectFilter()
{
    m_filterValue = QString("()");
    setText(TagTree::COLUMN_FILTER, m_filterValue);
    delete m_filterNode;
    m_filterNode = new FilterNodeTagDateTime(dynamic_cast<TagNodeDateTime*>(m_tagNode), m_filterValue);

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
    tracer->invoked(__func__);
    return m_filterValue;
}


void TagTreeNodeDateTime::applyFilterString(QString filter)
{
    tracer->sinvoked(__func__) << "filter: " << filter << endl;
    m_filterValue = filter;
    setText(TagTree::COLUMN_FILTER, filter);
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
            DialogDateTimeFilter* dateTimeFilter = createDialogDateTimeFilter(text(TagTree::COLUMN_FILTER));
            if (dateTimeFilter->exec() == QDialog::Accepted) {
                applyFilter(dateTimeFilter);
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
                        setText(TagTree::COLUMN_VALUE, formatDateTime(*commonValue));
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

    tracer->sdebug(__func__) << "common value is: " << (commonValue ? m_locale->formatDateTime(*commonValue, true, true) : "0") << endl;
    return commonValue;
}


QString TagTreeNodeDateTime::formatDateTime(const QDateTime& dateTime)
{
    if (dateTime.time().isValid()) {
        return m_locale->formatDateTime(dateTime, true, true);
    } else {
        return m_locale->formatDate(dateTime.date(), true);
    }
}


QDateTime* TagTreeNodeDateTime::readDateTime(const QString& dateTimeStr)
{
    tracer->sdebug(__func__) << "dateTime to convert: " << dateTimeStr << endl;

    int pos = dateTimeStr.findRev(" ");
    QString dateStr = dateTimeStr.mid(0, pos);
    QString timeStr = dateTimeStr.mid(pos+1);

    tracer->sdebug(__func__) << "date to convert: " << dateStr << endl;
    tracer->sdebug(__func__) << "time to convert: " << timeStr << endl;

    bool* ok;
    QDate date = m_locale->readDate(dateStr, ok);
    if (!*ok) {
        tracer->serror(__func__) << "Date part of datetime (" << dateTimeStr << ") is invalid: " << dateStr << "! Using current date." << endl;
        date = QDate::currentDate();
    }

    QTime time = m_locale->readTime(timeStr, ok);
    if (!*ok) {
        tracer->serror(__func__) << "Time part of datetime (" << dateTimeStr << ") is invalid: " << timeStr << "! Not setting time time." << endl;
        return new QDateTime(date);
    }

    return new QDateTime(date, time);
}


void TagTreeNodeDateTime::applyFilter(DialogDateTimeFilter* dateTimeFilter)
{
    TagNodeDateTime* tagNode = dynamic_cast<TagNodeDateTime*>(m_tagNode);

    switch (dateTimeFilter->getState()) {
        case DialogDateTimeFilter::INVALID:
        case DialogDateTimeFilter::NO_FILTER_SET: {
            m_filterValue = QString::null;
            delete m_filterNode;
            m_filterNode = 0;

            break;
        }
        case DialogDateTimeFilter::FROM_DATE_SET: {
            m_filterValue = QString(">= %1").arg(formatDateTime(dateTimeFilter->getDateTimeFrom()));
            delete m_filterNode;
            m_filterNode = new FilterNodeTagDateTime(tagNode, new QDateTime(dateTimeFilter->getDateTimeFrom()), 0);

            break;
        }
        case DialogDateTimeFilter::TO_DATE_SET: {
            m_filterValue = QString("<= %1").arg(formatDateTime(dateTimeFilter->getDateTimeTo()));
            delete m_filterNode;
            m_filterNode = new FilterNodeTagDateTime(tagNode, 0, new QDateTime(dateTimeFilter->getDateTimeTo()));

            break;
        }
        case DialogDateTimeFilter::FROM_TO_DATE_SET: {
            m_filterValue = QString("%1 - %2").arg(formatDateTime(dateTimeFilter->getDateTimeFrom())).arg(formatDateTime(dateTimeFilter->getDateTimeTo()));
            delete m_filterNode;
            m_filterNode = new FilterNodeTagDateTime(tagNode, new QDateTime(dateTimeFilter->getDateTimeFrom()), new QDateTime(dateTimeFilter->getDateTimeTo()));

            break;
        }
        case DialogDateTimeFilter::PATTERN_DATE_SET: {
            m_filterValue = QString(dateTimeFilter->getPattern());
            delete m_filterNode;
            m_filterNode = new FilterNodeTagDateTime(tagNode, m_filterValue);

            break;
        }
        case DialogDateTimeFilter::SINGLE_DATE_SET: {
            m_filterValue = QString("= %1").arg(formatDateTime(dateTimeFilter->getDateTimeFrom()));
            delete m_filterNode;
            m_filterNode = new FilterNodeTagDateTime(tagNode, new QDateTime(dateTimeFilter->getDateTimeFrom()), new QDateTime(dateTimeFilter->getDateTimeTo()));

            break;
        }
        case DialogDateTimeFilter::NO_DATE_SET: {
            m_filterValue = QString("()");
            delete m_filterNode;
            m_filterNode = new FilterNodeTagDateTime(tagNode, m_filterValue);

            break;
        }
    }
    setText(TagTree::COLUMN_FILTER, m_filterValue);

    // force redrawing of this listviewitem
    this->repaint();
}


DialogDateTimeFilter* TagTreeNodeDateTime::createDialogDateTimeFilter(QString filter)
{
    filter = filter.stripWhiteSpace();
    if (filter.isEmpty()) {
        // no filter is set
        return new DialogDateTimeFilter(0, "DialogDateTimeFilter");
    }
    if (filter.startsWith(">= ")) {
        // from date is set
        QDateTime* fromDateTime = readDateTime(filter.mid(2).stripWhiteSpace());
        return new DialogDateTimeFilter(0, "DialogDateTimeFilter", fromDateTime, 0);
        delete fromDateTime;
    }
    if (filter.startsWith("<= ")) {
        // to date is set
        QDateTime* toDateTime = readDateTime(filter.mid(2).stripWhiteSpace());
        return new DialogDateTimeFilter(0, "DialogDateTimeFilter", 0, toDateTime);
        delete toDateTime;
    } if (filter.contains(" - ")) {
        // from and to date are set

        int pos = filter.find(" - ");
        QDateTime* fromDateTime = readDateTime(filter.mid(0, pos).stripWhiteSpace());
        QDateTime* toDateTime = readDateTime(filter.mid(pos+3).stripWhiteSpace());
        return new DialogDateTimeFilter(0, "DialogDateTimeFilter", fromDateTime, toDateTime);
        delete fromDateTime;
        delete toDateTime;
    }
    if (filter.startsWith("= ")) {
        // singledate is set
        QDateTime* singleDateTime = readDateTime(filter.mid(2).stripWhiteSpace());
        return new DialogDateTimeFilter(0, "DialogDateTimeFilter", singleDateTime);
        delete singleDateTime;
    }
    if (filter == "()") {
        // 'no date' filter is set
        return new DialogDateTimeFilter(0, "DialogDateTimeFilter", true);
    }

    // 'regexp' filter is set
    return new DialogDateTimeFilter(0, "DialogDateTimeFilter", filter);
}

