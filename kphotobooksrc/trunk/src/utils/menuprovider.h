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

#ifndef _MENUPROVIDER_H_
#define _MENUPROVIDER_H_

#include "../tracer/tracer.h"

#include <kpopupmenu.h>

class KPhotoBook;


/**
 * This class holds all context menus used in kphotobook.
 * It is implemented using lazy loading of the menus. That means the menus are
 * read from the config during the first call for getting a menu.
 *
 * CVS-ID $Id: constants.h 435 2005-08-02 20:36:17Z starcube $
 */
class MenuProvider
{
    private:
        static Tracer* tracer;
    
    public:
        /**
         * Instantiates this MenuProvider and sets up all menus.
         */
        MenuProvider(KPhotoBook* kphotobook);
        ~MenuProvider()
        {
            // the menus will be deleted automatically as soon m_kphotobook is deleted
        }

        KPopupMenu* contextMenuSourceDirTree();
        KPopupMenu* contextMenuSourceDir();
        KPopupMenu* contextMenuSubDir();
        KPopupMenu* contextMenuTagTree();
        KPopupMenu* contextMenuTagTreeItem();
        KPopupMenu* contextMenuTagTreeItemLeaf();
        KPopupMenu* contextMenuImageViewer();

    private:
        KPopupMenu* foobar (KPopupMenu* menu, QString menuname)
        {
            if (menu) {
                return menu;
            }

            tracer->swarning(__func__) << "menu with internal name '" << menuname << "' does not exist, could not be loaded!" << endl;
            // we just return an empty context menu to avoid crashes!
            return m_emptyContextMenu;
        }

    private:
        KPhotoBook* m_kphotobook;

        KPopupMenu* m_emptyContextMenu;
        
        KPopupMenu* m_contextMenuSourceDirTree;
        KPopupMenu* m_contextMenuSourceDir;
        KPopupMenu* m_contextMenuSubDir;
        
        KPopupMenu* m_contextMenuTagTree;
        KPopupMenu* m_contextMenuTagTreeItem;
        KPopupMenu* m_contextMenuTagTreeItemLeaf;

        KPopupMenu* m_contextMenuImageViewer;
};

#endif
