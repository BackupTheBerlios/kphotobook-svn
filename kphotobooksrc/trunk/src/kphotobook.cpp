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

#include "kphotobook.h"

#include "constants.h"

#include "settings/settings.h"
#include "settings/settingsgeneral.h"
#include "settings/settingstagtree.h"
#include "settings/settingssourcedirtree.h"
#include "settings/settingsimagepreview.h"
#include "settings/settingstools.h"
#include "settings/settingsfilehandling.h"

#include "kphotobookview.h"
#include "engine/engine.h"
#include "engine/file.h"
#include "exception.h"
#include "dialogs/dialogaddsourcedir.h"
#include "dialogs/dialogcreatetag.h"
#include "dialogs/dialogedittag.h"

#include "engine/tagnode.h"
#include "uitrees/tagtree.h"
#include "uitrees/tagtreenode.h"
#include "uitrees/tagtreenodetitle.h"
#include "uitrees/tagtreenodeboolean.h"
#include "uitrees/tagtreenode.h"

#include "engine/sourcedir.h"
#include "uitrees/sourcedirtree.h"
#include "uitrees/sourcedirtreenode.h"

#include "export/exportsymlinks.h"


#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdeversion.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kkeydialog.h>
#include <kaccel.h>
#include <kio/netaccess.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <kurldrag.h>
#include <kurlrequesterdlg.h>
#include <kmessagebox.h>
#include <kedittoolbar.h>
#include <kstdaccel.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kfileitem.h>
#include <kconfigdialog.h>
#include <kcombobox.h>
#include <ktoolbar.h>
#include <klistview.h>
#include <kfilemetainfo.h>
#include <kedittoolbar.h>
#include <kmditoolviewaccessor.h>
#include <kurlrequesterdlg.h>

#include <kdebug.h>

#include <qdragobject.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qfileinfo.h>
#include <qptrlist.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qsizepolicy.h>
#include <qheader.h>
#include <qstringlist.h>

#include <typeinfo>


KPhotoBook::KPhotoBook(KMdi::MdiMode mdiMode)
    : KMdiMainFrm(0, "kphotobookMainWindow", mdiMode)
    , m_view(0)
    , m_tagTree(0)
    , m_sourcedirTree(0)
    , m_metaInfoTree(0)
    , m_engine(new Engine())
    
    , m_inTagtreeTemporaryUnlocking(false)
    , m_tagtreeWasLocked(false)

    , m_tagTreeToolBar(0)
    , m_sourceDirTreeToolBar(0)

    , m_autoRefreshViewAction(0)
    , m_zoomIn(0)
    , m_zoomOut(0)
    , m_save(0)
    , m_andifyTagsAction(0)
    , m_orifyTagsAction(0)

    , m_contextMenuSourceDirTree(0)
    , m_contextMenuSourceDir(0)
    , m_contextMenuSubDir(0)
    , m_contextMenuTagTree(0)
    , m_contextMenuTagTreeItem(0)

    , m_settingsGeneral(0)
    , m_settingsImagePreview(0)
    , m_settingsTagTree(0)
    , m_settingsSourceDirTree(0)
    , m_settingsFileHandling(0)
    , m_settingsTools(0) {

    // no idea why i call this, but it sounds good (because i do not really want an mdi application...)
    fakeSDIApplication();

    // accept dnd
    setAcceptDrops(false);

    // then, setup our actions
    setupActions();

    // show toggle menu entry for statusbar
    createStandardStatusBarAction();

    // create the gui
    createGUI(0);

    // let's setup our context menus
    setupContextMenus();

    // it is important to create the view after setting up context menus
    m_view = new KPhotoBookView(this);

    // add the KPhotoBookView as mainwindow
    addWindow(m_view);

    // create toolwindows
    setupToolWindowTagTree();
    setupToolWindowSourceDirTree();
    setupToolWindowMetaInfoTree();
    
    // init some other things: statusbar,..
    init();

    // apply the saved mainwindow settings, if any, and ask the mainwindow
    // to automatically save settings if changed: window size, toolbar
    // position, icon size, etc.
    setAutoSaveSettings();

    // read dock configuration
    readDockConfig(KGlobal::config(), "DockConfig");
}


KPhotoBook::~KPhotoBook() {

    delete m_engine;
    closeWindow(m_view);
}


void KPhotoBook::init() {

    // and a status bar
    statusBar()->show();
    statusBar()->insertItem(i18n("Initialized"), 1, 100);
    statusBar()->setItemAlignment(1, QLabel::AlignLeft|QLabel::AlignVCenter);
    statusBar()->insertItem(i18n("Files: ??"), 2);
    statusBar()->setItemAlignment(2, QLabel::AlignRight|QLabel::AlignVCenter);
    statusBar()->insertItem(i18n("Matched: ??"), 3);
    statusBar()->setItemAlignment(3, QLabel::AlignRight|QLabel::AlignVCenter);
    statusBar()->insertItem(i18n("Selected: 0"), 4);
    statusBar()->setItemAlignment(4, QLabel::AlignRight|QLabel::AlignVCenter);

    // show toggle menu entries for all toolbars
    setStandardToolBarMenuEnabled(true);

    // allow the view to change the statusbar
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString&)), this, SLOT(changeStatusbar(const QString&)));

    // reflect the current settings
    slotLoadSettings();
}


QString KPhotoBook::currentURL() {

    if (m_engine && m_engine->fileinfo()) {
        return m_engine->fileinfo()->absFilePath();
    } else {
        return "";
    }
}


QPtrList<SourceDir>* KPhotoBook::sourceDirs() {
    return m_engine->sourceDirs();
}


QPtrList<TagNode>* KPhotoBook::tagForest() {
    return m_engine->tagForest();
}


void KPhotoBook::dirtyfy() {

    m_engine->dirtyfy();
    updateState();
}


bool KPhotoBook::isTagTextValid(TagTreeNode* parent, QString& text) {

    TagNode* tagNode = 0;
    if (parent) {
        tagNode = parent->tagNode();
    }

    return m_engine->isTagTextValid(tagNode, text);
}


void KPhotoBook::load(QFileInfo& fileinfo) {

    kdDebug() << "[KPhotoBook::load] invoked with file: " << fileinfo.absFilePath() << endl;

    Engine* newEngine = 0;
    try {
        newEngine = new Engine(fileinfo);
    } catch(EngineException* ex) {
        kdError() << "[KPhotoBook::load] Caught an exception during loading the file '" << fileinfo.absFilePath() << "'. Aborting loading. Exception: " << ex->toString() << endl;
        QString msg = QString(i18n("Could not open file: '%1'")).arg(fileinfo.absFilePath());
        KMessageBox::detailedSorry(m_view, msg, ex->toString(), i18n("Opening file failed"));

        // TODO
        // it's very strange, but the application crashes if I delete the exception!!!
//        delete ex;
    }

    kdDebug() << "[KPhotoBook::load] newEngine instantiated: " << newEngine << endl;

    // load the new engine if there is any
    if (newEngine) {

        kdDebug() << "[KPhotoBook::load] deleting engine: " << m_engine << endl;

        // delete current engine
        delete m_engine;

        // set the new angine as current engine
        m_engine = newEngine;
        Settings::setFileSystemLastOpenedFile(currentURL());

        updateState();

        //
        // update the view
        //

        // add the tagNodes to the tagtree
        m_tagTree->addTagNodes(tagForest());

        // add the sourcedirectories to the tagtree
        m_sourcedirTree->clear();
        m_sourcedirTree->addSourceDirs(m_engine->sourceDirs());

        // restore the tree states
        loadTreeState();
        loadFilter();

        // add the files to the view
        m_view->updateFiles();
    } else {
        kdDebug() << "[KPhotoBook::load] nothing done, m_engine: " << m_engine << endl;
    }
}


