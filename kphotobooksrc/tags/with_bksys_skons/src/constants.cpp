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

#include <qstring.h>


const QString Constants::STRING_VALUE_TRUE = QString("true");
const QString Constants::STRING_VALUE_FALSE = QString("false");


//
// icons
//
const QString Constants::ICON_TAG = QString("kpb_tag");
const QString Constants::ICON_CREATE_MAINTAG = QString("kpb_tag_add");
const QString Constants::ICON_CREATE_SUBTAG = QString("kpb_tag_add");
const QString Constants::ICON_EDIT_TAG = QString("kpb_tag_edit");
const QString Constants::ICON_DELETE_TAG = QString("kpb_tag_del");
const QString Constants::ICON_TAG_FILTER_DESELECT = QString("kpb_filter_deselect");
const QString Constants::ICON_TAG_FILTER_RESET = QString("kpb_filter_reset");

const QString Constants::ICON_SOURCEDIR = QString("kpb_folder");
const QString Constants::ICON_ADD_SOURCEDIR = QString("kpb_folder_add");
const QString Constants::ICON_EDIT_SOURCEDIR = QString("kpb_folder_edit");
const QString Constants::ICON_REMOVE_SOURCEDIR = QString("kpb_folder_remove");

const QString Constants::ICON_RESCAN_FILESYSTEM = QString("kpb_filesystem_reload");
const QString Constants::ICON_RESCAN_FILESYSTEM_WITH_EXIF = QString("kpb_filesystem_reload");

const QString Constants::ICON_AUTOREFRESH_VIEW = QString("kpb_view_autorefresh");
const QString Constants::ICON_REFRESH_VIEW = QString("kpb_view_refresh");
const QString Constants::ICON_INCREASE_PREVIEWSIZE = QString("kpb_viewmag_plus");
const QString Constants::ICON_DECREASE_PREVIEWSIZE = QString("kpb_viewmag_minus");

const QString Constants::ICON_INCLUDE_WHOLE_FOLDER = QString("kpb_folder_green");
const QString Constants::ICON_EXCLUDE_WHOLE_FOLDER = QString("kpb_folder_red");
const QString Constants::ICON_INVERT_FOLDER_SELECTION = QString("kpb_invert");

const QString Constants::ICON_EXPAND_FOLDER = QString("kpb_expand");
const QString Constants::ICON_COLLAPSE_FOLDER = QString("kpb_collapse");

const QString Constants::ICON_SETTINGS_GENERAL = QString("kpb_folder_home");
const QString Constants::ICON_SETTINGS_TAG = QString("kpb_tag");
const QString Constants::ICON_SETTINGS_SOURCEDIR = QString("kpb_folder");
const QString Constants::ICON_SETTINGS_IMAGEPREVIEW = QString("kpb_view_refresh");
///@todo add an icon for the imageviewersettings
const QString Constants::ICON_SETTINGS_IMAGEVIEWER= QString("kpb_view_refresh");
const QString Constants::ICON_SETTINGS_FILEHANDLING = QString("kpb_folder_txt");
const QString Constants::ICON_SETTINGS_TOOLS = QString("kpb_kfind");

const QString Constants::ICON_OPERATOR_AND = QString("kpb_andify");
const QString Constants::ICON_OPERATOR_OR = QString("kpb_orify");

const QString Constants::ICON_RESTORE_TOOL_VIEWS = QString("kpb_view_layout_restore");

const QString Constants::ICON_TAG_LOCK = QString("kpb_tag_lock");
const QString Constants::ICON_TAG_UNLOCK = QString("kpb_tag_unlock");

const QString Constants::ICON_TAG_EXIF = QString("kpb_tag_exif");


//
// file-extension
//
const QString Constants::FILE_EXTENSION = QString("kpb");


//
// constants for settings
//
const int Constants::SETTINGS_MIN_PREVIEW_SIZE = 32;
const int Constants::SETTINGS_MAX_PREVIEW_SIZE = 160;
