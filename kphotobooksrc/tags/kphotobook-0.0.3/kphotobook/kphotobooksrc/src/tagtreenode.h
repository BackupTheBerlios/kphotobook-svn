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

#include <klistview.h>
#include <kpopupmenu.h>
#include <kdebug.h>

#include <qstring.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qrect.h>

class KPhotoBook;
class TagNode;
class TagTree;

/**
 * Superclass of all nodes to display in a tagtree.
 *
 * CVS-ID $Id$
 */
class TagTreeNode : public KListViewItem {

public:
    TagTreeNode(KListView* parent, QString text, KPhotoBook* photobook, QPixmap* icon = 0, KPopupMenu* contextMenu = 0);
    TagTreeNode(KListViewItem* parent, QString text, KPhotoBook* photobook, QPixmap* icon = 0, KPopupMenu* contextMenu = 0);

    virtual ~TagTreeNode() {
    }

    void refresh();

    KPopupMenu* contextMenu() {
        return m_contextMenu;
    }

    virtual TagNode* tagNode() = 0;

    virtual void showContextMenu();

    virtual void columnClicked(__attribute__((unused)) TagTree* tagTree, __attribute__((unused)) int column) {
    }

protected:
    void drawCheckBox(QPainter* p, const QColorGroup& cg, QRect rect, bool checked);

    KPhotoBook* m_photobook;

    KPopupMenu* m_contextMenu;
};

#endif
