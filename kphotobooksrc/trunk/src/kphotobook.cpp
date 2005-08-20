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
#include "exception.h"
#include "kphotobookview.h"

#include "dialogs/dialogaddsourcedir.h"
#include "dialogs/dialogmanagetag.h"
#include "engine/engine.h"
#include "engine/file.h"
#include "engine/filternodeopand.h"
#include "engine/filternodeopor.h"
#include "engine/sourcedir.h"
#include "engine/tagnode.h"
#include "export/exportsymlinks.h"
#include "import/imageimporter.h"
#include "settings/settings.h"
#include "settings/settingsfilehandling.h"
#include "settings/settingsgeneral.h"
#include "settings/settingsimagepreview.h"
#include "settings/settingssourcedirtree.h"
#include "settings/settingstagtree.h"
#include "settings/settingstools.h"
#include "uitrees/sourcedirtree.h"
#include "uitrees/sourcedirtreenode.h"
#include "uitrees/tagtree.h"
#include "uitrees/tagtreenode.h"
#include "uitrees/tagtreenodeboolean.h"
#include "uitrees/tagtreenodedatetime.h"
#include "uitrees/tagtreenoderadio.h"
#include "uitrees/tagtreenoderadiogroup.h"
#include "uitrees/tagtreenodestring.h"
#include "uitrees/tagtreenodetitle.h"

#include <kaccel.h>
#include <kapplication.h>
#include <kcombobox.h>
#include <kconfigdialog.h>
#include <kdeversion.h>
#include <kedittoolbar.h>
#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kio/netaccess.h>
#include <kkeydialog.h>
#include <klistview.h>
#include <klocale.h>
#include <kmditoolviewaccessor.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <ktip.h>
#include <ktoolbar.h>
#include <kurl.h>
#include <kurldrag.h>
#include <kurlrequesterdlg.h>

#include <qdragobject.h>
#include <qfileinfo.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qpaintdevicemetrics.h>
#include <qpainter.h>
#include <qptrlist.h>
#include <qsizepolicy.h>
#include <qstringlist.h>

#include <typeinfo>


Tracer* KPhotoBook::tracer = Tracer::getInstance("kde.kphotobook", "KPhotoBook");


KPhotoBook::KPhotoBook(KSplashScreen* splash, KMdi::MdiMode mdiMode) :
    KMdiMainFrm(0, "kphotobookMainWindow", mdiMode),
    m_view(0),
    m_tagTree(0),
    m_sourcedirTree(0),
    m_engine(new Engine()),

    m_inTagtreeTemporaryUnlocking(false),
    m_tagtreeWasLocked(false),

    m_tagTreeToolBar(0),
    m_sourceDirTreeToolBar(0),

    m_actions(new ActionProvider(this)),
    m_menus(0),

    m_settingsGeneral(0),
    m_settingsImagePreview(0),
    m_settingsTagTree(0),
    m_settingsSourceDirTree(0),
    m_settingsFileHandling(0),
    m_settingsTools(0)
{
    if (splash) {
        m_splashScreen = splash;
        m_splashTimer = new QTimer(this);
        connect(m_splashTimer, SIGNAL(timeout()), this, SLOT(slotSplashTimerFired()));
        m_splashTimer->start(1000, true);
    }

    // no idea why i call this, but it sounds good (because i do not really want an mdi application...)
    fakeSDIApplication();

    // accept dnd
    setAcceptDrops(false);

    // show toggle menu entry for statusbar
    createStandardStatusBarAction();

    // create the gui
    // creates the menus and toolbars from the ui.rc files
    // all actions used in the menus and toolbars must already be instantiated here!
    KMdiMainFrm::createGUI(0);

    // let's setup our context menus
    // it is important to this here and not earlier!!!
    // createGUI must be called before this
    m_menus = new MenuProvider(this);

    // apply action states
    applyAutorefreshSetting();
    applyZoomSetting();
    applyLockUnlockTaggingSettings();
    applyOperatorSetting();
    
    // it is important to create the view after setting up context menus
    m_view = new KPhotoBookView(this);

    // add the KPhotoBookView as mainwindow
    addWindow(m_view);

    // create toolwindows
    setupToolWindowTagTree();
    setupToolWindowSourceDirTree();

    // init some other things: statusbar,..
    init();

    // add the tagNodes to the tagtree (an EMPTY engine also can have tags!)
    m_tagTree->addTagNodes(tagForest());

    // apply the saved mainwindow settings, if any, and ask the mainwindow
    // to automatically save settings if changed: window size, toolbar
    // position, icon size, etc.
    setAutoSaveSettings();

    // read dock configuration
    readDockConfig(KGlobal::config(), "DockConfig");

    // show tip of the day
    // the default implementation does not show the TipOfDay dialog on first startup
    // and shows the dielog only once a week... we want that the dialog is shown
    // everytime if the user has not disabled it!
    KConfigGroup configGroup(kapp->config(), "TipOfDay");
    bool runOnStart = configGroup.readBoolEntry("RunOnStart", true);
    if (runOnStart) {
        KTipDialog::showTip(this, QString::null, true);
    }
}


KPhotoBook::~KPhotoBook()
{
    delete m_engine;
    delete m_actions;
    delete m_menus;

    closeWindow(m_view);
}