void KPhotoBook::setupActions() {

    //
    // file menu
    //
    KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection())->setWhatsThis(i18n("Create a new KPhotoBook database."));
    
    KStdAction::open(this, SLOT(slotFileOpen()), actionCollection())->setWhatsThis(i18n("Open an existing KPhotoBook database."));
    
    m_save = KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
    m_save->setWhatsThis(i18n("Save the current KPhotoBook database."));
    m_save->setEnabled(false);
    
    KStdAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection())->setWhatsThis(i18n("Save the current KPhotoBook database as a new file."));
    
    KStdAction::close(this, SLOT(close()), actionCollection())->setWhatsThis(i18n("Close this window."));
    
    KStdAction::quit(kapp, SLOT(closeAllWindows()), actionCollection())->setWhatsThis(i18n("Close all windows and quit."));


    //
    // settings menu
    //
    KStdAction::keyBindings(this, SLOT(slotOptionsConfigureKeys()), actionCollection())->setWhatsThis(i18n("Configure the application's keyboard shortcut assignments."));
    
    KStdAction::configureToolbars(this, SLOT(slotOptionsConfigureToolbars()), actionCollection())->setWhatsThis(i18n("Configure which items should appear in the toolbars."));
    
    KStdAction::preferences(this, SLOT(slotOptionsPreferences()), actionCollection())->setWhatsThis(i18n("Configure KPhotoBook."));

    
    //
    // export actions
    //
    m_exportMatchingFiles = new KAction(
        i18n("&Matching Files"), 0,
        0,
        this, SLOT(slotExportMatchingFiles()),
        actionCollection(), "exportMatchingFiles"
    );
    m_exportMatchingFiles->setWhatsThis(i18n("Exports all files matching the current filter as symbolic links."));
    m_exportMatchingFiles->setEnabled(false);

    m_exportSelectedFiles = new KAction(
        i18n("&Selected Files"), 0,
        0,
        this, SLOT(slotExportSelectedFiles()),
        actionCollection(), "exportSelectedFiles"
    );
    m_exportSelectedFiles->setWhatsThis(i18n("Exports all selected files as symbolic links."));
    m_exportSelectedFiles->setEnabled(false);

    
    //
    // engine actions
    //
    new KAction(
        i18n("&Rescan filesystem"), Constants::ICON_RESCAN_FILESYSTEM,
        0,
        this, SLOT(slotRescanFilesystem()),
        actionCollection(), "rescanFilesystem"
    );
    actionCollection()->action("rescanFilesystem")->setWhatsThis(i18n("This may, for example, be needed after having added a new directory to a directory which is already part of the current database. New directories and files will be added to the database, removed items will be prompted for user input."));

    
    //
    // view actions
    //
    KShortcut autoRefreshViewShortCut(KKey("CTRL+SHIFT+F5"));
    autoRefreshViewShortCut.append(KKey("CTRL+R"));
    m_autoRefreshViewAction = new KToggleAction(
        i18n("&Autorefresh view"), Constants::ICON_AUTOREFRESH_VIEW,
        autoRefreshViewShortCut,
        this, SLOT(slotAutoRefreshView()),
        actionCollection(), "autoRefreshView"
    );
    actionCollection()->action("autoRefreshView")->setWhatsThis(i18n("This will reload the thumbnail window automatically after, for example, changing the tag filter or the source directory to be shown."));
    applyAutorefreshSetting();

    KShortcut refreshViewShortCut(KStdAccel::shortcut(KStdAccel::Reload));
    refreshViewShortCut.append(KKey("CTRL+r"));
    new KAction(
        i18n("&Refresh view"), Constants::ICON_REFRESH_VIEW,
        refreshViewShortCut,
        this, SLOT(slotRefreshView()),
        actionCollection(), "refreshView"
    );
    actionCollection()->action("refreshView")->setWhatsThis(i18n("This may, for example, be needed after having changed the tag filter or the source directory to be shown."));

    m_zoomIn = new KAction(
        i18n("&Increase Previewsize"), Constants::ICON_INCREASE_PREVIEWSIZE,
        KStdAccel::shortcut(KStdAccel::ZoomIn),
        this, SLOT(slotIncreasePreviewSize()),
        actionCollection(), "increasePreviewSize"
    );
    actionCollection()->action("increasePreviewSize")->setWhatsThis(i18n("Make the preview size in the thumbnail window bigger."));// Click and hold down the mouse button for a menu with a set of available preview sizes."));
    
    m_zoomOut = new KAction(
        i18n("&Decrease Previewsize"), Constants::ICON_DECREASE_PREVIEWSIZE,
        KStdAccel::shortcut(KStdAccel::ZoomOut),
        this, SLOT(slotDecreasePreviewSize()),
        actionCollection(), "decreasePreviewSize"
    );
    actionCollection()->action("decreasePreviewSize")->setWhatsThis(i18n("Make the preview size in the thumbnail window smaller."));// Click and hold down the mouse button for a menu with a set of available preview sizes."));
    applyZoomSetting();

    
    //
    // sourcedir actions
    //
    new KAction(
        i18n("Add &sourcedirectory"), Constants::ICON_ADD_SOURCEDIR,
        0,
        this, SLOT(slotAddSourcedir()),
        actionCollection(), "addSourceDir"
    );
    actionCollection()->action("addSourceDir")->setWhatsThis(i18n("Add a source directory to the database. This can happen to a single directory or recursively. "));
    
    KShortcut editSourceDirShortCut(KKey("F3"));
    new KAction(
        i18n("&Edit sourcedirectory"), Constants::ICON_EDIT_SOURCEDIR,
        editSourceDirShortCut,
        this, SLOT(slotEditSourceDir()),
        actionCollection(), "editSourceDir"
    );
    actionCollection()->action("editSourceDir")->setEnabled(false);
    actionCollection()->action("editSourceDir")->setWhatsThis(i18n("Edit the properties of the sourcedirectory. I.e. change the location of the directory."));    
    
    new KAction(
        i18n("&Remove sourcedirectory"), Constants::ICON_REMOVE_SOURCEDIR,
        0,
        this, SLOT(slotRemoveSourceDir()),
        actionCollection(), "removeSourceDir"
    );
    actionCollection()->action("removeSourceDir")->setWhatsThis(i18n("Remove the selected source directory with all its sub directories. The files which are in this sub directory will be removed from the database - losing the tag associatioins."));

    new KAction(
        i18n("&Include whole folder"), Constants::ICON_INCLUDE_WHOLE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotIncludeWholeSourceDir()),
        actionCollection(), "includeWholeSourceDir"
    );
    actionCollection()->action("includeWholeSourceDir")->setWhatsThis(i18n("Include the current directory and all sub directories to the thumbnail preview."));
    
    new KAction(
        i18n("&Exclude whole folder"), Constants::ICON_EXCLUDE_WHOLE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExcludeWholeSourceDir()),
        actionCollection(), "excludeWholeSourceDir"
    );
    actionCollection()->action("excludeWholeSourceDir")->setWhatsThis(i18n("Exclude the current directory and all sub directories from the thumbnail preview."));
    
    new KAction(
        i18n("In&vert folder selection"), Constants::ICON_INVERT_FOLDER_SELECTION,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotInvertSourceDir()),
        actionCollection(), "invertSourceDir"
    );
    actionCollection()->action("invertSourceDir")->setWhatsThis(i18n("Exclude the included directories and include the excluded directories of the current folder to the thumbnail view."));

    new KAction(
        i18n("&Include all"), Constants::ICON_INCLUDE_WHOLE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotIncludeAllSourceDirs()),
        actionCollection(), "includeAllSourceDirs"
    );
    actionCollection()->action("includeAllSourceDirs")->setWhatsThis(i18n("Include all source directories with all sub directories to the thumbnail view."));
    
    new KAction(
        i18n("&Exclude all"), Constants::ICON_EXCLUDE_WHOLE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExcludeAllSourceDirs()),
        actionCollection(), "excludeAllSourceDirs"
    );
    actionCollection()->action("excludeAllSourceDirs")->setWhatsThis(i18n("Exclued all source directories with all sub directories from the thumbnail view."));
    
    new KAction(
        i18n("In&vert all"), Constants::ICON_INVERT_FOLDER_SELECTION,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotInvertAllSourceDirs()),
        actionCollection(), "invertAllSourceDirs"
    );
    actionCollection()->action("invertAllSourceDirs")->setWhatsThis(i18n("Exclude the included directories and include the excluded directories to the thumbnail view."));

    new KAction(
        i18n("Expand sourcedir"), Constants::ICON_EXPAND_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExpandSourceDir()),
        actionCollection(), "expandSourceDir"
    );
    actionCollection()->action("expandSourceDir")->setWhatsThis(i18n("Expand all children of the source directory."));
        
    new KAction(
        i18n("Collapse sourcedir"), Constants::ICON_COLLAPSE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotCollapseSourceDir()),
        actionCollection(), "collapseSourceDir"
    );
    actionCollection()->action("collapseSourceDir")->setWhatsThis(i18n("Hide the subtree of the source directory."));
    
    new KAction(
        i18n("Expand all sourcedirs"), Constants::ICON_EXPAND_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExpandAllSourceDirs()),
        actionCollection(), "expandAllSourceDirs"
    );
    actionCollection()->action("expandAllSourceDirs")->setWhatsThis(i18n("Expand the the whole source directory tree."));
    
    new KAction(
        i18n("Collapse all sourcedirs"), Constants::ICON_COLLAPSE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotCollapseAllSourceDirs()),
        actionCollection(), "collapseAllSourceDirs"
    );
    actionCollection()->action("collapseAllSourceDirs")->setWhatsThis(i18n("Collapse the whole source directory tree."));

    
    //
    // tag actions
    //
    new KAction(
        i18n("Add &maintag"), Constants::ICON_CREATE_MAINTAG,
        0,
        this, SLOT(slotAddMaintag()),
        actionCollection(), "addMaintag"
    );
    actionCollection()->action("addMaintag")->setWhatsThis(i18n("Add a new main tag to the database. This is a top level tag which can contain sub-tags."));
    
    new KAction(
        i18n("&Create subtag"), Constants::ICON_CREATE_SUBTAG,
        0,
        this, SLOT(slotCreateSubtag()),
        actionCollection(), "createSubtag"
    );
    actionCollection()->action("createSubtag")->setWhatsThis(i18n("Creates a tag as child of the selected tag."));
    
    KShortcut editTagShortCut(KKey("F2"));
    new KAction(
        i18n("&Edit tag"), Constants::ICON_EDIT_TAG,
        editTagShortCut,
        this, SLOT(slotEditTag()),
        actionCollection(), "editTag"
    );
    actionCollection()->action("editTag")->setWhatsThis(i18n("Change the attributes of this tag."));

    new KAction(
        i18n("&Delete tag"), Constants::ICON_DELETE_TAG,
        0,
        this, SLOT(slotDeleteTag()),
        actionCollection(), "deleteTag"
    );
    actionCollection()->action("deleteTag")->setWhatsThis(i18n("Deletes the tag after accepting a confirmation dialog."));
    
    m_lockUnlockTaggingAction = new KToggleAction(
        i18n("Lock Tagging"), 0,
        0,
        this, SLOT(slotToggleLockUnlockTagging()),
        actionCollection(), "toggleLockUnlockTagging"
    );
    actionCollection()->action("toggleLockUnlockTagging")->setWhatsThis(i18n("If locking is enabled it is not possible to change the tags of an image. The lock is also disabled while the CTRL-key is pressed."));
    applyLockUnlockTaggingSettings();

    m_andifyTagsAction = new KToggleAction(
        i18n("Tag filter operator = AND"), Constants::ICON_OPERATOR_AND,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotAndifyTags()),
        actionCollection(), "andifyTags"
    );
    actionCollection()->action("andifyTags")->setWhatsThis(i18n("This sets the filter operator to 'AND', which means that only images which contain all in the filter marked tags will be shown."));
    
    m_orifyTagsAction = new KToggleAction(
        i18n("Tag filter operator = OR"), Constants::ICON_OPERATOR_OR,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotOrifyTags()),
        actionCollection(), "orifyTags"
    );
    actionCollection()->action("orifyTags")->setWhatsThis(i18n("This sets the filter operator to 'OR', which means that pictures with any in the filter marked tags will be shown."));
    applyOperatorSetting();

    new KAction(
        i18n("Set filter to show all untagged images"), Constants::ICON_TAG_FILTER_DESELECT,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotDeselectFilter()),
        actionCollection(), "deselectFilter"
    );
    actionCollection()->action("deselectFilter")->setWhatsThis(i18n("Deselects all filters. In the filters 'AND' mode, all images without a associated tag are shown only. This is useful for finding new added images without having a tag yet."));
    
    new KAction(
        i18n("Reset Filter"), Constants::ICON_TAG_FILTER_RESET,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotResetFilter()),
        actionCollection(), "resetFilter"
    );
    actionCollection()->action("resetFilter")->setWhatsThis(i18n("Sets the filter that every image is shown."));

    new KAction(
        i18n("Expand tag"), Constants::ICON_EXPAND_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExpandTag()),
        actionCollection(), "expandTag"
    );
    actionCollection()->action("expandTag")->setWhatsThis(i18n("Expand all children of the tag."));
    
    new KAction(
        i18n("Collapse tag"), Constants::ICON_COLLAPSE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotCollapseTag()),
        actionCollection(), "collapseTag"
    );
    actionCollection()->action("collapseTag")->setWhatsThis(i18n("Hide the subtree of the tag."));
    
    new KAction(
        i18n("Expand all tags"), Constants::ICON_EXPAND_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExpandAllTags()),
        actionCollection(), "expandAllTags"
    );
    actionCollection()->action("expandAllTags")->setWhatsThis(i18n("Expand the whole tag tree."));
    
    new KAction(
        i18n("Collapse all tags"), Constants::ICON_COLLAPSE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),m_sourcedirTree
        this, SLOT(slotCollapseAllTags()),
        actionCollection(), "collapseAllTags"
    );
    actionCollection()->action("collapseAllTags")->setWhatsThis(i18n("Collapse the whole tag tree."));
    
    
    //
    // tool view actions
    //
    new KAction(
        i18n("Restore toolviews"), Constants::ICON_RESTORE_TOOL_VIEWS,
        0,
        this, SLOT(slotRestoreToolViews()),
        actionCollection(), "restoreToolViews"
    );
    actionCollection()->action("restoreToolViews")->setWhatsThis(i18n("Rearrange the toolviews (Tagtree, ...) to their default position."));
    
    new KAction(
        i18n("Show Tagtree"), 0,
        0,
        this, SLOT(slotShowToolViewTagTree()),
        actionCollection(), "showToolViewTagTree"
    );
    actionCollection()->action("showToolViewTagTree")->setWhatsThis(i18n("Display the Tagtree toolview."));

    new KAction(
        i18n("Show Sourcedirtree"), 0,
        0,
        this, SLOT(slotShowToolViewSourceDirTree()),
        actionCollection(), "showToolViewSourceDirTree"
    );
    actionCollection()->action("showToolViewSourceDirTree")->setWhatsThis(i18n("Display the Sourcedirectory toolview."));

    new KAction(
        i18n("Show EXIF"), 0,
        0,
        this, SLOT(slotShowToolViewMetaInfoTree()),
        actionCollection(), "showToolViewMetaInfoTree"
    );    
    actionCollection()->action("showToolViewMetaInfoTree")->setWhatsThis(i18n("Display the EXIF toolview."));
}


