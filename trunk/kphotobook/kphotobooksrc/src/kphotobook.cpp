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
#include "kphotobookview.h"
#include "pref.h"
#include "engine.h"
#include "exception.h"
#include "configuration.h"
#include "dialogaddsourcedir.h"
#include "dialogcreatetag.h"
#include "dialogedittag.h"
#include "constants.h"
#include "tagtree.h"
#include "sourcedirtree.h"
#include "file.h"
#include "sourcedir.h"
#include "tagtreenodesourcedir.h"
#include "tagtreenodetitle.h"
#include "tagtreenodeboolean.h"
#include "tagnode.h"

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

#include <kdebug.h>

#include <qdragobject.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qfileinfo.h>
#include <qptrlist.h>
#include <qlistview.h>

KPhotoBook::KPhotoBook()
    : KDockMainWindow( 0, "KPhotoBook" )
    , m_view(0)
    , m_engine(new Engine()) {

    // accept dnd
    setAcceptDrops(false);

    // then, setup our actions
    setupActions();

    // show toggle menu entry for statusbar
    createStandardStatusBarAction();

    // create the gui
    createGUI();

    // let's setup our context menus
    setupContextMenus();

    // it is important to create the view after setting up context menus
//    m_view = new KPhotoBookView(this);


    KDockWidget* mainDock = createDockWidget( "Falk's MainDockWidget", 0, 0L, "main_dock_widget");
    m_view = new KPhotoBookView(this);
    mainDock->setWidget(m_view);
    // allow others to dock to the 4 sides
    mainDock->setDockSite(KDockWidget::DockCorner);
    // forbit docking abilities of mainDock itself
    mainDock->setEnableDocking(KDockWidget::DockNone);
    setView( mainDock); // central widget in a KDE mainwindow
    setMainDockWidget(mainDock); // master dockwidget
    /*
    ...
    KDockWidget* dockLeft;
    dockLeft = createDockWidget( "Intially left one", anyOtherPixmap, 0L, i18n("The left dockwidget"));
    AnotherWidget* aw = new AnotherWidget( dockLeft);
    dockLeft->setWidget( aw);
    dockLeft->manualDock( mainDock,              // dock target
                            KDockWidget::DockLeft, // dock site
                            20 );                  // relation target/this (in percent)
    */

    KDockWidget* dock = this->createDockWidget("Any window caption", 0, 0, i18n("window caption"));
    TagTree* actualWidget = new TagTree(dock, this, "tagtree in dock");
    dock->setWidget(actualWidget); // embed it
    dock->setToolTipString(i18n("That's me")); // available when appearing as tab page
    dock->manualDock(mainDock,              // dock target
                     KDockWidget::DockLeft, // dock site
                     20 );                  // relation target/this (in percent)

    // tell the KMainWindow that this is indeed the main widget
    //setCentralWidget(m_view);

    // init some other things: statusbar,..
    init();

    // apply the saved mainwindow settings, if any, and ask the mainwindow
    // to automatically save settings if changed: window size, toolbar
    // position, icon size, etc.
    setAutoSaveSettings();
}


KPhotoBook::~KPhotoBook() {

    delete m_engine;
    delete m_view;
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
        // it's very strange, but the application crashes if a delete the exception!!!
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
        Configuration::getInstance()->setLastOpenedFile(currentURL());

        updateState();

        // update the view

        // add the tagNodes to the tagtree
        m_view->tagTree()->addTagNodes(tagForest());

        // add the sourcedirectories to the tagtree
        m_view->sourceDirTree()->clear();
        m_view->sourceDirTree()->addSourceDirs(m_engine->sourceDirs());

        // add the files to the view
        m_view->updateFiles();
    } else {
        kdDebug() << "[KPhotoBook::load] nothing done, m_engine: " << m_engine << endl;
    }
}


