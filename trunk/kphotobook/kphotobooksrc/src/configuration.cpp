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

#include "configuration.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kicontheme.h>
#include <kfile.h>

#include <kdebug.h>


//
// initialize constants
//
const int Configuration::MIN_PREVIEW_SIZE = 32;
const int Configuration::DEFAULT_PREVIEW_SIZE = 64;
const int Configuration::MAX_PREVIEW_SIZE = 160;

const int Configuration::MIN_TAGTREEICON_SIZE = KIcon::SizeSmall;
const int Configuration::DEFAULT_TAGTREEICON_SIZE = 22;
const int Configuration::MAX_TAGTREEICON_SIZE = KIcon::SizeHuge;

const int Configuration::DEFAULT_FILEPREVIEW_SELECTIONMODE = KFile::Multi;

const bool Configuration::DEFAULT_AUTOREFRESH = false;

const QString Configuration::GROUP_DIRECTORIES = QString("Directories");
const QString Configuration::CONFIG_ENTRY_LASTOPENEDFILE = QString("LastOpenedFile");
const QString Configuration::CONFIG_ENTRY_LASTADDEDSOURCEDIR = QString("LastAddedSourcedir");

const QString Configuration::GROUP_DISPLAYSETTINGS = QString("DisplaySettings");
const QString Configuration::CONFIG_ENTRY_TAGTREEICONSIZE = QString("TagtreeIconSize");
const QString Configuration::CONFIG_ENTRY_MAINSPLITTERSIZES = QString("MainSplitterSizes");
const QString Configuration::CONFIG_ENTRY_TREESPLITTERSIZES = QString("TreeSplitterSizes");

const QString Configuration::GROUP_FILEPREVIEW = QString("FilePreview");
const QString Configuration::CONFIG_ENTRY_PREVIEWSIZE = QString("PreviewSize");
const QString Configuration::CONFIG_ENTRY_SELECTIONMODE = QString("SelectionMode");
const QString Configuration::CONFIG_ENTRY_AUTOREFRESH = QString("AutoRefresh");

const QString Configuration::GROUP_FILTERS = QString("Filters");
const QString Configuration::CONFIG_ENTRY_SUBDIRSTOIGNORE = QString("SubdirsToIgnore");
const QString Configuration::CONFIG_ENTRY_FILETYPESTOHANDLE = QString("FiletypesToHandle");
const QString Configuration::CONFIG_ENTRY_TAGFILTEROPERATOR = QString("TagfilterOperator");


//
// public methods
//
void Configuration::load() {

    kdDebug() << "[Configuration::load] loading configuration..." << endl;

    KConfig* conf = kapp->config();

    // read directories
    conf->setGroup(GROUP_DIRECTORIES);
    m_lastOpenedFile = conf->readEntry(CONFIG_ENTRY_LASTOPENEDFILE, "");
    m_lastAddedSourcedir = conf->readEntry(CONFIG_ENTRY_LASTADDEDSOURCEDIR, "");

    // read display settings
    conf->setGroup(GROUP_DISPLAYSETTINGS);
    m_previewSize = conf->readNumEntry(CONFIG_ENTRY_PREVIEWSIZE, DEFAULT_PREVIEW_SIZE);
    m_tagtreeIconSize = conf->readNumEntry(CONFIG_ENTRY_TAGTREEICONSIZE, DEFAULT_TAGTREEICON_SIZE);
    m_mainSplitterSizes = conf->readIntListEntry(CONFIG_ENTRY_MAINSPLITTERSIZES);
    m_treeSplitterSizes = conf->readIntListEntry(CONFIG_ENTRY_TREESPLITTERSIZES);

    // read filepreview settings
    conf->setGroup(GROUP_FILEPREVIEW);
    m_previewSize = conf->readNumEntry(CONFIG_ENTRY_PREVIEWSIZE, DEFAULT_PREVIEW_SIZE);
    m_filetreeSelectionMode = conf->readNumEntry(CONFIG_ENTRY_SELECTIONMODE, DEFAULT_FILEPREVIEW_SELECTIONMODE);
    m_autoRefresh= conf->readBoolEntry(CONFIG_ENTRY_AUTOREFRESH, DEFAULT_AUTOREFRESH);

    // read filters
    conf->setGroup(GROUP_FILTERS);
    m_subdirsToIgnore = conf->readListEntry(CONFIG_ENTRY_SUBDIRSTOIGNORE);
    m_filetypesToHandle = conf->readListEntry(CONFIG_ENTRY_FILETYPESTOHANDLE);
    m_tagfilterOperator = conf->readEntry(CONFIG_ENTRY_TAGFILTEROPERATOR);

    conf->setGroup(QString::null);

    trace();
};