void KPhotoBook::setupContextMenus() {

    m_contextMenuSourceDirTree = static_cast<KPopupMenu*>(guiFactory()->container("contextMenu_sourceDirTree", this));
    m_contextMenuSourceDir = static_cast<KPopupMenu*>(guiFactory()->container("contextMenu_sourceDir", this));
    m_contextMenuSubDir = static_cast<KPopupMenu*>(guiFactory()->container("contextMenu_subDir", this));

    m_contextMenuTagTree = static_cast<KPopupMenu*>(guiFactory()->container("contextMenu_tagTree", this));
    m_contextMenuTagTreeItem = static_cast<KPopupMenu*>(guiFactory()->container("contextMenu_tagTreeItem", this));
}


QPtrList<File>* KPhotoBook::files(QString filter) {

    QString op = "&";
    if (Settings::tagTreeFilterOperator() == Settings::EnumTagTreeFilterOperator::Or) {
        op = "|";
    }

    // build the filter from the tagtree if the specified filter is empty
    if (filter.isNull() && m_view) {
        TagTreeNode* lastItem = 0;
        QListViewItemIterator it(m_tagTree);
        while (it.current()) {
            TagTreeNode* item = dynamic_cast<TagTreeNode*>(it.current());

            if (!item->filter().isNull()) {
                if (lastItem) {
                    filter.append(op);
                }
                filter.append(item->filter());
                lastItem = item;
            }

            ++it;
        }
    }

    // get the files matching the filter
    QPtrList<File>* files = m_engine->files(filter);

    updateState();
    return files;
}