void KPhotoBook::init()
{
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


QString KPhotoBook::currentURL()
{
    if (m_engine && m_engine->fileinfo()) {
        return m_engine->fileinfo()->absFilePath();
    } else {
        return "";
    }
}


QPtrList<SourceDir>* KPhotoBook::sourceDirs()
{
    return m_engine->sourceDirs();
}


QPtrList<TagNode>* KPhotoBook::tagForest()
{
    return m_engine->tagForest();
}


TagNodeTitle* KPhotoBook::getExifTagNodeTitle()
{
    return m_engine->getExifTagNodeTitle();
}


void KPhotoBook::dirtyfy()
{
    m_engine->dirtyfy();
    updateState();
}


bool KPhotoBook::isTagTextValid(TagTreeNode* parent, QString& text)
{
    TagNode* tagNode = 0;
    if (parent) {
        tagNode = parent->tagNode();
    }

    return m_engine->isTagTextValid(tagNode, text);
}


void KPhotoBook::load(QFileInfo& fileinfo)
{
    tracer->sinvoked(__func__) << "invoked with file: " << fileinfo.absFilePath() << endl;

    Engine* newEngine = 0;
    try {
        newEngine = new Engine(fileinfo);
    } catch(EngineException* ex) {
        tracer->serror(__func__) << "Caught an exception during loading the file '" << fileinfo.absFilePath() << "'. Aborting loading. Exception: " << ex->toString() << endl;
        QString msg = QString(i18n("Could not open file: '%1'")).arg(fileinfo.absFilePath());
        KMessageBox::detailedSorry(m_view, msg, ex->toString(), i18n("Opening file failed"));

        ///@todo it's very strange, but the application crashes if I delete the exception!!!
        //delete ex;
    }

    tracer->sdebug(__func__) << "New engine instantiated: " << newEngine << endl;

    // load the new engine if there is any
    if (newEngine) {

        tracer->sdebug(__func__) << "Deleting current engine: " << m_engine << endl;

        // delete current engine
        delete m_engine;

        // set the new angine as current engine
        m_engine = newEngine;
        Settings::setFileSystemLastOpenedFile(currentURL());

        updateState();

        //
        // update the view
        //

        //first clear the tagTree, otherwise we get doubles on loading a new .kpb
        m_tagTree->clear();

        // add the tagNodes to the tagtree
        m_tagTree->addTagNodes(tagForest());

        // add the sourcedirectories to the tagtree
        m_sourcedirTree->clear();
        m_sourcedirTree->addSourceDirs(m_engine->sourceDirs());

        // restore the tree states
        loadTreeState();
        loadFilter();

        // restore the view
        m_view->loadConfiguration();

        // add the files to the view
        m_view->updateFiles();
    } else {
        tracer->sdebug(__func__) << "Nothing done, still using old m_engine: " << m_engine << endl;
    }
}


QPtrList<File>* KPhotoBook::files(FilterNode *filterRootNode)
{
    tracer->invoked(__func__);

    // build the filter from the tagtree if the specified filter is empty
    if (filterRootNode == 0 && m_view) {
        if (Settings::tagTreeFilterOperator() == Settings::EnumTagTreeFilterOperator::Or) {
            filterRootNode = new FilterNodeOpOr(0);
        } else {
            filterRootNode = new FilterNodeOpAnd(0);
        }

        QListViewItemIterator it(m_tagTree);
        while (it.current()) {
            TagTreeNode* item = dynamic_cast<TagTreeNode*>(it.current());

            FilterNode* filterNode = item->filter();

            if (filterNode != 0) {
                filterRootNode->addChild(filterNode);
            }

            ++it;
        }
    }

    // get the files matching the filter
    QPtrList<File>* files = m_engine->files(filterRootNode);

    // delete the filter tree
    delete filterRootNode;

    updateState();
    return files;
}


const QString* KPhotoBook::uid()
{
    return m_engine->uid();
}


//
// protected
//
bool KPhotoBook::queryClose()
{
    tracer->invoked(__func__);

    //store returnvalue temporary...
    bool retval = true;

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
        case KMdi::TabPageMode: {
            Settings::setGeneralViewMode(Settings::EnumGeneralViewMode::TabPageMode);
            break;
        }
        default: {
            Settings::setGeneralViewMode(Settings::EnumGeneralViewMode::IDEAlMode);
            break;
        }
    }

    //save the recent files
    m_actions->saveRecentFiles();

    // force writing the settings
    Settings::writeConfig();

    // store dock configuration
    writeDockConfig(KGlobal::config(), "DockConfig");

    // store the tree states
    if (m_engine->uid()) {
        storeTreeState();
        storeFilter();
    }

    // check for untaged images
    if (!checkForUntagged()) {
        return false;
    }

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
            retval = slotFileSave();
            break;

        case KMessageBox::No :
            // just close the application
            retval = true;
            break;

        case KMessageBox::Cancel :
            // abort closing if cancel is clicked
            retval = false;
            break;
        }
    }

    return retval;
}


// this checks, if there are untagged images, and if so, informs the user about that
// problems: if there are untagged images, and the users decides to not exit kpb,
// the old treestatefilter is set to the new one on exit. but there is no suitable
// sollution for this, i think. On the other hand this is not a real problem, as we
// most probably can consider the user to change his filtersettings while tagging
// the images.
bool KPhotoBook::checkForUntagged()
{
    tracer->invoked(__func__);

    // only perform the check if the user likes to have that feature
    if (! Settings::generalCheckUntaggedOnExit() ) {
        return true;
    }

    //now switch the filter to 'and' and untagged images
    slotAndifyTags();
    m_tagTree->deselectFilter();

    // refresh tag view
    slotRefreshView();

    // and check, if there are untagged images
    if ( m_engine->filteredNumberOfFiles() ) {

        //this misuses the nice kde "don't show again" function to disable our settings.
        int button = KMessageBox::warningYesNo(this,
                                               "<b></b>There are untagged images...<br>The images without tags are shown in the iconview.<br><b>Do you want to exit anyway?</b>",
                                               "Untagged Images",
                                               KStdGuiItem::yes(),
                                               KStdGuiItem::no(),
                                               "checkForUntagged");

        KMessageBox::ButtonCode unused;
        if (!KMessageBox::shouldBeShownYesNo("checkForUntagged", unused)) {
            KMessageBox::enableMessage("checkForUntagged");
            Settings::setGeneralCheckUntaggedOnExit(false);
            Settings::writeConfig();
        }
        if ( button == KMessageBox::No ) {
            return false;
        }
    } else {
        tracer->info(__func__, "No untagged files found.");
    }

    return true;
}


