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
 *   along with m_kphotobook program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "actionprovider.h"

#include "../constants.h"
#include "../kphotobook.h"
#include "../tracer/tracer.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kstdaccel.h>
#include <kstdaction.h>


Tracer* ActionProvider::tracer = Tracer::getInstance("kde.kphotobook.utils", "ActionProvider");


ActionProvider::ActionProvider(KPhotoBook* kphotobook) :
    m_kphotobook(kphotobook)
{
    setupFileActions();
    setupSettingActions();
    setupHelpActions();
    setupExportActions();
    setupImportActions();
    setupEngineActions();
    setupViewActions();
    setupFolderActions();
    setupTagActions();
    setupToolviewActions();
}


void ActionProvider::addRecentFile(KURL url)
{
    m_recentFiles->addURL(url);
}
        

void ActionProvider::saveRecentFiles() const
{
    m_recentFiles->saveEntries(kapp->config());
}


void ActionProvider::setupFileActions()
{
    m_new = KStdAction::openNew(m_kphotobook, SLOT(slotFileNew()), actionCollection());
    m_new->setWhatsThis(i18n("Create a new KPhotoBook database."));

    m_open = KStdAction::open(m_kphotobook, SLOT(slotFileOpen()), actionCollection());
    m_open->setWhatsThis(i18n("Open an existing KPhotoBook database."));

    m_recentFiles = KStdAction::openRecent(m_kphotobook, SLOT(slotFileOpen(const KURL&)), actionCollection());
    m_recentFiles->setWhatsThis(i18n("Open a previously opened KPhotoBook database."));
    m_recentFiles->loadEntries(kapp->config());

    m_save = KStdAction::save(m_kphotobook, SLOT(slotFileSave()), actionCollection());
    m_save->setWhatsThis(i18n("Save the current KPhotoBook database."));
    m_save->setEnabled(false);

    m_saveAs = KStdAction::saveAs(m_kphotobook, SLOT(slotFileSaveAs()), actionCollection());
    m_saveAs->setWhatsThis(i18n("Save the current KPhotoBook database as a new file."));

    m_close = KStdAction::close(m_kphotobook, SLOT(close()), actionCollection());
    m_close->setWhatsThis(i18n("Close m_kphotobook window."));

    m_quit = KStdAction::quit(kapp, SLOT(closeAllWindows()), actionCollection());
    m_quit->setWhatsThis(i18n("Close all windows and quit."));

    m_fullScreen = KStdAction::fullScreen(m_kphotobook, SLOT(slotToggleFullscreen()), actionCollection(), m_kphotobook);
    m_fullScreen->setWhatsThis(i18n("This toggles Fullscreen mode of the main programm."));
}


void ActionProvider::setupSettingActions()
{
    m_keyBindings = KStdAction::keyBindings(m_kphotobook, SLOT(slotOptionsConfigureKeys()), actionCollection());
    m_keyBindings->setWhatsThis(i18n("Configure the application's keyboard shortcut assignments."));

    m_configureToolbars = KStdAction::configureToolbars(m_kphotobook, SLOT(slotOptionsConfigureToolbars()), actionCollection());
    m_configureToolbars->setWhatsThis(i18n("Configure which items should appear in the toolbars."));

    m_preferences = KStdAction::preferences(m_kphotobook, SLOT(slotOptionsPreferences()), actionCollection());
    m_preferences->setWhatsThis(i18n("Configure KPhotoBook."));
}


void ActionProvider::setupHelpActions()
{
    m_tipOfDay = KStdAction::tipOfDay(m_kphotobook, SLOT(slotTipOfDay()), actionCollection());
    m_tipOfDay->setWhatsThis(i18n("Show the 'Tip of Day' dialog."));
}


void ActionProvider::setupExportActions()
{
    m_exportMatchingFiles = new KAction(i18n("&Matching Files"), 0, 0, m_kphotobook, SLOT(slotExportMatchingFiles()), actionCollection(), "exportMatchingFiles");
    m_exportMatchingFiles->setWhatsThis(i18n("Exports all files matching the current filter as symbolic links."));
    m_exportMatchingFiles->setEnabled(false);

    m_exportSelectedFiles = new KAction(i18n("&Selected Files"), 0, 0, m_kphotobook, SLOT(slotExportSelectedFiles()), actionCollection(), "exportSelectedFiles");
    m_exportSelectedFiles->setWhatsThis(i18n("Exports all selected files as symbolic links."));
    m_exportSelectedFiles->setEnabled(false);
}


void ActionProvider::setupImportActions()
{
    m_importImages = new KAction(i18n("Images"), 0, 0, m_kphotobook, SLOT(slotImportImages()), actionCollection(), "importImages");
}


