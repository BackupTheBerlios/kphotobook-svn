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

#include <kfile.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>

/**
 * This class stores the configuration of the application kphotobook.
 *
 * CVS-ID $Id: configuration.h,v 1.5 2004/03/23 22:19:39 starcube Exp $
 */
class Configuration {

public:
    // preview size
    static const int MIN_PREVIEW_SIZE; /**< The minimum image preview size. */
    static const int DEFAULT_PREVIEW_SIZE; /**< The default image preview size. */
    static const int MAX_PREVIEW_SIZE; /**< The maximum image preview size. */

    // tagtree icon size
    static const int MIN_TAGTREEICON_SIZE; /**< The minimum size for the icons in the tagtree. */
    static const int DEFAULT_TAGTREEICON_SIZE; /**< The default size for the icons in the tagtree. */
    static const int MAX_TAGTREEICON_SIZE; /**< The maximum size for the icons in the tagtree. */

    // selection mode
    static const QString DEFAULT_FILEPREVIEW_SELECTIONMODE; /**< The default selectionmode of the filepreview is 'Extended'. */
    static const QString FILEPREVIEW_SELECTIONMODE_MULTI; /**< The string representation of the selectionmode 'Multi'. */
    static const QString FILEPREVIEW_SELECTIONMODE_EXTENDED; /**< The string representation of the selectionmode 'Multi'. */


    // auto refresh default
    static const bool DEFAULT_AUTOREFRESH; /**< The default value for the autorefresh feature. */

    // directories
    static const QString GROUP_DIRECTORIES; /**< The name of the configuration group containing directory settings. */
    static const QString CONFIG_ENTRY_LASTOPENEDFILE; /**< This setting stores the last opened kphotobook file. */
    static const QString CONFIG_ENTRY_LASTADDEDSOURCEDIR; /**< This setting stores the location of the last added source directory. */

    // display settings
    static const QString GROUP_DISPLAYSETTINGS; /**< The name of the configuration group containing display settings. */
    static const QString CONFIG_ENTRY_TAGTREEICONSIZE; /**< This setting stores the iconsize of the icons in the tagtree. */
    static const QString CONFIG_ENTRY_MAINSPLITTERSIZES;
    static const QString CONFIG_ENTRY_TREESPLITTERSIZES;

    // file preview
    static const QString GROUP_FILEPREVIEW; /**< The name of the configuration group containing filepreview settings. */
    static const QString CONFIG_ENTRY_PREVIEWSIZE; /**< This setting contains the previewsize of the images. */
    static const QString CONFIG_ENTRY_AUTOREFRESH; /**< This setting defines if the view is automatically refreshed after the user changed something. */
    static const QString CONFIG_ENTRY_SELECTIONMODE; /**< This setting contains the selection mode for the image preview. */

    // filter settings
    static const QString GROUP_FILTERS; /**< The name of the configuration group containing filter settings. */
    static const QString CONFIG_ENTRY_SUBDIRSTOIGNORE; /**< This setting contains the list of patterns for directorynames to ignore. */
    static const QString CONFIG_ENTRY_FILETYPESTOHANDLE; /**< This setting contains the list of patterns for filenames to handle. */
    static const QString CONFIG_ENTRY_TAGFILTEROPERATOR; /**< This setting contains the last used operator for the tagfilter (& or |). */

public:
    /**
     * Returns the singleton instance of this configuration class.
     * @return The singleton instance of this configuration class.
     */
    static Configuration* getInstance() {

        static Configuration* m_configuration = new Configuration();

        return m_configuration;
    }

public:
    /**
     * Reads the configuration from the default configuration file of this
     * application and sets the fields of the configuration instance.
     * The configuration is traced with the method trace after it is loaded.
     * @see trace()
     */
    void load();

    /**
     * Validates all values stored in this configuration instance and changes
     * invalid values to the defaults.
     */
    void validate();

    /**
     * Stores the settings of this configuration instance to the default
     * configuration file of this application.
     * The configuration is traced with the method trace() after it is stored.
     * @see trace()
     */
    void store();

    /**
     * Sets the last opened kphotobook file. This file will be opened automatically
     * when the application is started the next time.
     * @param lastOpenedFile The file to open the next time the application is started.
     */
    void setLastOpenedFile(QString lastOpenedFile) {
        m_lastOpenedFile = lastOpenedFile;
    }
    /**
     * Returns the last opened file.
     * @return The last opened file.
     */
    QString& lastOpenedFile() {
        return m_lastOpenedFile;
    }

    /**
     * Sets the last added source directory. When adding another source directory the
     * dialog will use this directory as proposal.
     * @param lastAddedSourcedir The last added source directory.
     */
    void setLastAddedSourcedir(QString lastAddedSourcedir) {
        m_lastAddedSourcedir = lastAddedSourcedir;
    }
    /**
     * Returns the last added source directory.
     * @return The last added source directory.
     */
    QString& lastAddedSourcedir() {
        return m_lastAddedSourcedir;
    }

    /*
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
*/
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
/*
    void setAutoRefresh(bool autoRefresh) {
        m_autoRefresh = autoRefresh;
    }
    bool autoRefresh() {
        return m_autoRefresh;
    }

    void setSelectionMode(KFile::SelectionMode selectionMode) {
        m_selectionMode = selectionMode;
    }
    KFile::SelectionMode selectionMode() {
        return m_selectionMode;
    }
*/
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

    /**
     * Sets the operator used when filtering the files.
     * The operator must be '&' or '|'.
     * @param op The operator used when filtering the files.
     */
     /*
    void setTagfilterOperator(QString op) {
        m_tagfilterOperator = op;
    }
    */
    /**
     * Returns the current set operator for the filter.
     * @return The current set operator for the filter.
     */
     /*
    QString tagfilterOperator() {
        return m_tagfilterOperator;
    }
    */
    /**
     * Inverts the current set operator.
     * If currently '&' is set it is changed to '|' and vice versa.
     */
     /*
    void invertTagfilterOperation() {
        if (m_tagfilterOperator == "&") {
            m_tagfilterOperator = "|";
        } else {
            m_tagfilterOperator = "&";
        }
    }
    */

protected:
    /**
     * Protected default constructor of this singleton configuration instance.
     * It loads the settings from the configuration file and validates the values.
     */
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
    bool m_autoRefresh;
    KFile::SelectionMode m_selectionMode;

    QStringList m_subdirsToIgnore;
    QStringList m_filetypesToHandle;
    QString m_tagfilterOperator;

    /**
     * Dumps the whole configuration settings with level debug to the standardout.
     */
    void trace();

    /**
     * Creates a comma separated string of the int-values.
     * @param valueList The integer list to create a string from.
     * @return The string with the comma separated int-values.
     */
    QString toString(QValueList<int> valueList);
};

#endif
