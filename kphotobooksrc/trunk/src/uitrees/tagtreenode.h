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

#include "../tracer/tracer.h"

#include "../engine/tagnode.h"
#include "../engine/filternode.h"

#include <klistview.h>
#include <kpopupmenu.h>

#include <qstring.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qrect.h>

class KPhotoBook;
class TagTree;
class TagTreeNode;

/**
 * Superclass of all nodes to display in the tagtree.
 * It is a subclass of KListViewItem with some extensions.
 *
 * CVS-ID $Id$
 */
class TagTreeNode : public KListViewItem {

private:
    static Tracer* tracer;

public:
    /**
     * Creates a new toplevel tagtreenode in the specified TagTree.
     * @param parent The TagTree to add the created TagTreeNode to.
     * @param photobook The photobook.
     * @param tagNode The tagNode represented by this TagTreeNode.
     * @param contextMenu The contextMenu to display on this TagTreeNode (optional). If not set, no contextMenu is shown.
     */
    TagTreeNode(TagTree* parent, KPhotoBook* photobook, TagNode* tagNode, KPopupMenu* contextMenu = 0);
    TagTreeNode(TagTreeNode* parent,  KPhotoBook* photobook, TagNode* tagNode, KPopupMenu* contextMenu = 0);

    virtual ~TagTreeNode() {
    }

    /**
     * Refreshes the displayed icon and text.
     * Should be called after the represented TagNode has changed or another setting
     * has changed which influences the appearance of this TagTreeNode.
     */
    void refresh();

    KPopupMenu* contextMenu() {
        return m_contextMenu;
    }

    virtual TagNode* tagNode() {
        return m_tagNode;
    }

    /**
     * Returns an FilterNode used to describe this filter.
     */
    virtual FilterNode* filter() {
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
    virtual void deselectFilter() = 0;

    /**
     * Resets the filter.
     */
    virtual void resetFilter() = 0;
    
    /**
     * Returns the currently set filter as string representation.
     * This value is used to store in the database.
     */
    virtual QString getFilterString() {
        return QString::null;
    }
    
    /**
     * Applies the filter returned by getFilter().
     */
    virtual void applyFilterString(__attribute__((unused)) QString filter) {
    }

    /**
     * This method is called by the tagtree when this TagTreeNode is clicked with the
     * left mouse button.
     */
    virtual void leftClicked(__attribute__((unused)) TagTree* tagTree, __attribute__((unused)) int column) {
    }
    /**
     * This method is called by the tagtree when this TagTreeNode is clicked with the
     * right mouse button.
     */
    virtual void rightClicked(__attribute__((unused)) TagTree* tagTree, __attribute__((unused)) int column);

    /**
     * returns the tooltip to be shown in the tree of this node
     */
    virtual QString toolTip()
    {
        return *(m_tagNode->comment());
    }
    
protected:
    KPhotoBook* m_photobook;
    TagNode* m_tagNode;

    KPopupMenu* m_contextMenu;
};

#endif
