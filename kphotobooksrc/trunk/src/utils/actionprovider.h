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

#ifndef _ACTIONPROVIDER_H_
#define _ACTIONPROVIDER_H_


class ImageViewer;
class KPhotoBook;
class Tracer;

#include <kurl.h>
class KAction;
class KActionCollection;
class KToggleAction;
class KToolBarPopupAction;
class KRecentFilesAction;


/**
 * This class holds all actions used in kphotobook.
 *
 * CVS-ID $Id: constants.h 435 2005-08-02 20:36:17Z starcube $
 */
class ActionProvider
{
    private:
        static Tracer* tracer;
    
    public:
        /**
         * Instantiates this ActionProvider and sets up all actions.
         */
        ActionProvider(KPhotoBook* kphotobook);
        ~ActionProvider()
        {
            // actions must not be deleted explicitely. the actioncollection manages all actions!
        }

        void addRecentFile(KURL url);
        
        void saveRecentFiles() const;
        
    private:
        void setupFileActions();
        void setupSettingActions();
        void setupHelpActions();
        void setupExportActions();
        void setupImportActions();
        void setupEngineActions();
        void setupViewActions();
        void setupFolderActions();
        void setupTagActions();
        void setupToolviewActions();

        inline KActionCollection* actionCollection();
        
    public:
        // file actions
        KAction* m_new;
        KAction* m_open;
        KRecentFilesAction* m_recentFiles;
        KAction* m_save;
        KAction* m_saveAs;
        KAction* m_close;
        KAction* m_quit;
        KAction* m_fullScreen;

        // setting actions
        KAction* m_keyBindings;
        KAction* m_configureToolbars;
        KAction* m_preferences;

        // help actions
        KAction* m_tipOfDay;

        // export actions
        KAction*m_exportMatchingFiles;
        KAction* m_exportSelectedFiles;

        // import actions
        KAction* m_importImages;

        // engine actions
        KAction* m_rescanFilesystem;

        // view actions
        KToggleAction* m_autoRefreshView;
        KAction* m_refreshView;
        KToolBarPopupAction* m_increasePreviewSize;
        KToolBarPopupAction* m_decreasePreviewSize;

        // folder actions
        KAction* m_addFolder;
        KAction* m_editFolder;
        KAction* m_removeFolder;
        KAction* m_includeWholeFolder;
        KAction* m_excludeWholeFolder;
        KAction* m_invertFolderSelection;
        KAction* m_includeAllFolders;
        KAction* m_excludeAllFolders;
        KAction* m_invertAllFolderSelection;    
        KAction* m_expandFolder;
        KAction* m_collapseFolder;
        KAction* m_expandAllFolders;
        KAction* m_collapseAllFolders;

        // tag actions
        KAction* m_addMaintag;
        KAction* m_createSubtag;
        KAction* m_editTag;    
        KAction* m_deleteTag;
        KToggleAction* m_toggleLockUnlockTagging;
        KToggleAction* m_andifyTags;
        KToggleAction* m_orifyTags;
        KAction* m_deselectFilter;
        KAction* m_resetFilter;
        KAction* m_expandTag;
        KAction* m_collapseTag;
        KAction* m_expandAllTags;
        KAction* m_collapseAllTags;

        // toolview actions
        KAction* m_restoreToolviews;
        KAction* m_showTagTree;
        KAction* m_showFolderTree;

        // imageviewer actions
        KAction* m_viewerBack;
        KAction* m_viewerForward;
        KToggleAction* m_viewerFullScreen;
        KToggleAction* m_viewerSmoothScaling;
        KToggleAction* m_viewerShowImageCounter;
        KToggleAction* m_viewerShowImageInfos;
        KAction* m_viewerQuit;
        
    private:
        KPhotoBook* m_kphotobook;
};

#endif
