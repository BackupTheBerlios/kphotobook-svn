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

#ifndef _TAGTREENODE_H_
#define _TAGTREENODE_H_

#include "../engine/filternode.h"
#include "../engine/tagnode.h"
#include "../tracer/tracer.h"

#include <klistview.h>
#include <kpopupmenu.h>

#include <qpainter.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qrect.h>
#include <qstring.h>

class KPhotoBook;
class TagTree;
class TagTreeNode;


/**
 * Superclass of all nodes to display in the tagtree.
 * It is a subclass of KListViewItem with some extensions.
 *
 * CVS-ID $Id$
 */
class TagTreeNode : public KListViewItem
{
    private:
        static Tracer* tracer;
    
    public:
        /**
        * gives information about the tag status. ie if the current tagTreeNode is
        * UNTAGGED for all selected images,
        * TAGGED: all selected images contain this tag
        * MIXTAGGED: some images contain this tag, and some don't
        */
        enum MatchType {
            NOSELECT  = -2,
            UNTAGGED  = -1,
            MIXTAGGED = 0,
            TAGGED    = 1
        };
    
        /**
        * This enumeration is basis of the filterstates of the TagTreeNodes
        */
        enum FilterState {
            FILTERSTATE_EXCLUDE = -1,
            FILTERSTATE_IGNORE,
            FILTERSTATE_INCLUDE
        };
    
        /**
        * Creates a new toplevel tagtreenode in the specified TagTree.
        * @param parent The TagTree to add the created TagTreeNode to.
        * @param photobook The photobook.
        * @param tagNode The tagNode represented by this TagTreeNode.
        * @param contextMenu The contextMenu to display on this TagTreeNode (optional). If not set, no contextMenu is shown.
        */
        TagTreeNode(TagTree* parent, KPhotoBook* photobook, TagNode* tagNode, KPopupMenu* contextMenu = 0);
        TagTreeNode(TagTreeNode* parent,  KPhotoBook* photobook, TagNode* tagNode, KPopupMenu* contextMenu = 0);
    
        virtual ~TagTreeNode()
        {
        }
    
        /**
        * Refreshes the displayed icon and text.
        * Should be called after the represented TagNode has changed or another setting
        * has changed which influences the appearance of this TagTreeNode.
        */
        void refresh();
    
        KPopupMenu* contextMenu()
        {
            return m_contextMenu;
        }
    
        virtual TagNode* tagNode()
        {
            return m_tagNode;
        }
    
        /**
        * Returns an FilterNode used to describe this filter.
        */
        virtual FilterNode* filter()
        {
            return 0;
        }
    
        /**
        * Expands or collapses the whole subtree beginning with this TagTreeNode.
        * @param open Indicates if the subtree must be expanded (true) or collapsed (false).
        */
        void setOpenRecursive(bool open);
    
        /**
        * Sets the filter to find images without this tag set.
        */
        virtual void deselectFilter()
        {
            m_filterState = TagTreeNode::FILTERSTATE_EXCLUDE;
            // force redrawing of this listviewitem
            this->repaint();
        }
    
        /**
        * Resets the filter.
        */
        virtual void resetFilter()
        {
            m_filterState = TagTreeNode::FILTERSTATE_IGNORE;
            // force redrawing of this listviewitem
            this->repaint();
        }
    
        /**
        * Returns the currently set filter as string representation.
        * This value is used to store in the database.
        */
        virtual QString filterString();
    
        /**
        * returns the current state of the filternode
        */
        virtual TagTreeNode::FilterState filterState()
        {
            return m_filterState;
        }
    
        /**
        * sets the filter according to the string
        */
        virtual void applyFilterString(QString filter);
    
    
        /**
        * sets the filter to state
        */
        virtual void applyFilterState(TagTreeNode::FilterState state)
        {
            m_filterState = state;
        }
    
        /**
        * This method is called by the tagtree when this TagTreeNode is clicked with the
        * left mouse button.
        */
        virtual void leftClicked(TagTree* tagTree, int column);
    
        /**
        * This method is called by the tagtree when this TagTreeNode is clicked with the
        * right mouse button.
        */
        virtual void rightClicked(TagTree* tagTree, int column);
    
        /**
        * draws the cells of the tagtreeitems defaultwise
        */
        virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment);
    
    
        /**
        * returns the tooltip to be shown in the tree of this node
        */
        virtual QString toolTip(int column);
        /**
        * updates the internal tagMatch state.
        * That means, all currently selected images are parsed and it is checked, how they match
        * the current TagTreeNode
        */
        virtual void updateTagMatch()
        {
            m_tagCurrentMatch = findTagMatch();
        }
    
        /**
        * recursively asks all my children, if the are Tagged within the current selected images.
        * this also returns true, if a MIXTAGGED state is found, as this is also 'tagged'
        */
        virtual bool recursiveFindTagged();
    
    protected:
    
        /** parses the currently selected Images and looks, if a match can be found */
        MatchType findTagMatch();
    
        KPhotoBook* m_photobook;
        TagNode* m_tagNode;
    
        KPopupMenu* m_contextMenu;
    
        /** saves, whether the selected images have Matched the current tagTreeNode, or not (or Mixed) */
        MatchType m_tagCurrentMatch;
    
        TagTreeNode::FilterState m_filterState;
};

#endif
