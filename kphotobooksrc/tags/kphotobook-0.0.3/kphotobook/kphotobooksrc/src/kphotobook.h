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
#include <kmainwindow.h>
#include <kpopupmenu.h>

#include <qdir.h>

class Engine;
class Configuration;
class KPhotoBookView;
class SourceDir;
class File;
class TagNode;
class EngineException;

/**
 * This class serves as the main window for KPhotoBook.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author $AUTHOR <$EMAIL>
 * @version $APP_VERSION
 */
class KPhotoBook : public KMainWindow {

    Q_OBJECT

public:
    /**
     * Creates a new photobook mainwindow.
     */
    KPhotoBook();

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

    Configuration* configuration() {
        return m_configuration;
    }

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
     */
    QPtrList<File>* files(QPtrList<TagNode>* filter);

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

    void slotIncludeWholeFolder();
    void slotExcludeWholeFolder();
    void slotInvertFolderSelection();

    void slotIncludeAllSourceDirs();
    void slotExcludeAllSourceDirs();
    void slotInvertAllSourceDirs();

    void slotExpandFolder();
    void slotCollapseFolder();

    void slotExpandAllFolder();
    void slotCollapseAllFolder();

    void slotFileSelectionChanged();

    void changeStatusbar(const QString& text);

private:
    void init();
    void setupAccel();
    void setupActions();
    void setupContextMenus();

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

private:
    KPhotoBookView* m_view;

    Engine* m_engine;
    Configuration* m_configuration;

    // actions
    KAction* m_save;

    // contextMenus
    KPopupMenu* m_contextMenuSourceDirTree;
    KPopupMenu* m_contextMenuSourceDir;
    KPopupMenu* m_contextMenuSubDir;
    KPopupMenu* m_contextMenuTagTree;
    KPopupMenu* m_contextMenuTagTreeItem;

};

#endif // _KPHOTOBOOK_H_