void ActionProvider::setupEngineActions()
{
    m_rescanFilesystem = new KAction(i18n("&Rescan filesystem"), Constants::ICON_RESCAN_FILESYSTEM, 0, m_kphotobook, SLOT(slotRescanFilesystem()), actionCollection(), "rescanFilesystem");
    m_rescanFilesystem->setWhatsThis(i18n("This may, for example, be needed after having added a new folder to a folder which is already part of the current database. New folders and files will be added to the database, removed items will be prompted for user input."));
}


void ActionProvider::setupViewActions()
{
    m_autoRefreshView = new KToggleAction(i18n("&Autorefresh view"), Constants::ICON_AUTOREFRESH_VIEW, 0, m_kphotobook, SLOT(slotAutoRefreshView()), actionCollection(), "autoRefreshView");
    m_autoRefreshView->setWhatsThis(i18n("This will reload the thumbnail window automatically after, for example, changing the tag filter or the folders to be shown."));
    KShortcut autoRefreshViewShortCut = KShortcut();
    autoRefreshViewShortCut.append(KKey("CTRL+SHIFT+F5"));
    autoRefreshViewShortCut.append(KKey("CTRL+R"));
    m_autoRefreshView->setShortcut(autoRefreshViewShortCut);

    m_refreshView = new KAction(i18n("&Refresh view"), Constants::ICON_REFRESH_VIEW, 0, m_kphotobook, SLOT(slotRefreshView()), actionCollection(), "refreshView");
    m_refreshView->setWhatsThis(i18n("This may, for example, be needed after having changed the tag filter or the folders to be shown."));
    KShortcut refreshViewShortCut = KShortcut();
    refreshViewShortCut.append(KStdAccel::shortcut(KStdAccel::Reload));
    refreshViewShortCut.append(KKey("CTRL+r"));
    m_refreshView->setShortcut(refreshViewShortCut);
    
    m_increasePreviewSize = new KToolBarPopupAction (i18n("&Increase Previewsize"), Constants::ICON_INCREASE_PREVIEWSIZE, KStdAccel::shortcut(KStdAccel::ZoomIn), m_kphotobook, SLOT(slotIncreasePreviewSize()), actionCollection(), "increasePreviewSize");
    m_increasePreviewSize->setWhatsThis(i18n("Make the preview size in the thumbnail window bigger."));// Click and hold down the mouse button for a menu with a set of available preview sizes."));
    QObject::connect(m_increasePreviewSize->popupMenu(), SIGNAL(activated(int)), m_kphotobook, SLOT(slotChangePreviewSizeActivated(int)));
    QObject::connect(m_increasePreviewSize->popupMenu(), SIGNAL(aboutToShow()), m_kphotobook, SLOT(slotIncPreviewSizePopupAboutToShow()));

    m_decreasePreviewSize = new KToolBarPopupAction(i18n("&Decrease Previewsize"), Constants::ICON_DECREASE_PREVIEWSIZE, KStdAccel::shortcut(KStdAccel::ZoomOut), m_kphotobook, SLOT(slotDecreasePreviewSize()), actionCollection(), "decreasePreviewSize");
    m_decreasePreviewSize->setWhatsThis(i18n("Make the preview size in the thumbnail window smaller."));// Click and hold down the mouse button for a menu with a set of available preview sizes."));

    QObject::connect(m_decreasePreviewSize->popupMenu(), SIGNAL(activated(int)), m_kphotobook, SLOT(slotChangePreviewSizeActivated(int)));
    QObject::connect(m_decreasePreviewSize->popupMenu(), SIGNAL(aboutToShow()), m_kphotobook, SLOT(slotDecPreviewSizePopupAboutToShow()));
}


