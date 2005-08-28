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

#ifndef SOURCEDIRTREENODE_H
#define SOURCEDIRTREENODE_H

#include "../tracer/tracer.h"

#include <klistview.h>
#include <kpopupmenu.h>

#include <qpixmap.h>
#include <qstring.h>

class KPhotoBook;
class SourceDirTree;
class Folder;


/**
 * Specialized KListViewItem representing a Folder to be used in the SourceDirTree.
 *
 * CVS-ID $Id$
 */
class SourceDirTreeNode : public KListViewItem
{
    private:
        static Tracer* tracer;
    
    public:
        /**
        * Creates a new toplevel SourceDirTreeNode in the specified SourceDirTree.
        * @param parent The KListView to add the created SourceDirTreeNode to.
        * @param photobook The photobook.
        * @param sourceDir The Folder to represent in the SourceDirTree.
        * @param contextMenu The contextMenu to display on this SourceDirTreeNode (optional). If not set, no contextMenu is shown.
        */
        SourceDirTreeNode(SourceDirTree* parent, KPhotoBook* photobook, Folder* sourceDir, KPopupMenu* contextMenu = 0);
        SourceDirTreeNode(SourceDirTreeNode* parent, KPhotoBook* photobook, Folder* sourceDir, KPopupMenu* contextMenu = 0);
    
        virtual ~SourceDirTreeNode()
        {
        }
    
        /**
        * Refreshes the displayed icon.
        */
        void refreshIcon();
    
        /**
        * Expands or collapses the whole subtree beginning with this TagTreeNode.
        * @param open Indicates if the subtree must be expanded (true) or collapsed (false).
        */
        void setOpenRecursive(bool open);
    
        /**
        * Returns the source directory represented by this SourceDirTreeNode.
        * @return The source directory represented by this SourceDirTreeNode.
        */
        Folder* sourceDir() {
            return m_sourceDir;
        }
    
        void setSelectedFilesCount(int selectedFilesCount);
        void increaseSelectedFilesCount(int amount)
        {
            setSelectedFilesCount(m_selectedFilesCount + amount);
        }
        int selectedFilesCount()
        {
            return m_selectedFilesCount;
        }
    
        void setIncluded(bool included);
        bool included();
        void invertInclusion();
    
        void setIncludedRecursive(bool included);
        void invertInclusionRecursive();
    
        /**
        * Returns the currently set filter as string representation.
        */
        virtual QString getFilterString();
    
        /**
        * Applies the filter returned by getFilter().
        */
        virtual void applyFilterString(QString filter);
    
        /**
        * This method is called by the SourceDirTree when this SourceDirTreeNode is clicked with the
        * left mouse button.
        * @param sourceDirTree The SourceDirTree we belong to.
        * @param column The column number the user clicked in.
        */
        void leftClicked(SourceDirTree* sourceDirTree, int column);
    
        /**
        * This method is called by the SourceDirTree when this SourceDirTreeNode is clicked with the
        * right mouse button.
        * @param sourceDirTree The SourceDirTree we belong to.
        * @param column The column number the user clicked in.
        */
        void rightClicked(SourceDirTree* sourceDirTree, int column);
    
        void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment);
    
    private:
        KPhotoBook* m_photobook;
        Folder* m_sourceDir;
    
        unsigned int m_selectedFilesCount;
    
        KPopupMenu* m_contextMenu;
    
        void init(bool showRelativePath);
};


#endif