bool KPhotoBook::queryExit()
{
    tracer->invoked(__func__);

    return true;
}


/**
 * this slot is called, when the splash timer fires.
 */
void KPhotoBook::slotSplashTimerFired()
{
    if (m_splashScreen) {
        m_splashScreen->finish(this);
        delete m_splashScreen;

        delete m_splashTimer;
    }
}


//
// private slots
//
void KPhotoBook::slotFileNew()
{
    // this slot is called whenever the File->New menu is selected,
    // the New shortcut is pressed (usually CTRL+N) or the New toolbar
    // button is clicked

    // create a new window
    (new KPhotoBook())->show();
}


void KPhotoBook::slotFileOpen(const KURL& url)
{
    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked. in those cases url isEmpty() !.
    // if it is called by openRecent Action, url contains the url to load

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

    //again, if url.isEmpty(), the uses has to be querried for the file to
    // open. otherwise we try to open the given KURL
    QString fileName = "";
    if (url.isEmpty()) {
        // try to get last opened file
        QString lastFileName = Settings::fileSystemLastOpenedFile();

        QString lastDirName("");
        if (!lastFileName.isEmpty()) {
            lastDirName = QFileInfo(lastFileName).dirPath(true);
        }

        // standard filedialog
        QString fileFilter = QString("*.%1").arg(Constants::FILE_EXTENSION);
        fileName = KFileDialog::getOpenFileName(lastDirName, fileFilter, this, i18n("Open Location"));
    } else {
        fileName = url.path();
    }

    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);

        if (fileInfo.exists()) {
            load(fileInfo);

            //if we loaded by hand, save the file as recently used
            if (url.isEmpty()) {
                m_actions->addRecentFile(KURL(fileInfo.absFilePath()));
            }
        }
    }

    updateState();
}


bool KPhotoBook::slotFileSave()
{
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
                ///@todo it's very strange, but the application crashes if I delete the exception!!!
                //delete ex;
            }
        } else {
            return slotFileSaveAs();
        }
    }

    updateState();
    return fileSaved;
}


bool KPhotoBook::slotFileSaveAs()
{
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
                ///@todo it's very strange, but the application crashes if I delete the exception!!!
                //delete ex;
            }
        }
    }

    updateState();
    return fileSaved;
}


void KPhotoBook::slotToggleFullscreen()
{
    if (isFullScreen()) {
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    } else {
        setWindowState(windowState() | Qt::WindowFullScreen);
    }
    setActiveWindow();
}


void KPhotoBook::slotOptionsConfigureKeys()
{
    KKeyDialog::configure(actionCollection(), "kphotobookui.rc");
}


void KPhotoBook::slotOptionsConfigureToolbars()
{
    // use the standard toolbar editor
    KEditToolbar dlg(factory());
    if (dlg.exec()) {
        applyMainWindowSettings( KGlobal::config(), "MainWindow" );
    }
}


void KPhotoBook::slotOptionsPreferences()
{
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


void KPhotoBook::slotTipOfDay()
{
    // show tip of the day
    KTipDialog::showTip(this, QString::null, true);
}


void KPhotoBook::slotAddSourcedir()
{
    tracer->invoked(__func__);

    DialogAddSourceDir* dialog = new DialogAddSourceDir(m_view, "DialogAddSourceDir");

    bool newDirIsOk = false;
    while (!newDirIsOk && dialog->exec()) {

        tracer->sdebug(__func__) << "Dialog exited with OK, dir: " << dialog->directory()->absPath() << ", recursive: " << dialog->recursive() << endl;

        try {
            // add the sourcedir to the engine
            SourceDir* sourceDir = addSourceDir(dialog->directory(), dialog->recursive());

            tracer->debug(__func__, "New sourcedirectory is ok. adding it to the view...");
            m_sourcedirTree->addSourceDir(sourceDir);

            // update the view to display the new found files
            tracer->debug(__func__, "updating fileview");
            m_view->updateFiles();

            // sourcedir added successfully
            newDirIsOk = true;
        } catch(EngineException* ex) {
            tracer->serror(__func__) << "adding chosen sourcedir failed, dir: " << dialog->directory()->absPath() << ", recursive: " << dialog->recursive() << endl;

            KMessageBox::detailedError(dialog, ex->message(), ex->detailMessage(), i18n("Adding sourcedir failed"));

            ///@todo it's very strange, but the application crashes if I delete the exception!!!
            //delete ex;
        }
    }
    delete dialog;
}


void KPhotoBook::slotEditSourceDir()
{
    tracer->invoked(__func__);

    tracer->error(__func__, "NOT IMPLEMENTED YET");

    ///@todo implement SourceDirPopupMenu::editSourceDir()
}


void KPhotoBook::slotRemoveSourceDir()
{
    tracer->invoked(__func__);

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
        tracer->sinfo(__func__) << "Removing source directory: " << currentNode->sourceDir()->dir()->absPath() << endl;

        // remove all items from the view without deleting them
        m_view->removeAllFiles();

        // force clearing the view (to avoid a crash of the application)
        m_view->fileView()->clearView();

        // remove the sourcedir from the engine
        removeSourceDir(currentNode->sourceDir());

        // remove the sourcedir from the view
        m_sourcedirTree->removeSourceDir(currentNode);

        // update the shown files
        m_view->updateFiles();
    }
}