void Configuration::validate() {

    kdDebug() << "[Configuration::validate] validating configuration..." << endl;

    // adjust preview size read from config if it is invalid
    if (m_previewSize < MIN_PREVIEW_SIZE || m_previewSize > MAX_PREVIEW_SIZE) {
        m_previewSize = DEFAULT_PREVIEW_SIZE;
    }

    // adjust tagtree icon size read from config if it is invalid
    if (m_tagtreeIconSize < MIN_TAGTREEICON_SIZE || m_tagtreeIconSize > MAX_TAGTREEICON_SIZE) {
        m_tagtreeIconSize = DEFAULT_TAGTREEICON_SIZE;
    }

    // adjust selection mode
    if (m_filetreeSelectionMode != KFile::Multi && m_filetreeSelectionMode != KFile::Extended) {
        m_filetreeSelectionMode = DEFAULT_FILEPREVIEW_SELECTIONMODE;
    }

    // set the default directories to ignore if none were set in the config
    if (m_subdirsToIgnore.empty()) {
        m_subdirsToIgnore.append("thumbs");
        m_subdirsToIgnore.append("Thumbs");
        m_subdirsToIgnore.append("thumbnails");
        m_subdirsToIgnore.append("Thumbnails");
        m_subdirsToIgnore.append("ThumbNails");
    }

    // set the default filetypes to handle if none were set in the config
    if (m_filetypesToHandle.empty()) {
        m_filetypesToHandle.append(".jpg");
        m_filetypesToHandle.append(".jpeg");
    }

    // set the default operator if it is invalid
    if (m_tagfilterOperator != "&" && m_tagfilterOperator != "|") {
        m_tagfilterOperator = "&";
    }
}


void Configuration::store() {

    kdDebug() << "[Configuration::store] storing configuration..." << endl;
    trace();

    KConfig *conf=kapp->config();

    // store directories
    conf->setGroup(GROUP_DIRECTORIES);
    conf->writeEntry(CONFIG_ENTRY_LASTOPENEDFILE, m_lastOpenedFile);
    conf->writeEntry(CONFIG_ENTRY_LASTADDEDSOURCEDIR, m_lastAddedSourcedir);

    // store display settings
    conf->setGroup(GROUP_DISPLAYSETTINGS);
    conf->writeEntry(CONFIG_ENTRY_TAGTREEICONSIZE, m_tagtreeIconSize);
    conf->writeEntry(CONFIG_ENTRY_MAINSPLITTERSIZES, m_mainSplitterSizes);
    conf->writeEntry(CONFIG_ENTRY_TREESPLITTERSIZES, m_treeSplitterSizes);

    // store display settings
    conf->setGroup(GROUP_FILEPREVIEW);
    conf->writeEntry(CONFIG_ENTRY_PREVIEWSIZE, m_previewSize);
    conf->writeEntry(CONFIG_ENTRY_SELECTIONMODE, m_filetreeSelectionMode);
    conf->writeEntry(CONFIG_ENTRY_AUTOREFRESH, m_autoRefresh);

    // store filter settings
    conf->setGroup(GROUP_FILTERS);
    conf->writeEntry(CONFIG_ENTRY_SUBDIRSTOIGNORE, m_subdirsToIgnore);
    conf->writeEntry(CONFIG_ENTRY_FILETYPESTOHANDLE, m_filetypesToHandle);
    conf->writeEntry(CONFIG_ENTRY_TAGFILTEROPERATOR, m_tagfilterOperator);

    conf->setGroup(QString::null);
    conf->sync();
}


//
// private methods
//
void Configuration::trace() {

    kdDebug() << "[Configuration::trace] configuration values: " << endl;

    kdDebug() << "[" << GROUP_DIRECTORIES << "]" << endl;
    kdDebug() << CONFIG_ENTRY_LASTOPENEDFILE << " = " << m_lastOpenedFile << endl;
    kdDebug() << CONFIG_ENTRY_LASTADDEDSOURCEDIR << " = " << m_lastAddedSourcedir << endl;

    kdDebug() << "[" << GROUP_DISPLAYSETTINGS << "]" << endl;
    kdDebug() << CONFIG_ENTRY_TAGTREEICONSIZE<< " = " << m_tagtreeIconSize << endl;
    kdDebug() << CONFIG_ENTRY_MAINSPLITTERSIZES << " = " << toString(m_mainSplitterSizes) << endl;
    kdDebug() << CONFIG_ENTRY_TREESPLITTERSIZES << " = " << toString(m_treeSplitterSizes) << endl;

    kdDebug() << "[" << GROUP_FILEPREVIEW << "]" << endl;
    kdDebug() << CONFIG_ENTRY_PREVIEWSIZE << " = " << m_previewSize << endl;
    kdDebug() << CONFIG_ENTRY_SELECTIONMODE << " = " << m_filetreeSelectionMode << endl;
    kdDebug() << CONFIG_ENTRY_AUTOREFRESH << " = " << m_autoRefresh << endl;

    kdDebug() << "[" << GROUP_FILTERS << "]" << endl;
    kdDebug() << CONFIG_ENTRY_SUBDIRSTOIGNORE << " = " << m_subdirsToIgnore.join(",") << endl;
    kdDebug() << CONFIG_ENTRY_FILETYPESTOHANDLE << " = " << m_filetypesToHandle.join(",") << endl;
    kdDebug() << CONFIG_ENTRY_TAGFILTEROPERATOR << " = " << m_tagfilterOperator << endl;
}


QString Configuration::toString(QValueList<int> valueList) {

    QString str("");
    for (unsigned int i = 0; i < valueList.count(); i++) {
        if (i > 0) {
            str.append(", ");
        }
        str.append(QString::number(valueList[i]));
    }

    return str;
}
