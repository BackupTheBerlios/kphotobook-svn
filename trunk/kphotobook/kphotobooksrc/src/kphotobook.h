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

#include "exception.h"

#include <kaction.h>
#include <kapplication.h>
#include <kpopupmenu.h>

#include <kmdimainfrm.h>

#include <qdir.h>

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
     * If the filter is QString::null, the filter is build from the selection in the tagtree.
     */
    QPtrList<File>* files(QString filter);

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
    void slotFileOpen();
    bool slotFileSave();
    bool slotFileSaveAs();

    void slotOptionsConfigureKeys();
    void slotOptionsConfigureToolbars();
    void slotOptionsPreferences();

    void slotNewToolbarConfig();

    void slotAddSourcedir();
    void slotEditSourceDir();
    void slotRemoveSourceDir();

    void slotAddMaintag();
    void slotCreateSubtag();
    void slotEditTag();
    void slotDeleteTag();

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

    void slotFileSelectionChanged();

    void changeStatusbar(const QString& text);

    void slotLoadSettings();

private:
    void init();
    void setupAccel();
    void setupActions();
    void setupWhatsThis();
    void setupContextMenus();

    void setupToolWindowTagTree();
    void setupToolWindowSourceDirTree();

    void updateState();
    void updateStatusBar();

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
    TagNode* createTag(int type, const QString& name, const QString& iconName, TagNode* parent = 0);

    /**
     * Enables or disables the zoomin and zoomout function depending on the
     * current preview size of the images.
     */
    void applyZoomSetting();

    /**
     * Aktivates or deactivates the operator (and / or) actions.
     */
    void applyOperatorSetting();

    /**
     * Aktivates or deactivates the autorefresh actions.
     */
    void applyAutorefreshSetting();

private:
    KPhotoBookView* m_view;

    TagTree* m_tagTree;
    SourceDirTree* m_sourcedirTree;

    Engine* m_engine;

    // actions
    KToggleAction* m_autoRefreshViewAction;
    KAction* m_zoomIn;
    KAction* m_zoomOut;
    KAction* m_save;
    KToggleAction* m_andifyTagsAction;
    KToggleAction* m_orifyTagsAction;

    // contextMenus
    KPopupMenu* m_contextMenuSourceDirTree;
    KPopupMenu* m_contextMenuSourceDir;
    KPopupMenu* m_contextMenuSubDir;
    KPopupMenu* m_contextMenuTagTree;
    KPopupMenu* m_contextMenuTagTreeItem;

    // settings pages
    SettingsGeneral* m_settingsGeneral;
    SettingsImagePreview* m_settingsImagePreview;
    SettingsTagTree* m_settingsTagTree;
    SettingsSourceDirTree* m_settingsSourceDirTree;
    SettingsFileHandling* m_settingsFileHandling;
    SettingsTools* m_settingsTools;

};

#endif // _KPHOTOBOOK_H_