//
// protected
//
bool KPhotoBook::queryClose() {

    kdDebug() << "[KPhotoBook::queryClose] invoked" << endl;

    // store the configuration
    m_view->storeConfiguration();

    // we have to store the properties which aren't handled magically by framework (stringlist)
    QStringList stringList;
    if (m_settingsFileHandling) {
        for (uint i = 0; i < m_settingsFileHandling->kcfg_FileFilterFileToHandle->count(); i++) {
            stringList.append(m_settingsFileHandling->kcfg_FileFilterFileToHandle->text(i));
        }
        Settings::setFileFilterFileToHandle(stringList);

        stringList.clear();
        for (uint i = 0; i < m_settingsFileHandling->kcfg_FileFilterSubdirsToIgnore->count(); i++) {
            stringList.append(m_settingsFileHandling->kcfg_FileFilterSubdirsToIgnore->text(i));
        }
        Settings::setFileFilterSubdirsToIgnore(stringList);
    }
    
    if (m_settingsTools) {
        stringList.clear();
        for (uint i = 0; i < m_settingsTools->kcfg_ToolsExternalTools->count(); i++) {
            stringList.append(m_settingsTools->kcfg_ToolsExternalTools->text(i));
        }
        Settings::setToolsExternalTools(stringList);
    }
    
    switch (mdiMode()) {
        case KMdi::TabPageMode:
            Settings::setGeneralViewMode(Settings::EnumGeneralViewMode::TabPageMode);
            break;
        default:
            Settings::setGeneralViewMode(Settings::EnumGeneralViewMode::IDEAlMode);
            break;
    }
    
    // force writing the settings
    Settings::writeConfig();
    
    // store dock configuration
    writeDockConfig(KGlobal::config(), "DockConfig");

    // store the data if necessary
    if (m_engine && m_engine->dirty()) {

        // show save, discard, abort dialog
        QString fileName = QString("");
        if (m_engine->uid()) {
            fileName = QString("%1\n").arg(m_engine->fileinfo()->absFilePath());
        }

        QString text = QString(i18n("The document has been modified.\n%1Do you want to save it?")).arg(fileName);

        int button = KMessageBox::warningYesNoCancel(
                         this, // parent
                         text, // text
                         i18n("Save Document"), // caption
                         (m_engine->uid() ? KStdGuiItem::save() : KStdGuiItem::saveAs()),   // buttonYes
                         KStdGuiItem::discard()  // buttonNo
                     );

        // analyze the clicked button
        switch (button) {
        case KMessageBox::Yes :
            return slotFileSave();
            break;

        case KMessageBox::No :
            // just close the application
            return true;
            break;

        case KMessageBox::Cancel :
            // abort closing if cancel is clicked
            return false;
            break;
        }
    }

    // store the tree states
    if (m_engine->uid()) {
      storeTreeState();
      storeFilter();
    }
    
    // if we got here there was nothing to save --> simply close
    return true;
}


bool KPhotoBook::queryExit() {

    kdDebug() << "[KPhotoBook::queryExit] invoked..." << endl;

//    saveMainWindowSettings(kapp->config());

    return true;
}


//
// private slots
//
void KPhotoBook::slotFileNew() {

    // this slot is called whenever the File->New menu is selected,
    // the New shortcut is pressed (usually CTRL+N) or the New toolbar
    // button is clicked

    // create a new window
    (new KPhotoBook())->show();
}


void KPhotoBook::slotFileOpen() {

    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked

    // store the data if necessary
    if (m_engine && m_engine->dirty()) {
        // show save, discard, abort dialog
        QString fileName = QString("");
        if (m_engine->uid()) {
            fileName = QString("%1\n").arg(m_engine->fileinfo()->absFilePath());
        }

        QString text = QString(i18n("The document has been modified.\n%1Do you want to save it?")).arg(fileName);

        int button = KMessageBox::warningYesNoCancel(
                         this, // parent
                         text, // text
                         i18n("Save Document"), // caption
                         (m_engine->uid() ? KStdGuiItem::save() : KStdGuiItem::saveAs()),   // buttonYes
                         KStdGuiItem::discard()  // buttonNo
                     );

        // analyze the clicked button
        switch (button) {
        case KMessageBox::Yes :
            // we do not open another file, if we cannot save the current file
            if (!slotFileSave()) {
                return;
            }
            break;

        case KMessageBox::No :
            // we do not save the changes --> just open the new file
            break;

        case KMessageBox::Cancel :
            // abort opening if cancel is clicked
            return;
        }
    }

    // try to get last opened file
    QString lastFileName = Settings::fileSystemLastOpenedFile();

    QString lastDirName("");
    if (!lastFileName.isEmpty()) {
        lastDirName = QFileInfo(lastFileName).dirPath(true);
    }


    // standard filedialog
    QString fileFilter = QString("*.%1").arg(Constants::FILE_EXTENSION);
    QString fileName = KFileDialog::getOpenFileName(lastDirName, fileFilter, this, i18n("Open Location"));
    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);

        if (fileInfo.exists()) {
            load(fileInfo);
        }
    }

    updateState();
}


bool KPhotoBook::slotFileSave() {

    // this slot is called whenever the File->Save menu is selected,
    // the Save shortcut is pressed (usually CTRL+S) or the Save toolbar
    // button is clicked

    bool fileSaved = false;

    // save the current file
    if (m_engine) {
        if (m_engine->alreadySavedOnce()) {
            try {
                m_engine->save();
                fileSaved = true;
            } catch(PersistingException* ex) {
                KMessageBox::detailedError(m_view, ex->message(), ex->detailMessage(), i18n("Saving failed"));
                // TODO
                // it's very strange, but the application crashes if I delete the exception!!!
//                delete ex;
            }
        } else {
            return slotFileSaveAs();
        }
    }

    updateState();
    return fileSaved;
}


bool KPhotoBook::slotFileSaveAs() {

    // this slot is called whenever the File->Save As menu is selected,

    bool fileSaved = false;

    QString startDir = QString::null;
    // determine directory of current opened file
    if (m_engine->alreadySavedOnce()) {
        QFileInfo currentFile = *m_engine->fileinfo();
        startDir = currentFile.dirPath(true);
    }

    // show the savedialog
    QString fileFilter = QString("*.%1").arg(Constants::FILE_EXTENSION);
    QString newFileName = KFileDialog::getSaveFileName(startDir, fileFilter);

    if (!newFileName.isEmpty()) {

        QFileInfo newFile = QFileInfo(newFileName);
        QString msg = QString(i18n("The chosen file already exists:\n%1\nDo you want to overwrite it?")).arg(newFile.absFilePath());
        if (!newFile.exists() || (KMessageBox::questionYesNo(m_view, msg, i18n("Overwrite file?")) == KMessageBox::Yes)) {

            // save your info, here
            try {
                m_engine->saveAs(newFile);
                Settings::setFileSystemLastOpenedFile(currentURL());
                fileSaved = true;
            } catch(PersistingException* ex) {
                KMessageBox::detailedError(m_view, ex->message(), ex->detailMessage(), i18n("Saving failed"));
                // TODO
                // it's very strange, but the application crashes if I delete the exception!!!
//                delete ex;
            }
        }
    }

    updateState();
    return fileSaved;
}


void KPhotoBook::slotOptionsConfigureKeys() {

    KKeyDialog::configure(actionCollection(), "kphotobookui.rc");
}


void KPhotoBook::slotOptionsConfigureToolbars() {

    // use the standard toolbar editor
    KEditToolbar dlg(factory());
    if (dlg.exec()) {
        applyMainWindowSettings( KGlobal::config(), "MainWindow" );
    }
}


void KPhotoBook::slotOptionsPreferences() {

    // try to show the same settings dialog as last time
    if(KConfigDialog::showDialog("settings")) {
        return;
    }

    // first time --> create the settings dialog
    KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self(), KDialogBase::IconList);

    m_settingsGeneral = new SettingsGeneral(0, "SettingsGeneral");
    dialog->addPage(m_settingsGeneral, i18n("General"), Constants::ICON_SETTINGS_GENERAL);

    m_settingsTagTree = new SettingsTagTree(0, "SettingsTagtree");
    dialog->addPage(m_settingsTagTree, i18n("TagTree"), Constants::ICON_SETTINGS_TAG);

    m_settingsSourceDirTree = new SettingsSourceDirTree(0, "SettingsSourceDirTree");
    dialog->addPage(m_settingsSourceDirTree, i18n("SourceDirTree"), Constants::ICON_SETTINGS_SOURCEDIR);

    m_settingsImagePreview = new SettingsImagePreview(0, "SettingsImagePreview");
    dialog->addPage(m_settingsImagePreview, i18n("ImagePreview"), Constants::ICON_SETTINGS_IMAGEPREVIEW);

    m_settingsFileHandling = new SettingsFileHandling(0, "SettingsFileHandling");
    m_settingsFileHandling->kcfg_FileFilterFileToHandle->insertStringList(Settings::fileFilterFileToHandle());
    m_settingsFileHandling->kcfg_FileFilterFileToHandle->setSelected(0, true);
    m_settingsFileHandling->kcfg_FileFilterSubdirsToIgnore->insertStringList(Settings::fileFilterSubdirsToIgnore());
    m_settingsFileHandling->kcfg_FileFilterSubdirsToIgnore->setSelected(0, true);
    dialog->addPage(m_settingsFileHandling, i18n("FileHandling"), Constants::ICON_SETTINGS_FILEHANDLING);

    m_settingsTools = new SettingsTools(0, "SettingsTools");
    m_settingsTools->kcfg_ToolsExternalTools->insertStringList(Settings::toolsExternalTools());
    m_settingsTools->kcfg_ToolsExternalTools->setSelected(0, true);
    dialog->addPage(m_settingsTools, i18n("Tools"), Constants::ICON_SETTINGS_TOOLS);

    connect(dialog, SIGNAL(settingsChanged()), this, SLOT(slotLoadSettings()));
    connect(dialog, SIGNAL(settingsChanged()), m_tagTree, SLOT(slotLoadSettings()));
    connect(dialog, SIGNAL(settingsChanged()), m_sourcedirTree, SLOT(slotLoadSettings()));
    connect(dialog, SIGNAL(settingsChanged()), m_view, SLOT(slotLoadSettings()));

    connect(dialog, SIGNAL(defaultClicked()), this, SLOT(slotConfigDefaultClicked()));

    dialog->show();
}