void KPhotoBook::setupActions() {

    // file menu
    KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection());
    KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
    m_save = KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
    KStdAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection());
    KStdAction::close(this, SLOT(close()), actionCollection());

    m_save->setEnabled(false);

    KStdAction::keyBindings(this, SLOT(slotOptionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(slotOptionsConfigureToolbars()), actionCollection());
    KStdAction::preferences(this, SLOT(slotOptionsPreferences()), actionCollection());

    //
    // engine actions
    //
    new KAction(
        i18n("&Rescan filesystem"), Constants::ICON_RESCAN_FILESYSTEM,
        KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotRescanFilesystem()),
        actionCollection(), "rescanFilesystem"
    );

    //
    // view actions
    //
    KToggleAction* autoRefreshViewAction = new KToggleAction(
        i18n("&Autorefresh view"), Constants::ICON_AUTOREFRESH_VIEW,
        0,
        this, SLOT(slotAutoRefreshView()),
        actionCollection(), "autoRefreshView"
    );
    autoRefreshViewAction->setChecked(Configuration::getInstance()->autoRefresh());

    new KAction(
        i18n("&Refresh view"), Constants::ICON_REFRESH_VIEW,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotRefreshView()),
        actionCollection(), "refreshView"
    );

    new KAction(
        i18n("&Increase Previewsize"), Constants::ICON_INCREASE_PREVIEWSIZE,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotIncreasePreviewSize()),
        actionCollection(), "increasePreviewSize"
    );
        new KAction(
        i18n("&Decrease Previewsize"), Constants::ICON_DECREASE_PREVIEWSIZE,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotDecreasePreviewSize()),
        actionCollection(), "decreasePreviewSize"
    );

    //
    // sourcedir actions
    //
    new KAction(
        i18n("Add &sourcedirectory"), Constants::ICON_ADD_SOURCEDIR,
        0, //KStdAccel::shortcut(KStdAccel::New),
        this, SLOT(slotAddSourcedir()),
        actionCollection(), "addSourceDir"
    );
    new KAction(
        i18n("&Edit sourcedirectory"), Constants::ICON_EDIT_SOURCEDIR,
        0, //KStdAccel::shortcut(KStdAccel::New),
        this, SLOT(slotEditSourceDir()),
        actionCollection(), "editSourceDir"
    );
    new KAction(
        i18n("&Remove sourcedirectory"), Constants::ICON_REMOVE_SOURCEDIR,
        0, //KStdAccel::shortcut(KStdAccel::New),
        this, SLOT(slotRemoveSourceDir()),
        actionCollection(), "removeSourceDir"
    );

    new KAction(
        i18n("&Include whole folder"), Constants::ICON_INCLUDE_WHOLE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotIncludeWholeSourceDir()),
        actionCollection(), "includeWholeSourceDir"
    );
    new KAction(
        i18n("&Exclude whole folder"), Constants::ICON_EXCLUDE_WHOLE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExcludeWholeSourceDir()),
        actionCollection(), "excludeWholeSourceDir"
    );
    new KAction(
        i18n("In&vert folder selection"), Constants::ICON_INVERT_FOLDER_SELECTION,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotInvertSourceDir()),
        actionCollection(), "invertSourceDir"
    );

    new KAction(
        i18n("&Include all"), Constants::ICON_INCLUDE_WHOLE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotIncludeAllSourceDirs()),
        actionCollection(), "includeAllSourceDirs"
    );
    new KAction(
        i18n("&Exclude all"), Constants::ICON_EXCLUDE_WHOLE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExcludeAllSourceDirs()),
        actionCollection(), "excludeAllSourceDirs"
    );
    new KAction(
        i18n("In&vert all"), Constants::ICON_INVERT_FOLDER_SELECTION,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotInvertAllSourceDirs()),
        actionCollection(), "invertAllSourceDirs"
    );

    new KAction(
        i18n("Expand sourcedir"), Constants::ICON_EXPAND_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExpandSourceDir()),
        actionCollection(), "expandSourceDir"
    );
    new KAction(
        i18n("Collapse sourcedir"), Constants::ICON_COLLAPSE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotCollapseSourceDir()),
        actionCollection(), "collapseSourceDir"
    );
    new KAction(
        i18n("Expand all sourcedirs"), Constants::ICON_EXPAND_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExpandAllSourceDirs()),
        actionCollection(), "expandAllSourceDirs"
    );
    new KAction(
        i18n("Collapse all sourcedirs"), Constants::ICON_COLLAPSE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotCollapseAllSourceDirs()),
        actionCollection(), "collapseAllSourceDirs"
    );

    //
    // tag actions
    //
    new KAction(
        i18n("Add &maintag"), Constants::ICON_CREATE_MAINTAG,
        0, //KStdAccel::shortcut(KStdAccel::New),
        this, SLOT(slotAddMaintag()),
        actionCollection(), "addMaintag"
    );
    new KAction(
        i18n("&Create subtag"), Constants::ICON_CREATE_SUBTAG,
        0, //KStdAccel::shortcut(KStdAccel::New),
        this, SLOT(slotCreateSubtag()),
        actionCollection(), "createSubtag"
    );
    new KAction(
        i18n("&Edit tag"), Constants::ICON_EDIT_TAG,
        0, //KStdAccel::shortcut(KStdAccel::New),
        this, SLOT(slotEditTag()),
        actionCollection(), "editTag"
    );
    new KAction(
        i18n("&Delete tag"), Constants::ICON_DELETE_TAG,
        0, //KStdAccel::shortcut(KStdAccel::New),
        this, SLOT(slotDeleteTag()),
        actionCollection(), "deleteTag"
    );

    KToggleAction* andifyTagsAction = new KToggleAction(
        i18n("Andify tags"), "attach",
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotAndifyTags()),
        actionCollection(), "andifyTags"
    );
    andifyTagsAction->setChecked(Configuration::getInstance()->tagfilterOperator() == "&");

    new KAction(
        i18n("Expand tag"), Constants::ICON_EXPAND_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExpandTag()),
        actionCollection(), "expandTag"
    );
    new KAction(
        i18n("Collapse tag"), Constants::ICON_COLLAPSE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotCollapseTag()),
        actionCollection(), "collapseTag"
    );
    new KAction(
        i18n("Expand all tags"), Constants::ICON_EXPAND_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),
        this, SLOT(slotExpandAllTags()),
        actionCollection(), "expandAllTags"
    );
    new KAction(
        i18n("Collapse all tags"), Constants::ICON_COLLAPSE_FOLDER,
        0, //KStdAccel::shortcut(KStdAccel::Reload),m_view->sourceDirTree()
        this, SLOT(slotCollapseAllTags()),
        actionCollection(), "collapseAllTags"
    );
}


