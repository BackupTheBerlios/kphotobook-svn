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


MenuProvider::MenuProvider(KPhotoBook* kphotobook) :
    m_kphotobook(kphotobook)
{
    m_contextMenuSourceDirTree = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_folderTree", m_kphotobook));
    m_contextMenuSourceDir = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_folder", m_kphotobook));
    m_contextMenuSubDir = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_subfolder", m_kphotobook));

    m_contextMenuTagTree = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_tagTree", m_kphotobook));
    m_contextMenuTagTreeItem = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_tagTreeItem", m_kphotobook));
    m_contextMenuTagTreeItemLeaf = static_cast<KPopupMenu*>(m_kphotobook->guiFactory()->container("contextMenu_tagTreeItemLeaf", m_kphotobook));
}
