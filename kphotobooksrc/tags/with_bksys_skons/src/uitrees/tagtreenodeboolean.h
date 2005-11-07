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

#ifndef TAGTREENODEBOOLEAN_H
#define TAGTREENODEBOOLEAN_H

#include "tagtreenode.h"

class TagNodeBoolean;


/**
 * Concrete tagtreenode for displaying a boolean tagnode.
 *
 * CVS-ID $Id: tagtreenodeboolean.h 435 2005-08-02 20:36:17Z starcube $
 */
class TagTreeNodeBoolean : public TagTreeNode
{
    public:
        TagTreeNodeBoolean(TagTree* parent, TagNodeBoolean* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);
        TagTreeNodeBoolean(TagTreeNode* parent, TagNodeBoolean* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);
        
        virtual ~TagTreeNodeBoolean();
    
        /**
        * Returns an FilterNode used to describe this filter.
        */
        virtual FilterNode* filter() const;
    
        virtual void leftClicked(TagTree* tagTree, int column);
        virtual void rightClicked(TagTree* tagTree, int column);
    
        virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment);
};

#endif
