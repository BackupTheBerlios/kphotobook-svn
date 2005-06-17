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

#ifndef TAGTREENODESTRING_H
#define TAGTREENODESTRING_H

#include "tagtreenode.h"
#include "../tracer/tracer.h"

#include <kfileitem.h>

class TagNodeString;


/**
 * Concrete tagtreenode for displaying a string tagnode.
 *
 * CVS-ID $Id: tagtreenodeboolean.h 274 2005-03-25 08:52:15Z choenig $
 */
class TagTreeNodeString : public TagTreeNode
{
    private:
        static Tracer* tracer;
    
    public:
        TagTreeNodeString(TagTree* parent, TagNodeString* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);
    
        TagTreeNodeString(TagTreeNode* parent, TagNodeString* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);
    
        virtual ~TagTreeNodeString();
    
        void setFilterValue(QString filterValue) {
    
            m_filterValue = filterValue;
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
        virtual QString filterString() {
            return m_filterValue;
        }
    
    
        /**
        * Applies the filter returned by getFilter().
        */
        virtual void applyFilterString(QString filter);
    
        virtual void leftClicked(TagTree* tagTree, int column);
        virtual void rightClicked(TagTree* tagTree, int column);
    
        /**
        * This method is called by the tagtree after the value or filter
        * was changed by the user.
        */
        void handleRenaming(int column, const QString& text);
    
        virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment);

    private:
        /**
         * Tests if all given files have the same value to this tagnode.
         * Returns null if none of the files has an assoc to this tagnode.
         * Returns a null QString if the values differ.
         * Returns the correct QString if all files have the same value set.
         */
        QString* getCommonValue(const KFileItemList* selectedFiles);
        
    private:
        QString m_filterValue;
};

#endif