void KPhotoBook::slotAddMaintag()
{
    tracer->invoked(__func__);

    DialogManageTag* dialog = new DialogManageTag(m_view, DialogManageTag::MODE_CREATE_TAG, 0, 0, this, "DialogManageTag");
    tracer->debug(__func__, "dialog created... executing now...");
    if (dialog->exec()) {

        tracer->sinfo(__func__) << "Dialog exited with OK, type: " << dialog->tagType() << ", name: " << dialog->tagName() << ", icon: " << dialog->tagIcon() << endl;

        // let the engine create the new tagnode
        TagNode* newTagNode = createTag(dialog->tagType(), dialog->tagName(), dialog->tagComment(), dialog->tagIcon());
        newTagNode->setSecret(dialog->tagSecret());

        // add the new tagnode to the tagnodetree
        m_tagTree->addTagNode(newTagNode);
    }
    delete dialog;
}


void KPhotoBook::slotCreateSubtag()
{
    tracer->invoked(__func__);

    // get the tag to add a child to
    QListViewItem* currentItem = m_tagTree->currentItem();
    if (typeid(*currentItem) != typeid(TagTreeNodeTitle)
        && typeid(*currentItem) != typeid(TagTreeNodeBoolean)
        && typeid(*currentItem) != typeid(TagTreeNodeRadioGroup)
       ) {

        tracer->error(__func__, "Called on a tree item other than TagTreeNode!");
        return;
    }
    TagTreeNode* parent = dynamic_cast<TagTreeNode*>(currentItem);

    DialogManageTag* dialog = new DialogManageTag(m_view, DialogManageTag::MODE_CREATE_TAG, parent, 0, this, "DialogManageTag");
    if (dialog->exec()) {

        tracer->sdebug(__func__) << "Dialog exited with OK, type: " << dialog->tagType() << ", name: " << dialog->tagName() << ", icon: " << dialog->tagIcon() << endl;

        // let the engine create the new tagnode
        TagNode* newTagNode = createTag(dialog->tagType(), dialog->tagName(), dialog->tagComment(), dialog->tagIcon(), parent->tagNode());

        // add the new tagnode to the tagnodetree
        m_tagTree->addTagNode(parent, newTagNode);
        parent->setOpen(true);
    }
    delete dialog;
}


void KPhotoBook::slotEditTag()
{
    tracer->invoked(__func__);

    // get the tag to add a child to
    QListViewItem* currentItem = m_tagTree->currentItem();
    if (typeid(*currentItem) != typeid(TagTreeNodeTitle)
        && typeid(*currentItem) != typeid(TagTreeNodeBoolean)
        && typeid(*currentItem) != typeid(TagTreeNodeString)
        && typeid(*currentItem) != typeid(TagTreeNodeRadioGroup)
        && typeid(*currentItem) != typeid(TagTreeNodeRadio)
        && typeid(*currentItem) != typeid(TagTreeNodeDateTime)
       ) {

        tracer->error(__func__, "Called on a tree item other than TagTreeNode!");
        return;
    }
    TagTreeNode* tagTreeNode = dynamic_cast<TagTreeNode*>(currentItem);

    DialogManageTag* dialog = new DialogManageTag(m_view, DialogManageTag::MODE_EDIT_TAG, 0, tagTreeNode, this, "DialogManageTag");
    if (dialog->exec()) {

        tracer->sdebug(__func__) << "Dialog exited with OK, newname: " << dialog->tagName() << ", newicon: " << dialog->tagIcon() << endl;

        // update the tagnode and the tagtreenode
        m_engine->editTag(tagTreeNode->tagNode(), dialog->tagName(), dialog->tagComment(), dialog->tagIcon());
        tagTreeNode->tagNode()->setSecret(dialog->tagSecret());
        tagTreeNode->refresh();

    }
    delete dialog;

    updateState();
}


void KPhotoBook::slotDeleteTag()
{
    tracer->invoked(__func__);

    // get the tag to add a child to
    QListViewItem* currentItem = m_tagTree->currentItem();
    if (typeid(*currentItem) != typeid(TagTreeNodeTitle)
        && typeid(*currentItem) != typeid(TagTreeNodeBoolean)
        && typeid(*currentItem) != typeid(TagTreeNodeString)
        && typeid(*currentItem) != typeid(TagTreeNodeRadioGroup)
        && typeid(*currentItem) != typeid(TagTreeNodeRadio)
        && typeid(*currentItem) != typeid(TagTreeNodeDateTime)
       ) {
        tracer->error(__func__, "Called on a tree item other than TagTreeNode!");
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

        tracer->sdebug(__func__) << "Dialog exited with OK, deleteing tag: " << *(tagTreeNode->tagNode()->text()) << endl;

        // remove the tag from the engine
        m_engine->removeTag(tagTreeNode->tagNode());

        // remove the tag from the view
        m_view->removeTagNode(tagTreeNode);

        // update the shown files
        m_view->updateFiles();
    }

    updateState();
}


void KPhotoBook::slotToggleLockUnlockTagging()
{
    tracer->invoked(__func__);

    Settings::setTagTreeLocked(!Settings::tagTreeLocked());

    applyLockUnlockTaggingSettings();
}


void KPhotoBook::slotRescanFilesystem()
{
    tracer->invoked(__func__);

    m_engine->fileSystemScanner()->rescan();

    // add the sourcedirectories to the tagtree
    m_sourcedirTree->clear();
    m_sourcedirTree->addSourceDirs(m_engine->sourceDirs());

    // add the files to the view
    m_view->updateFiles();

    updateState();
}


