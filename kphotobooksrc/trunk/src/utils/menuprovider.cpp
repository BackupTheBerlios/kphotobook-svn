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

#include "menuprovider.h"

#include "../kphotobook.h"

#include <kxmlguifactory.h>


Tracer* MenuProvider::tracer = Tracer::getInstance("kde.kphotobook.utils", "MenuProvider");


MenuProvider::MenuProvider(KPhotoBook* kphotobook) :
    m_kphotobook(kphotobook),
    m_emptyContextMenu(0),
    m_contextMenuSourceDirTree(0),
    m_contextMenuSourceDir(0),
    m_contextMenuSubDir(0),
    m_contextMenuTagTree(0),
    m_contextMenuTagTreeItem(0),
    m_contextMenuTagTreeItemLeaf(0),
    m_contextMenuImageViewer(0)
{
    m_contextMenuSourceDirTree = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_folderTree", m_kphotobook));
    m_contextMenuSourceDir = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_folder", m_kphotobook));
    m_contextMenuSubDir = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_subfolder", m_kphotobook));
    m_contextMenuTagTree = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_tagTree", m_kphotobook));
    m_contextMenuTagTreeItem = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_tagTreeItem", m_kphotobook));
    m_contextMenuTagTreeItemLeaf = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_tagTreeItemLeaf", m_kphotobook));
    m_contextMenuImageViewer = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_imageViewer", m_kphotobook));

    m_emptyContextMenu = new KPopupMenu(m_kphotobook, "emptyContextMenu");
    m_emptyContextMenu->insertTitle(i18n("ERROR: Could not load menu!"), 0);
}


KPopupMenu* MenuProvider::contextMenuSourceDirTree()
{
    return foobar(m_contextMenuSourceDirTree, "contextMenu_folderTree");
}
    

KPopupMenu* MenuProvider::contextMenuSourceDir()
{
    return foobar(m_contextMenuSourceDir, "contextMenu_folder");
}
    

KPopupMenu* MenuProvider::contextMenuSubDir()
{
    return foobar(m_contextMenuSubDir, "contextMenu_subfolder");
}
    

KPopupMenu* MenuProvider::contextMenuTagTree()
{
    return foobar(m_contextMenuTagTree, "contextMenu_tagTree");
}


KPopupMenu* MenuProvider::contextMenuTagTreeItem()
{
    return foobar(m_contextMenuTagTreeItem, "contextMenu_tagTreeItem");
}
    

KPopupMenu* MenuProvider::contextMenuTagTreeItemLeaf()
{
    return foobar(m_contextMenuTagTreeItemLeaf, "contextMenu_tagTreeItemLeaf");
}


KPopupMenu* MenuProvider::contextMenuImageViewer()
{
    return foobar(m_contextMenuImageViewer, "contextMenu_imageViewer");
}

