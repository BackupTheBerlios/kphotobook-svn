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

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <qstring.h>

/**
 * Constants for kphotobook.
 *
 * CVS-ID $Id: constants.h,v 1.8 2004/03/30 21:44:31 starcube Exp $
 */
class Constants {

public:
    // icons
    static const QString ICON_TAG;
    static const QString ICON_CREATE_MAINTAG;
    static const QString ICON_CREATE_SUBTAG;
    static const QString ICON_EDIT_TAG;
    static const QString ICON_DELETE_TAG;

    static const QString ICON_SOURCEDIR;
    static const QString ICON_ADD_SOURCEDIR;
    static const QString ICON_EDIT_SOURCEDIR;
    static const QString ICON_REMOVE_SOURCEDIR;

    static const QString ICON_RESCAN_FILESYSTEM;

    static const QString ICON_AUTOREFRESH_VIEW;
    static const QString ICON_REFRESH_VIEW;
    static const QString ICON_INCREASE_PREVIEWSIZE;
    static const QString ICON_DECREASE_PREVIEWSIZE;

    static const QString ICON_INCLUDE_WHOLE_FOLDER;
    static const QString ICON_EXCLUDE_WHOLE_FOLDER;
    static const QString ICON_INVERT_FOLDER_SELECTION;

    static const QString ICON_EXPAND_FOLDER;
    static const QString ICON_COLLAPSE_FOLDER;

    static const QString ICON_SETTINGS_GENERAL;
    static const QString ICON_SETTINGS_TAG;
    static const QString ICON_SETTINGS_SOURCEDIR;
    static const QString ICON_SETTINGS_IMAGEPREVIEW;
    static const QString ICON_SETTINGS_FILEHANDLING;
    static const QString ICON_SETTINGS_TOOLS;

    static const QString ICON_OPERATOR_AND;
    static const QString ICON_OPERATOR_OR;

    // file-extension
    static const QString FILE_EXTENSION; /**< The default used fileextension for kphotobook files. */

    // constants for settings
    static const int SETTINGS_MIN_PREVIEW_SIZE;
    static const int SETTINGS_MAX_PREVIEW_SIZE;

    // constants for tree
    static const int TREE_CHECKBOX_MAXSIZE;
};

#endif