void KPhotoBook::setupContextMenus() {

    m_contextMenuSourceDirTree = static_cast<KPopupMenu*>(guiFactory()->container("contextMenu_sourceDirTree", this));
    m_contextMenuSourceDir = static_cast<KPopupMenu*>(guiFactory()->container("contextMenu_sourceDir", this));
    m_contextMenuSubDir = static_cast<KPopupMenu*>(guiFactory()->container("contextMenu_subDir", this));

    m_contextMenuTagTree = static_cast<KPopupMenu*>(guiFactory()->container("contextMenu_tagTree", this));
    m_contextMenuTagTreeItem = static_cast<KPopupMenu*>(guiFactory()->container("contextMenu_tagTreeItem", this));
}


QPtrList<File>* KPhotoBook::files(QString filter) {

    // build the filter from the tagtree if the specified filter is empty
    if (filter.isNull() && m_view) {
        TagTreeNode* lastItem = 0;
        QListViewItemIterator it(m_view->tagTree());
        while (it.current()) {
            TagTreeNode* item = dynamic_cast<TagTreeNode*>(it.current());

            if (!item->filter().isNull()) {
                if (lastItem) {
                    filter.append(Configuration::getInstance()->tagfilterOperator());
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
    Configuration::getInstance()->store();

    // store the data if necessary
    if (m_engine && m_engine->dirty()) {

        // show save, discard, abort dialog
        QString fileName = QString("");
        if (m_engine->name()) {
            fileName = QString("%1\n").arg(m_engine->fileinfo()->absFilePath());
        }

        QString text = QString(i18n("The document has been modified.\n%1Do you want to save it?")).arg(fileName);

        int button = KMessageBox::warningYesNoCancel(
                         this, // parent
                         text, // text
                         i18n("Save Document"), // caption
                         (m_engine->name() ? KStdGuiItem::save() : KStdGuiItem::saveAs()),   // buttonYes
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
        if (m_engine->name()) {
            fileName = QString("%1\n").arg(m_engine->fileinfo()->absFilePath());
        }

        QString text = QString(i18n("The document has been modified.\n%1Do you want to save it?")).arg(fileName);

        int button = KMessageBox::warningYesNoCancel(
                         this, // parent
                         text, // text
                         i18n("Save Document"), // caption
                         (m_engine->name() ? KStdGuiItem::save() : KStdGuiItem::saveAs()),   // buttonYes
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
    QString lastFileName = Configuration::getInstance()->lastOpenedFile();

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
                delete ex;
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
                Configuration::getInstance()->setLastOpenedFile(currentURL());
                fileSaved = true;
            } catch(PersistingException* ex) {
                KMessageBox::detailedError(m_view, ex->message(), ex->detailMessage(), i18n("Saving failed"));
                delete ex;
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
    saveMainWindowSettings(KGlobal::config(), autoSaveGroup());
}


void KPhotoBook::slotOptionsPreferences() {

    // popup some sort of preference dialog, here
    KPhotoBookPreferences dlg;
    if (dlg.exec()) {
        // redo your settings
    }
}


void KPhotoBook::slotNewToolbarConfig() {

    // this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
    // recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
    createGUI();

    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
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
            m_view->sourceDirTree()->addSourceDir(sourceDir);

            // update the view to display the new found files
            kdDebug() << "[KPhotoBook::slotAddSourcedir] updating fileview" << endl;
            m_view->updateFiles();

            // sourcedir added successfully
            newDirIsOk = true;
        } catch(EngineException* ex) {
            kdDebug() << "[KPhotoBook::slotAddSourcedir] adding choosen sourcedir failed, dir: " << dialog->directory()->absPath() << ", recursive: " << dialog->recursive() << endl;

            KMessageBox::detailedError(dialog, ex->message(), ex->detailMessage(), i18n("Adding sourcedir failed"));
            delete ex;
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
    TagTreeNodeSourceDir* currentNode = m_view->selectedSourceDir();

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
        m_view->sourceDirTree()->removeSourceDir(currentNode);

        kdDebug() << "[KPhotoBook::slotRemoveSourceDir] sourcedirnode from sourcedirtree removed... " << endl;

        // update the shown files
        m_view->updateFiles();
    }

    kdDebug() << "[KPhotoBook::slotRemoveSourceDir] ended... " << endl;
}


void KPhotoBook::slotAddMaintag() {

    kdDebug() << "[KPhotoBook::slotRemoveSourceDir] called... " << endl;

    DialogCreateTag* dialog = new DialogCreateTag(m_view, 0, "DialogCreateTag");
    if (dialog->exec()) {

        kdDebug() << "[KPhotoBook::slotRemoveSourceDir] dialog exited with OK, type: " << dialog->tagType() << ", name: " << dialog->tagName() << ", icon: " << dialog->tagIcon() << endl;

        // let the engine create the new tagnode
        TagNode* newTagNode = createTag(dialog->tagType(), dialog->tagName(), dialog->tagIcon());

        // add the new tagnode to the tagnodetree
        m_view->tagTree()->addTagNode(newTagNode);
    }
    delete dialog;
}


void KPhotoBook::slotCreateSubtag() {

    kdDebug() << "[KPhotoBook::slotCreateSubtag] called... " << endl;

    // get the tag to add a child to
    QListViewItem* currentItem = m_view->tagTree()->currentItem();
    if (typeid(*currentItem) != typeid(TagTreeNodeTitle)
        && typeid(*currentItem) != typeid(TagTreeNodeBoolean)) {
        kdDebug() << "[KPhotoBook::slotCreateSubtag] called on a tree item other than TagTreeNode!" << endl;
        return;
    }
    TagTreeNode* parent = dynamic_cast<TagTreeNode*>(currentItem);

    DialogCreateTag* dialog = new DialogCreateTag(m_view, parent, "DialogCreateTag");
    if (dialog->exec()) {

        kdDebug() << "[KPhotoBook::slotCreateSubtag] dialog exited with OK, type: " << dialog->tagType() << ", name: " << dialog->tagName() << ", icon: " << dialog->tagIcon() << endl;

        // let the engine create the new tagnode
        TagNode* newTagNode = createTag(dialog->tagType(), dialog->tagName(), dialog->tagIcon(), parent->tagNode());

        // add the new tagnode to the tagnodetree
        m_view->tagTree()->addTagNode(parent, newTagNode);
        parent->setOpen(true);
    }
    delete dialog;
}


void KPhotoBook::slotEditTag() {

    kdDebug() << "[KPhotoBook::slotEditTag] called... " << endl;

    // get the tag to add a child to
    QListViewItem* currentItem = m_view->tagTree()->currentItem();
    if (typeid(*currentItem) != typeid(TagTreeNodeTitle)
        && typeid(*currentItem) != typeid(TagTreeNodeBoolean)) {
        kdDebug() << "[KPhotoBook::slotCreateSubtag] called on a tree item other than TagTreeNode!" << endl;
        return;
    }
    TagTreeNode* tagTreeNode = dynamic_cast<TagTreeNode*>(currentItem);

    DialogEditTag* dialog = new DialogEditTag(m_view, tagTreeNode, "DialogEditTag");
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
    QListViewItem* currentItem = m_view->tagTree()->currentItem();
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

        // update the shown files
        m_view->updateFiles();

        // remove the tag from the view
        m_view->removeTagNode(tagTreeNode);
    }

    updateState();
}


void KPhotoBook::slotRescanFilesystem() {

    kdDebug() << "[KPhotoBook::slotRescanFilesystem] called... " << endl;

    m_engine->rescanSourceDirs();

    // add the sourcedirectories to the tagtree
    m_view->sourceDirTree()->clear();
    m_view->sourceDirTree()->addSourceDirs(m_engine->sourceDirs());

    // add the files to the view
    m_view->updateFiles();

    updateState();
}


void KPhotoBook::slotAutoRefreshView() {
    Configuration::getInstance()->setAutoRefresh(!Configuration::getInstance()->autoRefresh());

    autoRefreshView();
}
void KPhotoBook::slotRefreshView() {
    m_view->updateFiles();
}

void KPhotoBook::slotIncreasePreviewSize() {
    m_view->increasePreviewSize();
}
void KPhotoBook::slotDecreasePreviewSize() {
    m_view->decreasePreviewSize();
}


void KPhotoBook::slotIncludeWholeSourceDir() {
    m_view->sourceDirTree()->includeWholeSourceDir();

    autoRefreshView();
}
void KPhotoBook::slotExcludeWholeSourceDir() {
    m_view->sourceDirTree()->excludeWholeSourceDir();

    autoRefreshView();
}
void KPhotoBook::slotInvertSourceDir() {
    m_view->sourceDirTree()->invertSourceDir();

    autoRefreshView();
}
void KPhotoBook::slotIncludeAllSourceDirs() {
    m_view->sourceDirTree()->includeAllSourceDirs();

    autoRefreshView();
}
void KPhotoBook::slotExcludeAllSourceDirs() {
    m_view->sourceDirTree()->excludeAllSourceDirs();

    autoRefreshView();
}
void KPhotoBook::slotInvertAllSourceDirs() {
    m_view->sourceDirTree()->invertAllSourceDirs();

    autoRefreshView();
}


void KPhotoBook::slotExpandSourceDir() {
    m_view->sourceDirTree()->expandCurrent();
}
void KPhotoBook::slotCollapseSourceDir() {
    m_view->sourceDirTree()->collapseCurrent();
}
void KPhotoBook::slotExpandAllSourceDirs() {
    m_view->sourceDirTree()->expandAll();
}
void KPhotoBook::slotCollapseAllSourceDirs() {
    m_view->sourceDirTree()->collapseAll();
}


void KPhotoBook::slotAndifyTags() {
    Configuration::getInstance()->invertTagfilterOperation();

    autoRefreshView();
}

void KPhotoBook::slotExpandTag() {
    m_view->tagTree()->expandCurrent();
}
void KPhotoBook::slotCollapseTag() {
    m_view->tagTree()->collapseCurrent();
}
void KPhotoBook::slotExpandAllTags() {
    m_view->tagTree()->expandAll();
}
void KPhotoBook::slotCollapseAllTags() {
    m_view->tagTree()->collapseAll();
}


void KPhotoBook::slotFileSelectionChanged() {

    // update the statusbar to reflect the number of selected files
    QString selectedMsg = QString(i18n("Selected: %1")).arg(m_view->fileView()->selectedItems()->count());
    statusBar()->changeItem(selectedMsg, 4);

    m_view->sourceDirTree()->reflectSelectedFiles(m_view->fileView()->selectedItems());

    m_view->tagTree()->doRepaintAll();
}


//
// private
//
void KPhotoBook::autoRefreshView() {

    if (Configuration::getInstance()->autoRefresh()) {
        view()->updateFiles();
    }
}


void KPhotoBook::changeStatusbar(const QString& text) {

    // display the text on the statusbar
    statusBar()->changeItem(text, 1);
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


#include "kphotobook.moc"
