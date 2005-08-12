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

#ifndef TAGTREENODEDATETIME_H
#define TAGTREENODEDATETIME_H

#include "tagtreenode.h"
#include "../utils/datetimefilterdata.h"

#include <kfileitem.h>

class DialogDateTimeFilter;
class TagNodeDateTime;


/**
 * Concrete tagtreenode for displaying a datetime tagnode.
 *
 * CVS-ID $Id: tagtreenodeboolean.h 274 2005-03-25 08:52:15Z choenig $
 */
class TagTreeNodeDateTime : public TagTreeNode
{
    private:
        static Tracer* tracer;

    public:
        TagTreeNodeDateTime(TagTree* parent, TagNodeDateTime* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);
        TagTreeNodeDateTime(TagTreeNode* parent, TagNodeDateTime* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

        virtual ~TagTreeNodeDateTime();

        void setFilterValue(QString filterValue)
        {
            delete m_data;
            m_data = new DateTimeFilterData(filterValue);
        }

        /**
        * Returns an FilterNode used to describe this filter.
        */
        virtual FilterNode* filter();

        /**
        * Sets the filter to find images without this tag set.
        */
        virtual void deselectFilter();

        /**
        * Resets the filter.
        */
        virtual void resetFilter();

        /**
        * Returns the currently set filter as string representation.
        * This value is used to store in the database.
        */
        virtual QString getFilterString();

        /**
        * Applies the filter returned by getFilter().
        */
        virtual void applyFilterString(QString filter);

        virtual void leftClicked(TagTree* tagTree, int column);
        virtual void rightClicked(TagTree* tagTree, int column);

        virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment);

    private:
        /**
         * Tests if all given files have the same value to this tagnode.
         * Returns null if none of the files has an assoc to this tagnode.
         * Returns an invalid QDateTime if the values differ.
         * Returns the correct QDateTime if all files have the same value set.
         */
        QDateTime* getCommonValue(const KFileItemList* selectedFiles);

        /**
         * Returns the dateTime represented by the given string. Returns 0 if the given string is invalid.
         */
//        QDateTime* readDateTime(const QString& dateTimeStr);

        /**
         * Returns a string representing the filter set in the @link DialogDateTimeFilter.
         */
//        void applyFilter(DialogDateTimeFilter* dateTimeFilter);

        /**
         * Instantiates a @link DialogDateTimeFilter representing the given filter.
         */
//        DialogDateTimeFilter* createDialogDateTimeFilter(QString filter);

    private:
        DateTimeFilterData* m_data;
        FilterNode* m_filterNode;
};

#endif
