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

 #include "constants.h"

//
// icons
//
const QString Constants::ICON_TAG = QString("tag");
const QString Constants::ICON_CREATE_MAINTAG = QString("tag_add");
const QString Constants::ICON_CREATE_SUBTAG = QString("tag_add");
const QString Constants::ICON_EDIT_TAG = QString("tag_edit");
const QString Constants::ICON_DELETE_TAG = QString("tag_del");
const QString Constants::ICON_TAG_FILTER_DESELECT = QString("filter_deselect");
const QString Constants::ICON_TAG_FILTER_RESET = QString("filter_reset");

const QString Constants::ICON_SOURCEDIR = QString("folder");
const QString Constants::ICON_ADD_SOURCEDIR = QString("sourcedir_add");
const QString Constants::ICON_EDIT_SOURCEDIR = QString("sourcedir_edit");
const QString Constants::ICON_REMOVE_SOURCEDIR = QString("sourcedir_remove");

const QString Constants::ICON_RESCAN_FILESYSTEM = QString("filesystem_reload");

const QString Constants::ICON_AUTOREFRESH_VIEW = QString("view_autorefresh");
const QString Constants::ICON_REFRESH_VIEW = QString("view_refresh");
const QString Constants::ICON_INCREASE_PREVIEWSIZE = QString("viewmag+");
const QString Constants::ICON_DECREASE_PREVIEWSIZE = QString("viewmag-");

const QString Constants::ICON_INCLUDE_WHOLE_FOLDER = QString("folder_green");
const QString Constants::ICON_EXCLUDE_WHOLE_FOLDER = QString("folder_red");
const QString Constants::ICON_INVERT_FOLDER_SELECTION = QString("invert");

const QString Constants::ICON_EXPAND_FOLDER = QString("expand");
const QString Constants::ICON_COLLAPSE_FOLDER = QString("collapse");

const QString Constants::ICON_SETTINGS_GENERAL = QString("folder_home");
const QString Constants::ICON_SETTINGS_TAG = QString("tag");
const QString Constants::ICON_SETTINGS_SOURCEDIR = QString("folder");
const QString Constants::ICON_SETTINGS_IMAGEPREVIEW = QString("view_refresh");
const QString Constants::ICON_SETTINGS_FILEHANDLING = QString("folder_txt");
const QString Constants::ICON_SETTINGS_TOOLS = QString("kfind");

const QString Constants::ICON_OPERATOR_AND = QString("andify");
const QString Constants::ICON_OPERATOR_OR = QString("orify");


//
// file-extension
//
const QString Constants::FILE_EXTENSION = QString("kpb");


//
// constants for settings
//
const int Constants::SETTINGS_MIN_PREVIEW_SIZE = 32;
const int Constants::SETTINGS_MAX_PREVIEW_SIZE = 160;


//
// constants for tree
//
const int Constants::TREE_CHECKBOX_MAXSIZE = 16;