void ActionProvider::setupFolderActions()
{
    m_addFolder = new KAction(i18n("Add &folder"), Constants::ICON_ADD_SOURCEDIR, 0, m_kphotobook, SLOT(slotAddSourcedir()), actionCollection(), "addFolder");
    m_addFolder->setWhatsThis(i18n("Add a folder to the database. This can happen to a single folder or recursively. "));

    m_editFolder= new KAction(i18n("&Edit folder"), Constants::ICON_EDIT_SOURCEDIR, KShortcut(KKey("F3")), m_kphotobook, SLOT(slotEditSourceDir()), actionCollection(), "editFolder");
    m_editFolder->setWhatsThis(i18n("Edit the properties of the folder. I.e. change the location of the folder on the filesystem."));
    m_editFolder->setEnabled(false);

    m_removeFolder = new KAction(i18n("&Remove folder"), Constants::ICON_REMOVE_SOURCEDIR, 0, m_kphotobook, SLOT(slotRemoveSourceDir()), actionCollection(), "removeFolder");
    m_removeFolder->setWhatsThis(i18n("Remove the selected folder with all its subfolders. The files which are in m_kphotobook folder will be removed from the database - losing the tag associatioins."));

    m_includeWholeFolder = new KAction(i18n("&Include whole folder"), Constants::ICON_INCLUDE_WHOLE_FOLDER, 0, m_kphotobook, SLOT(slotIncludeWholeSourceDir()),actionCollection(), "includeWholeFolder");
    m_includeWholeFolder->setWhatsThis(i18n("Include the selected folder and all subfolders to the thumbnail preview."));

    m_excludeWholeFolder = new KAction(i18n("&Exclude whole folder"), Constants::ICON_EXCLUDE_WHOLE_FOLDER, 0, m_kphotobook, SLOT(slotExcludeWholeSourceDir()), actionCollection(), "excludeWholeFolder");
    m_excludeWholeFolder->setWhatsThis(i18n("Exclude the current folder and all subfolders from the thumbnail preview."));

    m_invertFolderSelection = new KAction(i18n("In&vert folder selection"), Constants::ICON_INVERT_FOLDER_SELECTION, 0, m_kphotobook, SLOT(slotInvertSourceDir()), actionCollection(), "invertFolderSelection");
    m_invertFolderSelection->setWhatsThis(i18n("Exclude the included folders and include the excluded folders of the current folder to the thumbnail view."));

    m_includeAllFolders = new KAction(i18n("&Include all"), Constants::ICON_INCLUDE_WHOLE_FOLDER, 0, m_kphotobook, SLOT(slotIncludeAllSourceDirs()), actionCollection(), "includeAllFolders");
    m_includeAllFolders->setWhatsThis(i18n("Include all folders with all subfolders to the thumbnail view."));

    m_excludeAllFolders = new KAction(i18n("&Exclude all"), Constants::ICON_EXCLUDE_WHOLE_FOLDER, 0, m_kphotobook, SLOT(slotExcludeAllSourceDirs()), actionCollection(), "excludeAllFolders");
    m_excludeAllFolders->setWhatsThis(i18n("Exclude all folders with all subfolders from the thumbnail view."));

    m_invertAllFolderSelection = new KAction(i18n("In&vert all"), Constants::ICON_INVERT_FOLDER_SELECTION, 0, m_kphotobook, SLOT(slotInvertAllSourceDirs()), actionCollection(), "invertAllFolderSelection");
    m_invertAllFolderSelection->setWhatsThis(i18n("Exclude the included folder and include the excluded folders to the thumbnail view."));

    m_expandFolder = new KAction(i18n("Expand folder"), Constants::ICON_EXPAND_FOLDER, 0, m_kphotobook, SLOT(slotExpandSourceDir()), actionCollection(), "expandFolder");
    m_expandFolder->setWhatsThis(i18n("Expand all children of the selected folder."));

    m_collapseFolder = new KAction(i18n("Collapse folder"), Constants::ICON_COLLAPSE_FOLDER, 0, m_kphotobook, SLOT(slotCollapseSourceDir()), actionCollection(), "collapseFolder");
    m_collapseFolder->setWhatsThis(i18n("Hide the subtree of the selected folder."));

    m_expandAllFolders = new KAction(i18n("Expand all folders"), Constants::ICON_EXPAND_FOLDER, 0, m_kphotobook, SLOT(slotExpandAllSourceDirs()), actionCollection(), "expandAllFolders");
    m_expandAllFolders->setWhatsThis(i18n("Expand the the whole folder tree."));

    m_collapseAllFolders = new KAction(i18n("Collapse all folders"), Constants::ICON_COLLAPSE_FOLDER, 0, m_kphotobook, SLOT(slotCollapseAllSourceDirs()), actionCollection(), "collapseAllFolders");
    m_collapseAllFolders->setWhatsThis(i18n("Collapse the whole folder tree."));
}