void KPhotoBook::slotAddSourcedir() {

    kdDebug() << "[KPhotoBook::slotAddSourcedir] called... " << endl;

    DialogAddSourceDir* dialog = new DialogAddSourceDir(m_view, "DialogAddSourceDir");

    bool newDirIsOk = false;
    while (!newDirIsOk && dialog->exec()) {

        kdDebug() << "[KPhotoBook::slotAddSourcedir] dialog exited with OK, dir: " << dialog->directory()->absPath() << ", recursive: " << dialog->recursive() << endl;

        try {
            // add the sourcedir to the engine
            SourceDir* sourceDir = addSourceDir(dialog->directory(), dialog->recursive());

            kdDebug() << "[KPhotoBook::slotAddSourcedir] new sourcedirectory is ok. adding it to the view..." << endl;
            m_sourcedirTree->addSourceDir(sourceDir);

            // update the view to display the new found files
            kdDebug() << "[KPhotoBook::slotAddSourcedir] updating fileview" << endl;
            m_view->updateFiles();

            // sourcedir added successfully
            newDirIsOk = true;
        } catch(EngineException* ex) {
            kdDebug() << "[KPhotoBook::slotAddSourcedir] adding choosen sourcedir failed, dir: " << dialog->directory()->absPath() << ", recursive: " << dialog->recursive() << endl;

            KMessageBox::detailedError(dialog, ex->message(), ex->detailMessage(), i18n("Adding sourcedir failed"));

            // TODO
            // it's very strange, but the application crashes if I delete the exception!!!
            //delete ex;
        }
    }
    delete dialog;
}


void KPhotoBook::slotEditSourceDir() {

    kdDebug() << "[KPhotoBook::slotEditSourceDir] called... " << endl;
    kdDebug() << "NOT IMPLEMENTED YET" << endl;

    // TODO: implement SourceDirPopupMenu::editSourceDir()
}


void KPhotoBook::slotRemoveSourceDir() {

    kdDebug() << "[KPhotoBook::slotRemoveSourceDir] called... " << endl;

    // get the sourcedir to remove from the tagtree
    SourceDirTreeNode* currentNode = m_sourcedirTree->selectedSourceDir();

    // show a dialog to the user
    QString msg = QString(i18n("Do you want to remove the source directory?\n%1")).arg(currentNode->sourceDir()->dir()->absPath());
    int button = KMessageBox::questionYesNo(
        m_view,
        msg,
        i18n("Removing source directory?")
    );

    if (button == KMessageBox::Yes) {
        // remove all items from the view without deleting them
        m_view->removeAllFiles();

        // force clearing the view (to avoid a crash of the application)
        m_view->fileView()->clearView();

        // remove the sourcedir from the engine
        removeSourceDir(currentNode->sourceDir());

        kdDebug() << "[KPhotoBook::slotRemoveSourceDir] sourcedirs removed from engine... " << endl;

        // remove the sourcedir from the view
        m_sourcedirTree->removeSourceDir(currentNode);

        kdDebug() << "[KPhotoBook::slotRemoveSourceDir] sourcedirnode from sourcedirtree removed... " << endl;

        // update the shown files
        m_view->updateFiles();
    }

    kdDebug() << "[KPhotoBook::slotRemoveSourceDir] ended... " << endl;
}


void KPhotoBook::slotAddMaintag() {

    kdDebug() << "[KPhotoBook::slotRemoveSourceDir] called... " << endl;

    DialogCreateTag* dialog = new DialogCreateTag(m_view, 0, this, "DialogCreateTag");
    if (dialog->exec()) {

        kdDebug() << "[KPhotoBook::slotRemoveSourceDir] dialog exited with OK, type: " << dialog->tagType() << ", name: " << dialog->tagName() << ", icon: " << dialog->tagIcon() << endl;

        // let the engine create the new tagnode
        TagNode* newTagNode = createTag(dialog->tagType(), dialog->tagName(), dialog->tagIcon());

        // add the new tagnode to the tagnodetree
        m_tagTree->addTagNode(newTagNode);
    }
    delete dialog;
}


void KPhotoBook::slotCreateSubtag() {

    kdDebug() << "[KPhotoBook::slotCreateSubtag] called... " << endl;

    // get the tag to add a child to
    QListViewItem* currentItem = m_tagTree->currentItem();
    if (typeid(*currentItem) != typeid(TagTreeNodeTitle)
        && typeid(*currentItem) != typeid(TagTreeNodeBoolean)) {
        kdDebug() << "[KPhotoBook::slotCreateSubtag] called on a tree item other than TagTreeNode!" << endl;
        return;
    }
    TagTreeNode* parent = dynamic_cast<TagTreeNode*>(currentItem);

    DialogCreateTag* dialog = new DialogCreateTag(m_view, parent, this, "DialogCreateTag");
    if (dialog->exec()) {

        kdDebug() << "[KPhotoBook::slotCreateSubtag] dialog exited with OK, type: " << dialog->tagType() << ", name: " << dialog->tagName() << ", icon: " << dialog->tagIcon() << endl;

        // let the engine create the new tagnode
        TagNode* newTagNode = createTag(dialog->tagType(), dialog->tagName(), dialog->tagIcon(), parent->tagNode());

        // add the new tagnode to the tagnodetree
        m_tagTree->addTagNode(parent, newTagNode);
        parent->setOpen(true);
    }
    delete dialog;
}


void KPhotoBook::slotEditTag() {

    kdDebug() << "[KPhotoBook::slotEditTag] called... " << endl;

    // get the tag to add a child to
    QListViewItem* currentItem = m_tagTree->currentItem();
    if (typeid(*currentItem) != typeid(TagTreeNodeTitle)
        && typeid(*currentItem) != typeid(TagTreeNodeBoolean)) {
        kdDebug() << "[KPhotoBook::slotCreateSubtag] called on a tree item other than TagTreeNode!" << endl;
        return;
    }
    TagTreeNode* tagTreeNode = dynamic_cast<TagTreeNode*>(currentItem);

    DialogEditTag* dialog = new DialogEditTag(m_view, tagTreeNode, this, "DialogEditTag");
    if (dialog->exec()) {

        kdDebug() << "[KPhotoBook::slotEditTag] dialog exited with OK, newname: " << dialog->tagName() << ", newicon: " << dialog->tagIcon() << endl;

        // update the tagnode and the tagtreenode
        m_engine->editTag(tagTreeNode->tagNode(), dialog->tagName(), dialog->tagIcon());
        tagTreeNode->refresh();

    }
    delete dialog;

    updateState();
}


void KPhotoBook::slotDeleteTag() {

    kdDebug() << "[KPhotoBook::slotDeleteTag] called... " << endl;

    // get the tag to add a child to
    QListViewItem* currentItem = m_tagTree->currentItem();
    if (typeid(*currentItem) != typeid(TagTreeNodeTitle)
        && typeid(*currentItem) != typeid(TagTreeNodeBoolean)) {
        kdDebug() << "[KPhotoBook::slotDeleteTag] called on a tree item other than TagTreeNode!" << endl;
        return;
    }
    TagTreeNode* tagTreeNode = dynamic_cast<TagTreeNode*>(currentItem);

    // show a dialog, the user must confirm
    QString msg = QString(i18n("Do you want to delete the tag named '%1'?\n")).arg(*(tagTreeNode->tagNode()->text()));
    msg.append(i18n("All associations between files and this tag will be deleted!"));
    int button = KMessageBox::questionYesNo(
        m_view,
        msg,
        i18n("Deleting tag?")
    );

    if (button == KMessageBox::Yes) {
        // remove the tag from the engine
        m_engine->removeTag(tagTreeNode->tagNode());

        // remove the tag from the view
        m_view->removeTagNode(tagTreeNode);

        // update the shown files
        m_view->updateFiles();
    }

    updateState();
}


void KPhotoBook::slotToggleLockUnlockTagging() {

    kdDebug() << "[KPhotoBook::slotToggleLockUnlockTagging] called... " << endl;
    
    Settings::setTagTreeLocked(!Settings::tagTreeLocked());
    
    applyLockUnlockTaggingSettings();
}


void KPhotoBook::slotRescanFilesystem() {

    kdDebug() << "[KPhotoBook::slotRescanFilesystem] called... " << endl;

    m_engine->rescanSourceDirs();

    // add the sourcedirectories to the tagtree
    m_sourcedirTree->clear();
    m_sourcedirTree->addSourceDirs(m_engine->sourceDirs());

    // add the files to the view
    m_view->updateFiles();

    updateState();
}