void KPhotoBook::slotAutoRefreshView()
{
    Settings::setImagePreviewAutoRefresh(!Settings::imagePreviewAutoRefresh());

    applyAutorefreshSetting();
}


void KPhotoBook::slotRefreshView()
{
    m_view->updateFiles();
}


void KPhotoBook::slotIncreasePreviewSize()
{
    Settings::setImagePreviewSize(Settings::imagePreviewSize() + 8);

    applyZoomSetting();

    m_view->updateCurrentImageSize();
}


void KPhotoBook::slotDecreasePreviewSize()
{
    Settings::setImagePreviewSize(Settings::imagePreviewSize() - 8);

    applyZoomSetting();

    m_view->updateCurrentImageSize();
}


void KPhotoBook::slotIncPreviewSizePopupAboutToShow()
{
    KPopupMenu* popup = m_actions->m_increasePreviewSize->popupMenu();
    popup->clear();

    int curPercent = Settings::imagePreviewSize() * 100 / Constants::SETTINGS_MAX_PREVIEW_SIZE;

    popup->insertItem("100 %", 100);

    if (curPercent < 75) {
        popup->insertItem("75 %", 75);
    }
    if (curPercent < 50) {
        popup->insertItem("50 %", 50);
    }
    if (curPercent < 25) {
        popup->insertItem("25 %", 25);
    }
    if (curPercent < 10) {
        popup->insertItem("10 %", 10);
    }
}


void KPhotoBook::slotDecPreviewSizePopupAboutToShow()
{
    KPopupMenu* popup = m_actions->m_decreasePreviewSize->popupMenu();
    popup->clear();

    int curPercent = Settings::imagePreviewSize() * 100 / Constants::SETTINGS_MAX_PREVIEW_SIZE;

    if (curPercent > 75) {
        popup->insertItem("75 %", 75);
    }
    if (curPercent > 50) {
        popup->insertItem("50 %", 50);
    }
    if (curPercent > 25) {
        popup->insertItem("25 %", 25);
    }
    if (curPercent > 10) {
        popup->insertItem("10 %", 10);
    }
}


void KPhotoBook::slotChangePreviewSizeActivated(int percent)
{
    //calculate the new size
    int newSize = Constants::SETTINGS_MAX_PREVIEW_SIZE * percent / 100;

    //but limit its lower boundry
    if (newSize < Constants::SETTINGS_MIN_PREVIEW_SIZE) {
        newSize = Constants::SETTINGS_MIN_PREVIEW_SIZE;
    }

    tracer->debug(__func__, "New preview size is %i pixels.", newSize);

    Settings::setImagePreviewSize(newSize);

    applyZoomSetting();

    m_view->updateCurrentImageSize();
}


void KPhotoBook::slotIncludeWholeSourceDir()
{
    m_sourcedirTree->includeWholeSourceDir();

    autoRefreshView();
}


void KPhotoBook::slotExcludeWholeSourceDir()
{
    m_sourcedirTree->excludeWholeSourceDir();

    autoRefreshView();
}


void KPhotoBook::slotInvertSourceDir()
{
    m_sourcedirTree->invertSourceDir();

    autoRefreshView();
}


void KPhotoBook::slotIncludeAllSourceDirs()
{
    m_sourcedirTree->includeAllSourceDirs();

    autoRefreshView();
}


void KPhotoBook::slotExcludeAllSourceDirs()
{
    m_sourcedirTree->excludeAllSourceDirs();

    autoRefreshView();
}


void KPhotoBook::slotInvertAllSourceDirs()
{
    m_sourcedirTree->invertAllSourceDirs();

    autoRefreshView();
}


void KPhotoBook::slotExpandSourceDir()
{
    m_sourcedirTree->expandCurrent();
}


void KPhotoBook::slotCollapseSourceDir()
{
    m_sourcedirTree->collapseCurrent();
}


void KPhotoBook::slotExpandAllSourceDirs()
{
    m_sourcedirTree->expandAll();
}


void KPhotoBook::slotCollapseAllSourceDirs()
{
    m_sourcedirTree->collapseAll();
}


void KPhotoBook::slotAndifyTags()
{
    int lastOperator = Settings::tagTreeFilterOperator();

    Settings::setTagTreeFilterOperator(Settings::EnumTagTreeFilterOperator::And);
    applyOperatorSetting();

    if (lastOperator != Settings::EnumTagTreeFilterOperator::And) {
        autoRefreshView();
    }
}


void KPhotoBook::slotOrifyTags()
{
    int lastOperator = Settings::tagTreeFilterOperator();

    Settings::setTagTreeFilterOperator(Settings::EnumTagTreeFilterOperator::Or);
    applyOperatorSetting();

    if (lastOperator != Settings::EnumTagTreeFilterOperator::Or) {
        autoRefreshView();
    }
}


void KPhotoBook::slotDeselectFilter()
{
    m_tagTree->deselectFilter();

    autoRefreshView();
}


void KPhotoBook::slotResetFilter()
{
    m_tagTree->resetFilter();

    autoRefreshView();
}


void KPhotoBook::slotExpandTag()
{
    m_tagTree->expandCurrent();
}


void KPhotoBook::slotCollapseTag()
{
    m_tagTree->collapseCurrent();
}


void KPhotoBook::slotExpandAllTags()
{
    m_tagTree->expandAll();
}


void KPhotoBook::slotCollapseAllTags()
{
    m_tagTree->collapseAll();
}


