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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>

/**
 * This class stores the configuration of the application.
 *
 * CVS-ID $Id$
 */
class Configuration {

public:
    // preview size
    static const int MIN_PREVIEW_SIZE;
    static const int DEFAULT_PREVIEW_SIZE;
    static const int MAX_PREVIEW_SIZE;

    // tagtree icon size
    static const int MIN_TAGTREEICON_SIZE;
    static const int DEFAULT_TAGTREEICON_SIZE;
    static const int MAX_TAGTREEICON_SIZE;

    // selection mode
    static const int DEFAULT_FILEPREVIEW_SELECTIONMODE;

    // auto refresh default
    static const bool DEFAULT_AUTOREFRESH;

    // directories
    static const QString GROUP_DIRECTORIES;
    static const QString CONFIG_ENTRY_LASTOPENEDFILE;
    static const QString CONFIG_ENTRY_LASTADDEDSOURCEDIR;

    // display settings
    static const QString GROUP_DISPLAYSETTINGS;
    static const QString CONFIG_ENTRY_TAGTREEICONSIZE;
    static const QString CONFIG_ENTRY_MAINSPLITTERSIZES;
    static const QString CONFIG_ENTRY_TREESPLITTERSIZES;

    // file preview
    static const QString GROUP_FILEPREVIEW;
    static const QString CONFIG_ENTRY_PREVIEWSIZE;
    static const QString CONFIG_ENTRY_SELECTIONMODE;
    static const QString CONFIG_ENTRY_AUTOREFRESH;

    // filter settings
    static const QString GROUP_FILTERS;
    static const QString CONFIG_ENTRY_SUBDIRSTOIGNORE;
    static const QString CONFIG_ENTRY_FILETYPESTOHANDLE;
    static const QString CONFIG_ENTRY_TAGFILTEROPERATOR;

public:
    /**
     * Returns the singleton instance of this configuration class.
     */
    static Configuration* getInstance() {

        static Configuration* m_configuration = new Configuration();

        return m_configuration;
    }

    void load();
    void validate();
    void store();

    void setLastOpenedFile(QString lastOpenedFile) {
        m_lastOpenedFile = lastOpenedFile;
    }
    QString& lastOpenedFile() {
        return m_lastOpenedFile;
    }

    void setLastAddedSourcedir(QString lastAddedSourcedir) {
        m_lastAddedSourcedir = lastAddedSourcedir;
    }
    QString& lastAddedSourcedir() {
        return m_lastAddedSourcedir;
    }

    void setPreviewSize(int previewSize) {
        m_previewSize = previewSize;
        validate();
    }
    int previewSize() {
        return m_previewSize;
    }

    void setTagtreeIconSize(int tagtreeIconSize) {
        m_tagtreeIconSize = tagtreeIconSize;
        validate();
    }
    int tagtreeIconSize() {
        return m_tagtreeIconSize;
    }

    void setMainSplitterSizes(QValueList<int> splitterSizes) {
        m_mainSplitterSizes = splitterSizes;
    }
    QValueList<int>& mainSplitterSizes() {
        return m_mainSplitterSizes;
    }

    void setTreeSplitterSizes(QValueList<int> splitterSizes) {
        m_treeSplitterSizes = splitterSizes;
    }
    QValueList<int>& treeSplitterSizes() {
        return m_treeSplitterSizes;
    }

    void setFileTreeSelectionMode(int filetreeSelectionMode) {
        m_filetreeSelectionMode = filetreeSelectionMode;
        validate();
    }
    int fileTreeSelectionMode() {
        return m_filetreeSelectionMode;
    }

    void setAutoRefresh(bool autoRefresh) {
        m_autoRefresh = autoRefresh;
    }
    bool autoRefresh() {
        return m_autoRefresh;
    }

    void setSubdirsToIgnore(QStringList subdirsToIgnore) {
        m_subdirsToIgnore = subdirsToIgnore;
    }
    QStringList& subdirsToIgnore() {
        return m_subdirsToIgnore;
    }

    void setFiletypesToHandle(QStringList filetypesToHandle) {
        m_filetypesToHandle = filetypesToHandle;
    }
    QStringList& filetypesToHandle() {
        return m_filetypesToHandle;
    }

    void setTagfilterOperator(QString op) {
        m_tagfilterOperator = op;
    }
    QString tagfilterOperator() {
        return m_tagfilterOperator;
    }
    void invertTagfilterOperation() {
        if (m_tagfilterOperator == "&") {
            m_tagfilterOperator = "|";
        } else {
            m_tagfilterOperator = "&";
        }
    }

protected:
    Configuration() {
        load();
        validate();
    }

    ~Configuration() {
    }

private:
    QString m_lastOpenedFile;
    QString m_lastAddedSourcedir;

    int m_tagtreeIconSize;
    QValueList<int> m_mainSplitterSizes;
    QValueList<int> m_treeSplitterSizes;

    int m_previewSize;
    int m_filetreeSelectionMode;
    bool m_autoRefresh;

    QStringList m_subdirsToIgnore;
    QStringList m_filetypesToHandle;
    QString m_tagfilterOperator;

    // whatever
    void trace();
    QString toString(QValueList<int>);
};

#endif