void KPhotoBook::slotAutoRefreshView() {
    Settings::setImagePreviewAutoRefresh(!Settings::imagePreviewAutoRefresh());

    applyAutorefreshSetting();
}
void KPhotoBook::slotRefreshView() {
    m_view->updateFiles();
}

void KPhotoBook::slotIncreasePreviewSize() {
    Settings::setImagePreviewSize(Settings::imagePreviewSize() + 8);

    applyZoomSetting();

    m_view->updateCurrentImageSize();
}
void KPhotoBook::slotDecreasePreviewSize() {
    Settings::setImagePreviewSize(Settings::imagePreviewSize() - 8);

    applyZoomSetting();

    m_view->updateCurrentImageSize();
}


void KPhotoBook::slotIncludeWholeSourceDir() {
    m_sourcedirTree->includeWholeSourceDir();

    autoRefreshView();
}
void KPhotoBook::slotExcludeWholeSourceDir() {
    m_sourcedirTree->excludeWholeSourceDir();

    autoRefreshView();
}
void KPhotoBook::slotInvertSourceDir() {
    m_sourcedirTree->invertSourceDir();

    autoRefreshView();
}
void KPhotoBook::slotIncludeAllSourceDirs() {
    m_sourcedirTree->includeAllSourceDirs();

    autoRefreshView();
}
void KPhotoBook::slotExcludeAllSourceDirs() {
    m_sourcedirTree->excludeAllSourceDirs();

    autoRefreshView();
}
void KPhotoBook::slotInvertAllSourceDirs() {
    m_sourcedirTree->invertAllSourceDirs();

    autoRefreshView();
}


void KPhotoBook::slotExpandSourceDir() {
    m_sourcedirTree->expandCurrent();
}
void KPhotoBook::slotCollapseSourceDir() {
    m_sourcedirTree->collapseCurrent();
}
void KPhotoBook::slotExpandAllSourceDirs() {
    m_sourcedirTree->expandAll();
}
void KPhotoBook::slotCollapseAllSourceDirs() {
    m_sourcedirTree->collapseAll();
}


void KPhotoBook::slotAndifyTags() {

    int lastOperator = Settings::tagTreeFilterOperator();

    Settings::setTagTreeFilterOperator(Settings::EnumTagTreeFilterOperator::And);
    applyOperatorSetting();

    if (lastOperator != Settings::EnumTagTreeFilterOperator::And) {
        autoRefreshView();
    }
}

void KPhotoBook::slotOrifyTags() {

    int lastOperator = Settings::tagTreeFilterOperator();

    Settings::setTagTreeFilterOperator(Settings::EnumTagTreeFilterOperator::Or);
    applyOperatorSetting();

    if (lastOperator != Settings::EnumTagTreeFilterOperator::Or) {
        autoRefreshView();
    }
}


void KPhotoBook::slotDeselectFilter() {
    m_tagTree->deselectFilter();

    autoRefreshView();
}

void KPhotoBook::slotResetFilter() {
    m_tagTree->resetFilter();

    autoRefreshView();
}


void KPhotoBook::slotExpandTag() {
    m_tagTree->expandCurrent();
}
void KPhotoBook::slotCollapseTag() {
    m_tagTree->collapseCurrent();
}
void KPhotoBook::slotExpandAllTags() {
    m_tagTree->expandAll();
}
void KPhotoBook::slotCollapseAllTags() {
    m_tagTree->collapseAll();
}


void KPhotoBook::slotFileSelectionChanged() {

    unsigned int selectedImagesCount = m_view->fileView()->selectedItems()->count();

    // update the statusbar to reflect the number of selected files
    QString selectedMsg = QString(i18n("Selected: %1")).arg(selectedImagesCount);
    statusBar()->changeItem(selectedMsg, 4);
    
    // enable 'exportSelectedFiles' only if at least one file is selected
    m_exportSelectedFiles->setEnabled(selectedImagesCount > 0);

    // update the sourcedirtree
    m_sourcedirTree->reflectSelectedFiles(m_view->fileView()->selectedItems());

    // update the tagtree
    m_tagTree->doRepaintAll();

    // remove everything from the metainfo tree
    m_metaInfoTree->clear();

    // show EXIF info if only one image is seleced
    if (m_view->fileView()->selectedItems()->count() == 1) {
        kdDebug() << "one file is selected --> getting meta infos" << endl;
        
        QPtrListIterator<KFileItem> tempIt(*m_view->fileView()->selectedItems());
        KFileItem* selectedFile = tempIt.current();
        
        KFileMetaInfo metaInfo = selectedFile->metaInfo();

        // iterate over groups
        QStringList groups = metaInfo.groups();
        KListViewItem* currentGroup = 0;
        for ( QStringList::Iterator groupIt = groups.begin(); groupIt != groups.end(); ++groupIt ) {
            kdDebug() << "Handling metainfo group: " << *groupIt << endl;
            currentGroup = new KListViewItem(m_metaInfoTree, *groupIt);
            currentGroup->setOpen(true);
            KFileMetaInfoGroup group = metaInfo.group(*groupIt);

            QStringList keys = group.keys();

            // iterate over keys
            for ( QStringList::Iterator keysIt = keys.begin(); keysIt != keys.end(); ++keysIt ) {
                kdDebug() << "Handling metainfo key: " << *keysIt << endl;
                KFileMetaInfoItem item = group.item(*keysIt);
                QString value = item.string();

                new KListViewItem(currentGroup, *keysIt, value);
            }
        }
    }
}


void KPhotoBook::slotLoadSettings() {

    applyOperatorSetting();
    applyZoomSetting();
    applyAutorefreshSetting();

    m_tagTreeToolBar->setIconSize(Settings::tagTreeToolBarIconSize());
    m_sourceDirTreeToolBar->setIconSize(Settings::sourceDirTreeToolBarIconSize());

    KMdi::MdiMode newMdiMode = KMdi::IDEAlMode;
    if (Settings::generalViewMode() == Settings::EnumGeneralViewMode::TabPageMode) {
        newMdiMode = KMdi::TabPageMode;
    }
    if (newMdiMode != mdiMode()) {
        switch (newMdiMode) {
            case KMdi::TabPageMode:
                switchToTabPageMode();
                break;
            default:
                switchToIDEAlMode();
                break;
        }
    }
}


void KPhotoBook::slotConfigDefaultClicked() {

    Settings* defaultSettings = Settings::self();
    defaultSettings->useDefaults(true);

    m_settingsFileHandling->kcfg_FileFilterFileToHandle->clear();
    m_settingsFileHandling->kcfg_FileFilterFileToHandle->insertStringList(Settings::fileFilterFileToHandle());
    m_settingsFileHandling->kcfg_FileFilterFileToHandle->setSelected(0, true);

    m_settingsFileHandling->kcfg_FileFilterSubdirsToIgnore->clear();
    m_settingsFileHandling->kcfg_FileFilterSubdirsToIgnore->insertStringList(Settings::fileFilterSubdirsToIgnore());
    m_settingsFileHandling->kcfg_FileFilterSubdirsToIgnore->setSelected(0, true);

    m_settingsTools->kcfg_ToolsExternalTools->clear();
    m_settingsTools->kcfg_ToolsExternalTools->insertStringList(Settings::toolsExternalTools());
    m_settingsTools->kcfg_ToolsExternalTools->setSelected(0, true);

    defaultSettings->useDefaults(false);
}


void KPhotoBook::slotRestoreToolViews() {

    m_tagTreeToolView->place(KDockWidget::DockLeft, getMainDockWidget(), 20);
    m_sourceDirTreeToolView->place(KDockWidget::DockCenter, m_tagTree, 20);
    m_metaInfoTreeToolView->place(KDockWidget::DockCenter, m_tagTree, 20);
}

void KPhotoBook::slotShowToolViewTagTree() {

    if (!m_sourcedirTree->isHidden()) {
        m_tagTreeToolView->place(KDockWidget::DockCenter, m_sourcedirTree, 20);

    } else if (!m_metaInfoTree->isHidden()) {
        m_tagTreeToolView->place(KDockWidget::DockCenter, m_metaInfoTree, 20);

    } else {
        m_tagTreeToolView->place(KDockWidget::DockLeft, getMainDockWidget(), 20);
    }
    
    m_tagTreeToolView->show();
}

void KPhotoBook::slotShowToolViewSourceDirTree() {

    if (!m_tagTree->isHidden()) {
        m_sourceDirTreeToolView->place(KDockWidget::DockCenter, m_tagTree, 20);

    } else if (!m_metaInfoTree->isHidden()) {
        m_sourceDirTreeToolView->place(KDockWidget::DockCenter, m_metaInfoTree, 20);

    } else {
        m_sourceDirTreeToolView->place(KDockWidget::DockLeft, getMainDockWidget(), 20);
    }

    m_sourceDirTreeToolView->show();
}