void KPhotoBook::slotFileSelectionChanged()
{
    unsigned int selectedImagesCount = m_view->fileView()->selectedItems()->count();

    // update the statusbar to reflect the number of selected files
    QString selectedMsg = QString(i18n("Selected: %1")).arg(selectedImagesCount);
    statusBar()->changeItem(selectedMsg, 4);

    // enable 'exportSelectedFiles' only if at least one file is selected
    m_actions->m_exportSelectedFiles->setEnabled(selectedImagesCount > 0);

    // update the sourcedirtree
    m_sourcedirTree->reflectSelectedFiles(m_view->fileView()->selectedItems());

    // update the tagtree
    m_tagTree->doRepaintAll();
}


void KPhotoBook::slotLoadSettings()
{
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


void KPhotoBook::slotConfigDefaultClicked()
{
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


void KPhotoBook::slotRestoreToolViews()
{
    m_tagTreeToolView->place(KDockWidget::DockLeft, getMainDockWidget(), 20);
    m_sourceDirTreeToolView->place(KDockWidget::DockCenter, m_tagTree, 20);
}


void KPhotoBook::slotShowToolViewTagTree()
{
    if (!m_sourcedirTree->isHidden()) {
        m_tagTreeToolView->place(KDockWidget::DockCenter, m_sourcedirTree, 20);

    } else {
        m_tagTreeToolView->place(KDockWidget::DockLeft, getMainDockWidget(), 20);
    }

    m_tagTreeToolView->show();
}

void KPhotoBook::slotShowToolViewSourceDirTree()
{
    if (!m_tagTree->isHidden()) {
        m_sourceDirTreeToolView->place(KDockWidget::DockCenter, m_tagTree, 20);

    } else {
        m_sourceDirTreeToolView->place(KDockWidget::DockLeft, getMainDockWidget(), 20);
    }

    m_sourceDirTreeToolView->show();
}


void KPhotoBook::slotExportMatchingFiles()
{
    QDir dir(Settings::fileSystemLastExportedToDirectory());

    QString choosedDir = selectExportingDirectory(dir.absPath());

    if (!choosedDir.isEmpty()) {
        Settings::setFileSystemLastExportedToDirectory(choosedDir);

        tracer->debug(__func__, "Exporting to '%s'...", choosedDir.ascii());
        changeStatusbar(i18n("Exporting symbolic links..."));

        ExportSymlinks exporter(m_view, choosedDir);
        exporter.setSourceFiles(m_view->fileView()->items());
        exporter.execute();

        changeStatusbar(i18n("Exporting symbolic links finished"));
    }
}


void KPhotoBook::slotExportSelectedFiles()
{
    QDir dir(Settings::fileSystemLastExportedToDirectory());

    QString choosedDir = selectExportingDirectory(dir.absPath());

    if (!choosedDir.isEmpty()) {

        Settings::setFileSystemLastExportedToDirectory(choosedDir);

        tracer->debug(__func__, "Exporting to '%s'...", choosedDir.ascii());
        changeStatusbar(i18n("Exporting symbolic links..."));

        ExportSymlinks exporter(m_view, choosedDir);
        exporter.setSourceFiles(m_view->fileView()->selectedItems());
        exporter.execute();

        changeStatusbar(i18n("Exporting symbolic links finished"));
    }
}


///@todo finish implementation of the imageImporter
void KPhotoBook::slotImportImages()
{
    (new ImageImporter(this))->show();
}


//
// private
//
void KPhotoBook::autoRefreshView()
{
    if (m_view && Settings::imagePreviewAutoRefresh()) {
        m_view->updateFiles();
    }
}


void KPhotoBook::changeStatusbar(const QString& text)
{
    // display the text on the statusbar
    statusBar()->changeItem(text, 1);
}


void KPhotoBook::setupToolWindowTagTree()
{
    QWidget* tagTreePanel = new QWidget(this, "tagTreePanel");
    QVBoxLayout* tagTreePanelLayout = new QVBoxLayout(tagTreePanel, 0, 0, "tagTreePanelLayout");
    tagTreePanelLayout->setAutoAdd(true);

    m_tagTreeToolBar = new KToolBar(tagTreePanel, "tagTreeToolBar", true, true);
    m_tagTreeToolBar->setIconSize(Settings::tagTreeToolBarIconSize());

    m_actions->m_addMaintag->plug(m_tagTreeToolBar);

    m_tagTreeToolBar->insertSeparator();

    m_actions->m_expandAllTags->plug(m_tagTreeToolBar);
    m_actions->m_collapseAllTags->plug(m_tagTreeToolBar);

    QWidget* spacer = new QWidget(m_tagTreeToolBar);
    QSizePolicy sizePolicy = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorStretch(99);
    spacer->setSizePolicy(sizePolicy);
    m_tagTreeToolBar->insertWidget(99999, 99999, spacer, -1);

    m_actions->m_andifyTags->plug(m_tagTreeToolBar);
    m_actions->m_orifyTags->plug(m_tagTreeToolBar);
    m_tagTreeToolBar->insertSeparator();
    m_actions->m_deselectFilter->plug(m_tagTreeToolBar);
    m_actions->m_resetFilter->plug(m_tagTreeToolBar);
    m_tagTreeToolBar->insertSeparator();
    m_actions->m_toggleLockUnlockTagging->plug(m_tagTreeToolBar);

    m_tagTree = new TagTree(tagTreePanel, this, "tagtree");

    // set the icon
    QIconSet iconSet = KGlobal::iconLoader()->loadIconSet(Constants::ICON_TAG, KIcon::Small, 16, true);
    if (iconSet.isNull()) {
        tracer->swarning(__func__) << "Could not load iconset with iconname: '" << Constants::ICON_TAG << "'" << endl;
    } else {
        tagTreePanel->setIcon(iconSet.pixmap());
    }

    // eventually do add the tool window
    m_tagTreeToolView = addToolWindow(tagTreePanel, KDockWidget::DockLeft, getMainDockWidget(), 20, i18n("Tags"), i18n("Tags"));
}


void KPhotoBook::setupToolWindowSourceDirTree()
{
    // create sourcedirtree toolwindow
    QWidget* sourceDirTreePanel = new QWidget(this, "sourceDirTreePanel");
    QVBoxLayout* sourceDirTreePanelLayout = new QVBoxLayout(sourceDirTreePanel, 0, 0, "sourceDirTreePanelLayout");
    sourceDirTreePanelLayout->setAutoAdd(true);

    m_sourceDirTreeToolBar = new KToolBar(sourceDirTreePanel, "sourceDirTreeToolBar", true, true);
    m_sourceDirTreeToolBar->setIconSize(Settings::sourceDirTreeToolBarIconSize());

    m_actions->m_addFolder->plug(m_sourceDirTreeToolBar);

    m_sourceDirTreeToolBar->insertSeparator();

    m_actions->m_expandAllFolders->plug(m_sourceDirTreeToolBar);
    m_actions->m_collapseAllFolders->plug(m_sourceDirTreeToolBar);

    QWidget* spacer = new QWidget(m_sourceDirTreeToolBar);
    QSizePolicy sizePolicy = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorStretch(99);
    spacer->setSizePolicy(sizePolicy);
    m_sourceDirTreeToolBar->insertWidget(99999, 99999, spacer, -1);

    m_actions->m_includeAllFolders->plug(m_sourceDirTreeToolBar);
    m_actions->m_excludeAllFolders->plug(m_sourceDirTreeToolBar);
    m_actions->m_invertAllFolderSelection->plug(m_sourceDirTreeToolBar);

    m_sourcedirTree = new SourceDirTree(sourceDirTreePanel, this, "sourcedirTree");

    // set the icon
    QIconSet iconSet = KGlobal::iconLoader()->loadIconSet(Constants::ICON_SOURCEDIR, KIcon::Small, 16, true);
    if (iconSet.isNull()) {
        tracer->swarning(__func__) << "Could not load iconset with iconname: '" << Constants::ICON_SOURCEDIR << "'" << endl;
    } else {
        sourceDirTreePanel->setIcon(iconSet.pixmap());
    }

    // eventually do add the tool window
    m_sourceDirTreeToolView = addToolWindow(sourceDirTreePanel, KDockWidget::DockCenter, m_tagTree, 20, i18n("Source directories"), i18n("Source"));
}


void KPhotoBook::updateState()
{
    // enable save if engine is dirty
    m_actions->m_save->setEnabled(m_engine->dirty());

    // display the opened file in the caption
    setCaption(currentURL(), m_engine->dirty());

    // update the statusbar
    updateStatusBar();
}


void KPhotoBook::updateStatusBar()
{
    // update the statusbar
    QString filesMsg = QString(i18n("Files: %1")).arg(m_engine->totalNumberOfFiles());
    statusBar()->changeItem(filesMsg, 2);
    QString filterMsg = QString(i18n("Matched: %1")).arg(m_engine->filteredNumberOfFiles());
    statusBar()->changeItem(filterMsg, 3);

    // I know, this is really not the right place to do this...
    // but it is the simplest way
    // enable the 'export matching files' only if at least 1 file is displayed
    m_actions->m_exportMatchingFiles->setEnabled(m_engine->filteredNumberOfFiles() > 0);
}


SourceDir* KPhotoBook::addSourceDir(QDir* sourceDir, bool recursive) throw(EngineException*)
{
    SourceDir* newSourceDir = m_engine->fileSystemScanner()->addSourceFolder(sourceDir, recursive);

    updateState();
    return newSourceDir;
}


void KPhotoBook::removeSourceDir(SourceDir* sourceDir)
{
    m_engine->fileSystemScanner()->removeSourceFolder(sourceDir);

    updateState();
}


TagNode* KPhotoBook::createTag(TagNode::Type type, const QString& name, const QString& comment, const QString& iconName, TagNode* parent)
{
    tracer->sinvoked(__func__) << "Invoked with type: " << type << ", name: " << name << ", icon: " << iconName << endl;

    TagNode* tagNode = m_engine->createTag(parent, type, name, comment, iconName);

    updateState();
    return tagNode;
}


void KPhotoBook::applyZoomSetting()
{
    m_actions->m_increasePreviewSize->setEnabled(Settings::imagePreviewSize() < Constants::SETTINGS_MAX_PREVIEW_SIZE);
    m_actions->m_decreasePreviewSize->setEnabled(Settings::imagePreviewSize() > Constants::SETTINGS_MIN_PREVIEW_SIZE);
}


void KPhotoBook::applyOperatorSetting()
{
    m_actions->m_andifyTags->setChecked(Settings::tagTreeFilterOperator() == Settings::EnumTagTreeFilterOperator::And);
    m_actions->m_orifyTags->setChecked(Settings::tagTreeFilterOperator() != Settings::EnumTagTreeFilterOperator::And);

    if (m_settingsTagTree) {
        m_settingsTagTree->kcfg_TagTreeFilterOperator->setCurrentItem(Settings::tagTreeFilterOperator());
    }
}


void KPhotoBook::applyAutorefreshSetting()
{
    m_actions->m_autoRefreshView->setChecked(Settings::imagePreviewAutoRefresh());

    if (m_settingsImagePreview) {
        m_settingsImagePreview->kcfg_ImagePreviewAutoRefresh->setChecked(Settings::imagePreviewAutoRefresh());
    }

    autoRefreshView();
}


void KPhotoBook::applyLockUnlockTaggingSettings()
{
    m_actions->m_toggleLockUnlockTagging->setChecked(Settings::tagTreeLocked());
    if (Settings::tagTreeLocked()) {
        m_actions->m_toggleLockUnlockTagging->setText("Unlock tagging");
        m_actions->m_toggleLockUnlockTagging->setIcon(Constants::ICON_TAG_LOCK);
    } else {
        m_actions->m_toggleLockUnlockTagging->setText("Lock tagging");
        m_actions->m_toggleLockUnlockTagging->setIcon(Constants::ICON_TAG_UNLOCK);
    }

    if (m_tagTree) {
        m_tagTree->doRepaintAll();
    }
}


void KPhotoBook::storeTreeState()
{
    KConfig* config = KGlobal::config();
    
    QString group = QString("TagTreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    if (Settings::tagTreeRememberTree()) {
        QStringList* openNodes = m_tagTree->getOpenNodes();
        config->writeEntry("OpenNodes", *openNodes);
        delete openNodes;
    }
    m_tagTree->saveLayout(config, group);
    
    group = QString("SourceDirTreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    if (Settings::sourceDirTreeRememberTree()) {
        QStringList* openNodes = m_sourcedirTree->getOpenNodes();
        config->writeEntry("OpenNodes", *openNodes);
        delete openNodes;
    }
    m_sourcedirTree->saveLayout(config, group);

    // force writing
    config->sync();
}


void KPhotoBook::loadTreeState()
{
    KConfig* config = KGlobal::config();
    
    QString group = QString("TagTreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    m_tagTree->restoreLayout(config, group);
    if (Settings::tagTreeRememberTree()) {
        QStringList openNodes = config->readListEntry("OpenNodes");
        m_tagTree->openNodes(&openNodes);
    }

    group = QString("SourceDirTreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    m_sourcedirTree->restoreLayout(config, group);
    if (Settings::sourceDirTreeRememberTree()) {
        QStringList openNodes = config->readListEntry("OpenNodes");
        m_sourcedirTree->openNodes(&openNodes);
    }
}

/**
 * Stores the filters set on the trees.
 */
void KPhotoBook::storeFilter()
{
    KConfig* config = KGlobal::config();
    
    QString group = QString("TagTreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    if (Settings::tagTreeRememberFilter()) {
        QIntDict<QString>* filterDict = m_tagTree->getFilter();

        QStringList* filterList = intDict2stringList(filterDict);

        config->writeEntry("Filter", *filterList);

        filterDict->setAutoDelete(true);
        delete filterDict;
        delete filterList;
    }

    group = QString("SourceDirTreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    if (Settings::sourceDirTreeRememberFilter()) {
        QIntDict<QString>* filterDict = m_sourcedirTree->getFilter();

        QStringList* filterList = intDict2stringList(filterDict);

        config->writeEntry("Filter", *filterList);

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
void KPhotoBook::loadFilter()
{
    KConfig* config = KGlobal::config();
    
    QString group = QString("TagTreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    if (Settings::tagTreeRememberFilter()) {
        QStringList filterList = config->readListEntry("Filter");

        QIntDict<QString>* filterDict = stringList2intDict(filterList);

        m_tagTree->applyFilter(filterDict);

        filterDict->setAutoDelete(true);
        delete filterDict;
    }

    group = QString("SourceDirTreeState:%1").arg(*(m_engine->uid()));
    config->setGroup(group);
    if (Settings::sourceDirTreeRememberFilter()) {
        QStringList filterList = config->readListEntry("Filter");

        QIntDict<QString>* filterDict = stringList2intDict(filterList);

        m_sourcedirTree->applyFilter(filterDict);

        filterDict->setAutoDelete(true);
        delete filterDict;
    }
}


QStringList* KPhotoBook::intDict2stringList(QIntDict<QString>* intDict)
{
    QStringList* stringList = new QStringList();

    QIntDictIterator<QString> it(*intDict);
    for (; it.current(); ++it) {
        QString entry = QString("%1:%2").arg(it.currentKey()).arg(*it.current());
        stringList->append(entry);
    }

    return stringList;
}


QIntDict<QString>* KPhotoBook::stringList2intDict(QStringList stringList)
{
    QIntDict<QString>* filterDict = new QIntDict<QString>;

    // loop over all entries in the stringlist
    for (QStringList::Iterator it = stringList.begin(); it != stringList.end(); ++it) {

        tracer->sdebug(__func__) << "Handling entry: '" << *it << "'" << endl;

        // split the current entry into key and value and put them into the intdict
        int delimitorPos = (*it).find(':');
        if (delimitorPos > 0) {
            QString keyStr = (*it).mid(0, delimitorPos);
            QString value = (*it).mid(delimitorPos + 1);

            tracer->sdebug(__func__) << "key-->value: '" << keyStr << "-->" << value << "'" << endl;

            bool ok;
            int key = keyStr.toInt(&ok);

            if (ok) {
                filterDict->insert(key, new QString(value));
            } else {
                tracer->swarning(__func__) << "Key '" << *it << "' is invalid! It is not a number. (Valid format: 'key:value')" << endl;
            }

        } else {
            tracer->swarning(__func__) << "KeyValue pair '" << *it << "' is invalid! (Valid format: 'key:value')" << endl;
        }
    }

    return filterDict;
}


QString KPhotoBook::selectExportingDirectory(QString startDirectory)
{
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

    // test if the chosen directory exists and is really a directory
    KFileItem fileItem(KFileItem::Unknown, KFileItem::Unknown, choosedDir);
    if (fileItem.isDir()) {
      delete dialog;
      return choosedDir;
    }

    // if we got here the chosen directory is invalid
    KMessageBox::sorry(this, i18n("You must select an existing directory."), i18n("Export"));
  }
}


#include "kphotobook.moc"
