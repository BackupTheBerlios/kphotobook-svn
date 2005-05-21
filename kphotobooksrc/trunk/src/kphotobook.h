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

#ifndef _KPHOTOBOOK_H_
#define _KPHOTOBOOK_H_

#include "settings/settings.h"
#include "exception.h"
#include "engine/tagnode.h"
#include "tracer/tracer.h"
#include "engine/filternode.h"

#include <kaction.h>
#include <kapplication.h>
#include <kpopupmenu.h>

#include <kmdimainfrm.h>
class KToolBar;
class KListView;

#include <qdir.h>
#include <qstringlist.h>
#include <qintdict.h>

#include <kactionclasses.h>
#include <kurl.h>

class Engine;

class SettingsGeneral;
class SettingsTagTree;
class SettingsSourceDirTree;
class SettingsImagePreview;
class SettingsFileHandling;
class SettingsTools;

class KPhotoBookView;
class SourceDir;
class File;
class TagNode;
class TagTreeNode;
class EngineException;

class TagTree;
class SourceDirTree;

/**
 * This class serves as the main window for KPhotoBook.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author $AUTHOR <$EMAIL>
 * @version $APP_VERSION
 */
class KPhotoBook : public KMdiMainFrm {

    Q_OBJECT

private:
    static Tracer* tracer;

public:
    /**
     * Creates a new photobook mainwindow.
     */
    KPhotoBook(KMdi::MdiMode mdiMode = KMdi::IDEAlMode);

    /**
     * Default Destructor
     */
    virtual ~KPhotoBook();

    /**
     * Returns the kphotobook-view of this kphotobook.
     */
    KPhotoBookView* view() {
        return m_view;
    }

    TagTree* tagTree() {
        return m_tagTree;
    }

    SourceDirTree* sourceDirTree() {
        return m_sourcedirTree;
    }

    void dirtyfy();

    /**
     * This method is invoked for temporary unlocking the tagging.
     * As soon as this method is invoked, tagging is possible, even
     * it was lacked.
     */
    void startTemporaryUnlockTagging() {

        m_inTagtreeTemporaryUnlocking = true;

        m_tagtreeWasLocked = Settings::tagTreeLocked();
        Settings::setTagTreeLocked(false);
        applyLockUnlockTaggingSettings();
    }

    /**
     * This method must be invoked as soon, as the temporary unlocking must end.
     * When this method has finished, tagging is no longer possible, if tagging
     * was locked before the method 'startTemporaryUnlockTagging' was called.
     *
     * Sometimes this method is called even we are not in temporaryr unlocking
     * mode. Thus the field 'm_inTagtreeTemporaryUnlocking' was introduced
     * to get things working correct.
     */
    void stopTemporaryUnlockTagging() {

        if (m_inTagtreeTemporaryUnlocking) {
          m_inTagtreeTemporaryUnlocking = false;

          Settings::setTagTreeLocked(m_tagtreeWasLocked);
          applyLockUnlockTaggingSettings();
        }
    }

    /**
     * Tests if the specified tagtext is valid. There must not exist a sibling
     * with the same name.
     * If the parent is null, it is considered to be a maintag.
     */
    bool isTagTextValid(TagTreeNode* parent, QString& text);

    /**
     * Use this method to load whatever file/URL you have
     */
    void load(QFileInfo& fileinfo);

    /**
     * Returns the current opened file or an empty string if the
     * was never saved.
     */
    QString currentURL();

    /**
     * Returns all currently added source directories handled by the engine.
     */
    QPtrList<SourceDir>* sourceDirs();

    /**
     * Returns all possible tags of all files.
     */
    QPtrList<TagNode>* tagForest();

    /**
     * Returns all files matching the specified filter.
     * If the filter is 0, it is build from the selection in the tagtree.
     */
    QPtrList<File>* files(FilterNode *filterRootNode);

    /**
     * Refreshes the view only if autorefresh is enabled.
     */
    void autoRefreshView();