void KPhotoBook::slotShowToolViewMetaInfoTree() {

    if (!m_tagTree->isHidden()) {
        m_metaInfoTreeToolView->place(KDockWidget::DockCenter, m_tagTree, 20);

    } else if (!m_sourcedirTree->isHidden()) {
        m_metaInfoTreeToolView->place(KDockWidget::DockCenter, m_sourcedirTree, 20);

    } else {
        m_metaInfoTreeToolView->place(KDockWidget::DockLeft, getMainDockWidget(), 20);
    }

    m_metaInfoTreeToolView->show();
}


void KPhotoBook::slotExportMatchingFiles() {

    QDir dir(Settings::fileSystemLastExportedToDirectory());
    
    QString choosedDir = selectExportingDirectory(dir.absPath());

    if (!choosedDir.isEmpty()) {
        Settings::setFileSystemLastExportedToDirectory(choosedDir);

        kdDebug() << "[KPhotoBook::slotExportSelectedFiles] exporting to '" << choosedDir << "'..." << endl;
        changeStatusbar(i18n("Exporting symbolic links..."));
        
        ExportSymlinks exporter(m_view, choosedDir);
        exporter.setSourceFiles(m_view->fileView()->items());
        exporter.execute();
        
        changeStatusbar(i18n("Exporting symbolic links finished"));
    }
}


void KPhotoBook::slotExportSelectedFiles() {

    QDir dir(Settings::fileSystemLastExportedToDirectory());
  
    QString choosedDir = selectExportingDirectory(dir.absPath());
    
    if (!choosedDir.isEmpty()) {
    
        Settings::setFileSystemLastExportedToDirectory(choosedDir);

        kdDebug() << "[KPhotoBook::slotExportSelectedFiles] exporting to '" << choosedDir << "'..." << endl;
        changeStatusbar(i18n("Exporting symbolic links..."));
        
        ExportSymlinks exporter(m_view, choosedDir);
        exporter.setSourceFiles(m_view->fileView()->selectedItems());
        exporter.execute();
        
        changeStatusbar(i18n("Exporting symbolic links finished"));
    }
}


//
// private
//
void KPhotoBook::autoRefreshView() {

    if (m_view && Settings::imagePreviewAutoRefresh()) {
        m_view->updateFiles();
    }
}


void KPhotoBook::changeStatusbar(const QString& text) {

    // display the text on the statusbar
    statusBar()->changeItem(text, 1);
}


void KPhotoBook::setupToolWindowTagTree() {

    QWidget* tagTreePanel = new QWidget(this, "tagTreePanel");
    QVBoxLayout* tagTreePanelLayout = new QVBoxLayout(tagTreePanel, 0, 0, "tagTreePanelLayout");
    tagTreePanelLayout->setAutoAdd(true);

    m_tagTreeToolBar = new KToolBar(tagTreePanel, "tagTreeToolBar", true, true);
    m_tagTreeToolBar->setIconSize(Settings::tagTreeToolBarIconSize());

    actionCollection()->action("addMaintag")->plug(m_tagTreeToolBar);

    m_tagTreeToolBar->insertSeparator();

    actionCollection()->action("expandAllTags")->plug(m_tagTreeToolBar);
    actionCollection()->action("collapseAllTags")->plug(m_tagTreeToolBar);

    QWidget* spacer = new QWidget(m_tagTreeToolBar);
    QSizePolicy sizePolicy = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorStretch(99);
    spacer->setSizePolicy(sizePolicy);
    m_tagTreeToolBar->insertWidget(99999, 99999, spacer, -1);

    actionCollection()->action("andifyTags")->plug(m_tagTreeToolBar);
    actionCollection()->action("orifyTags")->plug(m_tagTreeToolBar);
    m_tagTreeToolBar->insertSeparator();
    actionCollection()->action("deselectFilter")->plug(m_tagTreeToolBar);
    actionCollection()->action("resetFilter")->plug(m_tagTreeToolBar);
    m_tagTreeToolBar->insertSeparator();
    actionCollection()->action("toggleLockUnlockTagging")->plug(m_tagTreeToolBar);

    m_tagTree = new TagTree(tagTreePanel, this, "tagtree");

    // set the icon
    QIconSet iconSet = KGlobal::iconLoader()->loadIconSet(Constants::ICON_TAG, KIcon::Small, 16, true);
    if (iconSet.isNull()) {
        kdDebug() << "[KPhotoBook::setupToolWindowTagTree] Could not load iconset with iconname: '" << Constants::ICON_TAG << "'" << endl;
    } else {
        tagTreePanel->setIcon(iconSet.pixmap());
    }

    // eventually do add the tool window
    m_tagTreeToolView = addToolWindow(tagTreePanel, KDockWidget::DockLeft, getMainDockWidget(), 20, i18n("Tags"), i18n("Tags"));
}


void KPhotoBook::setupToolWindowSourceDirTree() {

    // create sourcedirtree toolwindow
    QWidget* sourceDirTreePanel = new QWidget(this, "sourceDirTreePanel");
    QVBoxLayout* sourceDirTreePanelLayout = new QVBoxLayout(sourceDirTreePanel, 0, 0, "sourceDirTreePanelLayout");
    sourceDirTreePanelLayout->setAutoAdd(true);

    m_sourceDirTreeToolBar = new KToolBar(sourceDirTreePanel, "sourceDirTreeToolBar", true, true);
    m_sourceDirTreeToolBar->setIconSize(Settings::sourceDirTreeToolBarIconSize());

    actionCollection()->action("addSourceDir")->plug(m_sourceDirTreeToolBar);

    m_sourceDirTreeToolBar->insertSeparator();

    actionCollection()->action("expandAllSourceDirs")->plug(m_sourceDirTreeToolBar);
    actionCollection()->action("collapseAllSourceDirs")->plug(m_sourceDirTreeToolBar);

    QWidget* spacer = new QWidget(m_sourceDirTreeToolBar);
    QSizePolicy sizePolicy = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorStretch(99);
    spacer->setSizePolicy(sizePolicy);
    m_sourceDirTreeToolBar->insertWidget(99999, 99999, spacer, -1);

    actionCollection()->action("includeAllSourceDirs")->plug(m_sourceDirTreeToolBar);
    actionCollection()->action("excludeAllSourceDirs")->plug(m_sourceDirTreeToolBar);
    actionCollection()->action("invertAllSourceDirs")->plug(m_sourceDirTreeToolBar);

    m_sourcedirTree = new SourceDirTree(sourceDirTreePanel, this, "sourcedirTree");

    // set the icon
    QIconSet iconSet = KGlobal::iconLoader()->loadIconSet(Constants::ICON_SOURCEDIR, KIcon::Small, 16, true);
    if (iconSet.isNull()) {
        kdDebug() << "[KPhotoBook::setupToolWindowSourceDirTree] Could not load iconset with iconname: '" << Constants::ICON_SOURCEDIR << "'" << endl;
    } else {
        sourceDirTreePanel->setIcon(iconSet.pixmap());
    }

    // eventually do add the tool window
    m_sourceDirTreeToolView = addToolWindow(sourceDirTreePanel, KDockWidget::DockCenter, m_tagTree, 20, i18n("Source directories"), i18n("Source"));
}


void KPhotoBook::setupToolWindowMetaInfoTree() {

    m_metaInfoTree = new KListView(this, "metaInfoTree");

    // create columns
    m_metaInfoTree->addColumn(i18n("Key"));
    m_metaInfoTree->addColumn(i18n("Value"));

    m_metaInfoTree->setSelectionMode(QListView::NoSelection);

    // we want that the value column gets as big as possible
    m_metaInfoTree->header()->setStretchEnabled(true, 1);

    // the root node must be closeable
    m_metaInfoTree->setRootIsDecorated(true);

    // set the icon
    QString iconName = "favorites";
    QIconSet iconSet = KGlobal::iconLoader()->loadIconSet(iconName, KIcon::Small, 16, true);
    if (iconSet.isNull()) {
        kdDebug() << "[KPhotoBook::setupToolWindowMetaInfoTree] Could not load iconset with iconname: '" << iconName << "'" << endl;
    } else {
        m_metaInfoTree->setIcon(iconSet.pixmap());
    }

    // eventually do add the tool window
    m_metaInfoTreeToolView = addToolWindow(m_metaInfoTree, KDockWidget::DockCenter, m_tagTree, 20, i18n("EXIF Information"), i18n("EXIF"));
}


void KPhotoBook::updateState() {

    // enable save if engine is dirty
    m_save->setEnabled(m_engine->dirty());

    // display the opened file in the caption
    setCaption(currentURL(), m_engine->dirty());

    // update the statusbar
    updateStatusBar();
}