void ActionProvider::setupTagActions()
{
    m_addMaintag = new KAction(i18n("Add &maintag"), Constants::ICON_CREATE_MAINTAG, 0, m_kphotobook, SLOT(slotAddMaintag()), actionCollection(), "addMaintag");
    m_addMaintag->setWhatsThis(i18n("Add a new main tag to the database. This is a top level tag which can contain sub-tags."));

    m_createSubtag = new KAction(i18n("&Create subtag"), Constants::ICON_CREATE_SUBTAG, 0, m_kphotobook, SLOT(slotCreateSubtag()), actionCollection(), "createSubtag");
    m_createSubtag->setWhatsThis(i18n("Creates a tag as child of the selected tag."));

    m_editTag = new KAction(i18n("&Edit tag"), Constants::ICON_EDIT_TAG, KShortcut(KKey("F2")), m_kphotobook, SLOT(slotEditTag()), actionCollection(), "editTag");
    m_editTag->setWhatsThis(i18n("Change the attributes of m_kphotobook tag."));

    m_deleteTag = new KAction(i18n("&Delete tag"), Constants::ICON_DELETE_TAG, 0, m_kphotobook, SLOT(slotDeleteTag()), actionCollection(), "deleteTag");
    m_deleteTag->setWhatsThis(i18n("Deletes the tag after accepting a confirmation dialog."));

    m_toggleLockUnlockTagging = new KToggleAction(i18n("Lock Tagging"), 0, m_kphotobook, SLOT(slotToggleLockUnlockTagging()), actionCollection(), "toggleLockUnlockTagging");
    m_toggleLockUnlockTagging->setWhatsThis(i18n("If locking is enabled it is not possible to change the tags of an image. The lock is also disabled while the CTRL-key is pressed."));

    m_andifyTags = new KToggleAction(i18n("Tag filter operator = AND"), Constants::ICON_OPERATOR_AND, 0, m_kphotobook, SLOT(slotAndifyTags()), actionCollection(), "andifyTags");
    m_andifyTags->setWhatsThis(i18n("This sets the filter operator to 'AND', which means that only images which contain all in the filter marked tags will be shown."));

    m_orifyTags = new KToggleAction(i18n("Tag filter operator = OR"), Constants::ICON_OPERATOR_OR, 0, m_kphotobook, SLOT(slotOrifyTags()), actionCollection(), "orifyTags");
    m_orifyTags->setWhatsThis(i18n("This sets the filter operator to 'OR', which means that pictures with any in the filter marked tags will be shown."));

    m_deselectFilter = new KAction(i18n("Set filter to show all untagged images"), Constants::ICON_TAG_FILTER_DESELECT, 0, m_kphotobook, SLOT(slotDeselectFilter()), actionCollection(), "deselectFilter");
    m_deselectFilter->setWhatsThis(i18n("Deselects all filters. In the filters 'AND' mode, all images without a associated tag are shown only. This is useful for finding new added images without having a tag yet."));

    m_resetFilter = new KAction(i18n("Reset Filter"), Constants::ICON_TAG_FILTER_RESET, 0, m_kphotobook, SLOT(slotResetFilter()), actionCollection(), "resetFilter");
    m_resetFilter->setWhatsThis(i18n("Sets the filter that every image is shown."));

    m_expandTag = new KAction(i18n("Expand tag"), Constants::ICON_EXPAND_FOLDER, 0, m_kphotobook, SLOT(slotExpandTag()), actionCollection(), "expandTag");
    m_expandTag->setWhatsThis(i18n("Expand all children of the tag."));

    m_collapseTag = new KAction(i18n("Collapse tag"), Constants::ICON_COLLAPSE_FOLDER, 0, m_kphotobook, SLOT(slotCollapseTag()), actionCollection(), "collapseTag");
    m_collapseTag->setWhatsThis(i18n("Hide the subtree of the tag."));

    m_expandAllTags = new KAction(i18n("Expand all tags"), Constants::ICON_EXPAND_FOLDER, 0, m_kphotobook, SLOT(slotExpandAllTags()), actionCollection(), "expandAllTags");
    m_expandAllTags->setWhatsThis(i18n("Expand the whole tag tree."));

    m_collapseAllTags = new KAction(i18n("Collapse all tags"), Constants::ICON_COLLAPSE_FOLDER, 0, m_kphotobook, SLOT(slotCollapseAllTags()), actionCollection(), "collapseAllTags");
    m_collapseAllTags->setWhatsThis(i18n("Collapse the whole tag tree."));
}


void ActionProvider::setupToolviewActions()
{
    m_restoreToolviews = new KAction(i18n("Restore toolviews"), Constants::ICON_RESTORE_TOOL_VIEWS, 0, m_kphotobook, SLOT(slotRestoreToolViews()), actionCollection(), "restoreToolViews");
    m_restoreToolviews->setWhatsThis(i18n("Rearrange the toolviews (Tagtree, ...) to their default position."));

    m_showTagTree = new KAction(i18n("Show tagtree"), 0, m_kphotobook, SLOT(slotShowToolViewTagTree()), actionCollection(), "showToolViewTagTree");
    m_showTagTree->setWhatsThis(i18n("Display the tagtree toolview."));

    m_showFolderTree = new KAction(i18n("Show foldertree"), 0, m_kphotobook, SLOT(slotShowToolViewSourceDirTree()), actionCollection(), "showToolViewFolderTree");
    m_showFolderTree->setWhatsThis(i18n("Display the folder toolview."));
}


KActionCollection* ActionProvider::actionCollection() {
    return m_kphotobook->actionCollection();
}