    KPopupMenu* contextMenuSourceDirTree() {
        return m_contextMenuSourceDirTree;
    }

    KPopupMenu* contextMenuSourceDir() {
        return m_contextMenuSourceDir;
    }

    KPopupMenu* contextMenuSubDir() {
        return m_contextMenuSubDir;
    }

    KPopupMenu* contextMenuTagTree() {
        return m_contextMenuTagTree;
    }

    KPopupMenu* contextMenuTagTreeItem() {
        return m_contextMenuTagTreeItem;
    }

    KPopupMenu* contextMenuTagTreeItemLeaf() {
        return m_contextMenuTagTreeItemLeaf;
    }

    /**
     * Enables or disables locking of tagging.
     */
    void applyLockUnlockTaggingSettings();

protected:
    /**
     * This method is called after close is called on the instance.
     * It is also called when the user closes the window with the close icon.
     */
    bool queryClose();

    /**
     * This method is called after queryClose() has returned with true and this is the last window.
     */
    bool queryExit();

private slots:
    void slotFileNew();
    void slotFileOpen(const KURL& = KURL());
    bool slotFileSave();
    bool slotFileSaveAs();

    void slotOptionsConfigureKeys();
    void slotOptionsConfigureToolbars();
    void slotOptionsPreferences();

    void slotAddSourcedir();
    void slotEditSourceDir();
    void slotRemoveSourceDir();

    void slotAddMaintag();
    void slotCreateSubtag();
    void slotEditTag();
    void slotDeleteTag();

    void slotToggleLockUnlockTagging();

    void slotRescanFilesystem();

    void slotAutoRefreshView();
    void slotRefreshView();

    void slotIncludeWholeSourceDir();
    void slotExcludeWholeSourceDir();
    void slotInvertSourceDir();
    void slotIncludeAllSourceDirs();
    void slotExcludeAllSourceDirs();
    void slotInvertAllSourceDirs();

    void slotExpandSourceDir();
    void slotCollapseSourceDir();
    void slotExpandAllSourceDirs();
    void slotCollapseAllSourceDirs();

    void slotAndifyTags();
    void slotOrifyTags();

    void slotDeselectFilter();
    void slotResetFilter();

    void slotExpandTag();
    void slotCollapseTag();
    void slotExpandAllTags();
    void slotCollapseAllTags();

    void slotIncreasePreviewSize();
    void slotDecreasePreviewSize();

    void slotIncPreviewSizePopupAboutToShow();
    void slotDecPreviewSizePopupAboutToShow();

    void slotChangePreviewSizeActivated(int percent);

    void slotFileSelectionChanged();

    void changeStatusbar(const QString& text);

    void slotLoadSettings();
    void slotConfigDefaultClicked();

    void slotRestoreToolViews();
    void slotShowToolViewTagTree();
    void slotShowToolViewSourceDirTree();
    void slotShowToolViewMetaInfoTree();

    void slotExportMatchingFiles();
    void slotExportSelectedFiles();

private:
    void init();
    void setupAccel();
    void setupActions();
    void setupContextMenus();

    void setupToolWindowTagTree();
    void setupToolWindowSourceDirTree();
    void setupToolWindowMetaInfoTree();

    void updateState();
    void updateStatusBar();

    bool checkForUntagged();

    /**
     * Adds the specified sourcedir and all images in it to the engine.
     * If recursive is true, all directories below the sourcedir are added too.
     * If the sourcedir cannot be added, an EngineException is thrown.
     */
    SourceDir* addSourceDir(QDir* sourceDir, bool recursive) throw(EngineException*);

    /**
     * Removes the specified sourceDir and deletes all Files and associations
     * below this sourceDir.
     */
    void removeSourceDir(SourceDir* sourceDir);

    /**
     * Creates a new tag with the specified parameters.
     * If parent is not null, the new tag is a child of the specified parent.
     */
    TagNode* createTag(TagNode::Type type, const QString& name, const QString& comment, const QString& iconName, TagNode* parent = 0);