void KPhotoBook::updateStatusBar() {

    // update the statusbar
    QString filesMsg = QString(i18n("Files: %1")).arg(m_engine->totalNumberOfFiles());
    statusBar()->changeItem(filesMsg, 2);
    QString filterMsg = QString(i18n("Matched: %1")).arg(m_engine->filteredNumberOfFiles());
    statusBar()->changeItem(filterMsg, 3);

    // I know, this is really not the right place to do this...
    // but it is the simples way
    // enable the 'export matching files' only if at least 1 file is displayed
    m_exportMatchingFiles->setEnabled(m_engine->filteredNumberOfFiles() > 0);
}


SourceDir* KPhotoBook::addSourceDir(QDir* sourceDir, bool recursive) throw(EngineException*) {

    SourceDir* newSourceDir = m_engine->addSourceDir(sourceDir, recursive);

    updateState();
    return newSourceDir;
}


void KPhotoBook::removeSourceDir(SourceDir* sourceDir) {

    m_engine->removeSourceDir(sourceDir);

    updateState();
}


TagNode* KPhotoBook::createTag(int type, const QString& name, const QString& iconName, TagNode* parent) {

    kdDebug() << "[KPhotoBook::createTag] invoked with type: " << type << ", name: " << name << ", icon: " << iconName << endl;

    TagNode* tagNode = m_engine->createTag(parent, type, name, iconName);

    updateState();
    return tagNode;
}


void KPhotoBook::applyZoomSetting() {

    m_zoomIn->setEnabled(Settings::imagePreviewSize() < Constants::SETTINGS_MAX_PREVIEW_SIZE);
    m_zoomOut->setEnabled(Settings::imagePreviewSize() > Constants::SETTINGS_MIN_PREVIEW_SIZE);
}


void KPhotoBook::applyOperatorSetting() {

    m_andifyTagsAction->setChecked(Settings::tagTreeFilterOperator() == Settings::EnumTagTreeFilterOperator::And);
    m_orifyTagsAction->setChecked(Settings::tagTreeFilterOperator() != Settings::EnumTagTreeFilterOperator::And);

    if (m_settingsTagTree) {
        m_settingsTagTree->kcfg_TagTreeFilterOperator->setCurrentItem(Settings::tagTreeFilterOperator());
    }
}


void KPhotoBook::applyAutorefreshSetting() {

    m_autoRefreshViewAction->setChecked(Settings::imagePreviewAutoRefresh());

    if (m_settingsImagePreview) {
        m_settingsImagePreview->kcfg_ImagePreviewAutoRefresh->setChecked(Settings::imagePreviewAutoRefresh());
    }

    autoRefreshView();
}


void KPhotoBook::applyLockUnlockTaggingSettings() {

    m_lockUnlockTaggingAction->setChecked(Settings::tagTreeLocked());
    if (Settings::tagTreeLocked()) {
        m_lockUnlockTaggingAction->setText("Unlock tagging");
        m_lockUnlockTaggingAction->setIcon(Constants::ICON_TAG_LOCK);
    } else {
        m_lockUnlockTaggingAction->setText("Lock tagging");
        m_lockUnlockTaggingAction->setIcon(Constants::ICON_TAG_UNLOCK);
    }
    
    if (m_tagTree) {
        m_tagTree->doRepaintAll();
    }
}


void KPhotoBook::storeTreeState() {

    KConfig* config = KGlobal::config();
    QString group = QString("TreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    
    if (Settings::tagTreeRememberTree()) {
        QStringList* openNodes = m_tagTree->getOpenNodes();
        config->writeEntry("TagTree:OpenNodes", *openNodes);
        delete openNodes;
    }
    
    if (Settings::sourceDirTreeRememberTree()) {
        QStringList* openNodes = m_sourcedirTree->getOpenNodes();
        config->writeEntry("SourceDirTree:OpenNodes", *openNodes);
        delete openNodes;
    }
            
    // force writing
    config->sync();
}
 
   
void KPhotoBook::loadTreeState() {

    KConfig* config = KGlobal::config();
    QString group = QString("TreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    
    if (Settings::tagTreeRememberTree()) {
        QStringList openNodes = config->readListEntry("TagTree:OpenNodes");
        m_tagTree->openNodes(&openNodes);
    }
    
    if (Settings::sourceDirTreeRememberTree()) {
        QStringList openNodes = config->readListEntry("SourceDirTree:OpenNodes");
        m_sourcedirTree->openNodes(&openNodes);
    }
}

/**
 * Stores the filters set on the trees.
 */
void KPhotoBook::storeFilter() {

    KConfig* config = KGlobal::config();
    QString group = QString("TreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    
    if (Settings::tagTreeRememberFilter()) {
        QIntDict<QString>* filterDict = m_tagTree->getFilter();
        
        QStringList* filterList = intDict2stringList(filterDict);
        
        config->writeEntry("TagTree:Filter", *filterList);
        
        filterDict->setAutoDelete(true);
        delete filterDict;
        delete filterList;
    }
    
    if (Settings::sourceDirTreeRememberFilter()) {
        QIntDict<QString>* filterDict = m_sourcedirTree->getFilter();
        
        QStringList* filterList = intDict2stringList(filterDict);
        
        config->writeEntry("SourceDirTree:Filter", *filterList);
        
        filterDict->setAutoDelete(true);
        delete filterDict;
        delete filterList;
    }
            
    // force writing
    config->sync();
}


/**
 * Loads the filters and sets them on the trees.
 */
void KPhotoBook::loadFilter() {

    KConfig* config = KGlobal::config();
    QString group = QString("TreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    
    if (Settings::tagTreeRememberFilter()) {
        QStringList filterList = config->readListEntry("TagTree:Filter");
        
        QIntDict<QString>* filterDict = stringList2intDict(filterList);
        
        m_tagTree->applyFilter(filterDict);
        
        filterDict->setAutoDelete(true);
        delete filterDict;
    }
    
    if (Settings::sourceDirTreeRememberFilter()) {
        QStringList filterList = config->readListEntry("SourceDirTree:Filter");
        
        QIntDict<QString>* filterDict = stringList2intDict(filterList);
        
        m_sourcedirTree->applyFilter(filterDict);
        
        filterDict->setAutoDelete(true);
        delete filterDict;
    }
}


QStringList* KPhotoBook::intDict2stringList(QIntDict<QString>* intDict) {

    QStringList* stringList = new QStringList();

    QIntDictIterator<QString> it(*intDict);
    for (; it.current(); ++it) {
        QString entry = QString("%1:%2").arg(it.currentKey()).arg(*it.current());
        stringList->append(entry);
    }
    
    return stringList;
}
    
    
QIntDict<QString>* KPhotoBook::stringList2intDict(QStringList stringList) {

    QIntDict<QString>* filterDict = new QIntDict<QString>;
    
    // loop over all entries in the stringlist
    for (QStringList::Iterator it = stringList.begin(); it != stringList.end(); ++it) {
    
        kdDebug() << "[KPhotoBook::stringList2intDict] handling entry: '" << *it << "'" << endl;
        
        // split the current entry into key and value and put them into the intdict
        int delimitorPos = (*it).find(':');
        if (delimitorPos > 0) {
            QString keyStr = (*it).mid(0, delimitorPos);
            QString value = (*it).mid(delimitorPos + 1);
            
            kdDebug() << "[KPhotoBook::stringList2intDict] key-->value: '" << keyStr << "-->" << value << "'" << endl;
            
            bool ok;
            int key = keyStr.toInt(&ok);
            
            if (ok) {
                filterDict->insert(key, new QString(value));
            } else {
                kdWarning() << "Filter " << *it << " is invalid! Id is not a number. (Valid format: 'key:value')" << endl;            
            }
            
        } else {
            kdWarning() << "Filter " << *it << " is invalid! (Valid format: 'key:value')" << endl;
        }
    }
    
    return filterDict;
}


QString KPhotoBook::selectExportingDirectory(QString startDirectory) {

  // prepare the directory chooser dialog
  KURLRequesterDlg* dialog = new KURLRequesterDlg(startDirectory, i18n("Directory to export to:"), this, "exportingDirectoryDialog", true);
  dialog->setCaption(i18n("Select the directory to export to"));
  
  // ok, this is not very beautiful code, but simple...
  while (true) {
    dialog->exec();
    QString choosedDir = dialog->selectedURL().path();

    // return QString::null if the user choosed 'cancel'
    if (choosedDir.isEmpty()) {
      delete dialog;
      return QString::null;
    }
    
    // test if the choosen directory exists and is really a directory
    KFileItem fileItem(KFileItem::Unknown, KFileItem::Unknown, choosedDir);
    if (fileItem.isDir()) {
      delete dialog;
      return choosedDir;
    }
    
    // if we got here the choosen directory is invalid
    KMessageBox::sorry(this, i18n("You must select an existing directory."), i18n("Export"));
  }
}


    
#include "kphotobook.moc"
