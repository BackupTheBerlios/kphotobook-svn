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

#ifndef TAGTREENODESOURCEDIR_H
#define TAGTREENODESOURCEDIR_H

#include "tagtreenode.h"

class SourceDir;

class TagTreeNodeSourceDir : public TagTreeNode {

public:
    TagTreeNodeSourceDir(KListView* parent, SourceDir* sourceDir, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);
    TagTreeNodeSourceDir(KListViewItem* parent, SourceDir* sourceDir, KPhotoBook* photobook, KPopupMenu* contextMenu = 0);

    virtual ~TagTreeNodeSourceDir() {
    }

    TagNode* tagNode() {
        return 0;
    }

    SourceDir* sourceDir() {
        return m_sourceDir;
    }

    void setSelectedFilesCount(int selectedFilesCount);
    void increaseSelectedFilesCount(int amount) {
        setSelectedFilesCount(m_selectedFilesCount + amount);
    }
    int selectedFilesCount() {
        return m_selectedFilesCount;
    }

    void setIncluded(bool included);
    bool included();
    void invertInclusion();

    void setIncludedRecursive(bool included);
    void invertInclusionRecursive();

    virtual void columnClicked(TagTree* tagTree, int column);
    virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment);

private:
    SourceDir* m_sourceDir;

    unsigned int m_selectedFilesCount;

    void init(bool showRelativePath);
};

#endif