    /**
     * Enables or disables the zoomin and zoomout function depending on the
     * current preview size of the images.
     */
    void applyZoomSetting();

    /**
     * Activates or deactivates the operator (and / or) actions.
     */
    void applyOperatorSetting();

    /**
     * Activates or deactivates the autorefresh actions.
     */
    void applyAutorefreshSetting();

    /**
     * Stores the state (open/closed nodes) of the trees.
     */
    void storeTreeState();

    /**
     * Loads the state (open/closed nodes) of the trees.
     */
    void loadTreeState();

    /**
     * Stores the filters set on the trees.
     */
    void storeFilter();

    /**
     * Loads the filters and sets them on the trees.
     */
    void loadFilter();

    /**
     * Builds a list of strings containing as many entries as the specified
     * int dictionary. Each entry of the dictionary is joined into one string.
     * The string is built by the int key followed by a colon and the value.
     * This helper method is used because the class KConfig or KConfigBase resp.
     * do not support writing/reading int dictionaries.
     */
    QStringList* intDict2stringList(QIntDict<QString>* intDict);

    /**
     * Builds a int dictionary with the entries found in the specified stringlist.
     * The entries in the stringlist must begin with an int followed by a colon and
     * an alphanumeric string: <int>:<alphanumeric>.
     * This helper method is used because the class KConfig or KConfigBase resp.
     * do not support writing/reading int dictionaries.
     */
    QIntDict<QString>* stringList2intDict(QStringList stringList);

    /**
     * Lets the user choose an existing directory.
     * If the user aborts QString::null is returned otherwise an
     * existing and valid directory name is returned.
     *
     * @param startDirectory The directory to display as default.
     * @return The selected directory or QString::null.
     */
    QString selectExportingDirectory(QString startDirectory);

private:
    KPhotoBookView* m_view;

    TagTree* m_tagTree;
    SourceDirTree* m_sourcedirTree;
    KListView* m_metaInfoTree;

    KMdiToolViewAccessor* m_tagTreeToolView;
    KMdiToolViewAccessor* m_sourceDirTreeToolView;
    KMdiToolViewAccessor* m_metaInfoTreeToolView;

    Engine* m_engine;

    /**
     * Truze whiel temporary unlocking is enabled.
     */
    bool m_inTagtreeTemporaryUnlocking;

    /**
     * Is true if tagging was locked before the method
     * startTemporaryUnlockTagging was invoked.
     */
    bool m_tagtreeWasLocked;

    // toolbars above the trees
    KToolBar* m_tagTreeToolBar;
    KToolBar* m_sourceDirTreeToolBar;

    // actions
    KToggleAction* m_autoRefreshViewAction;
    KToolBarPopupAction* m_zoomIn;
    KToolBarPopupAction* m_zoomOut;

    KRecentFilesAction* m_recentFilesAction;

    KAction* m_save;
    KAction* m_exportMatchingFiles;
    KAction* m_exportSelectedFiles;
    KToggleAction* m_andifyTagsAction;
    KToggleAction* m_orifyTagsAction;
    KToggleAction* m_lockUnlockTaggingAction;

    // contextMenus
    KPopupMenu* m_contextMenuSourceDirTree;
    KPopupMenu* m_contextMenuSourceDir;
    KPopupMenu* m_contextMenuSubDir;
    KPopupMenu* m_contextMenuTagTree;
    KPopupMenu* m_contextMenuTagTreeItem;
    KPopupMenu* m_contextMenuTagTreeItemLeaf;

    // settings pages
    SettingsGeneral* m_settingsGeneral;
    SettingsImagePreview* m_settingsImagePreview;
    SettingsTagTree* m_settingsTagTree;
    SettingsSourceDirTree* m_settingsSourceDirTree;
    SettingsFileHandling* m_settingsFileHandling;
    SettingsTools* m_settingsTools;

};

#endif // _KPHOTOBOOK_H_
