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

#include "xmlconstants.h"


// the constants containg the element- and attribute-names
const QString XmlConstants::ELEMENT_KPHOTOBOOK = QString("kphotobook");
const QString XmlConstants::ELEMENT_SOURCEDIRS = QString("sourcedirs");
const QString XmlConstants::ELEMENT_SOURCEDIR = QString("sourcedir");
const QString XmlConstants::ELEMENT_TAGS = QString("tags");
const QString XmlConstants::ELEMENT_TAG = QString("tag");
const QString XmlConstants::ELEMENT_FILES = QString("files");
const QString XmlConstants::ELEMENT_FILE = QString("file");
const QString XmlConstants::ELEMENT_TAGASSOC = QString("tagassoc");

const QString XmlConstants::ATTRIBUTE_KPHOTOBOOK_NAME = QString("name");
const QString XmlConstants::ATTRIBUTE_KPHOTOBOOK_UID = QString("uid");

const QString XmlConstants::ATTRIBUTE_SOURCEDIR_ID = QString("id");
const QString XmlConstants::ATTRIBUTE_SOURCEDIR_DIR = QString("dir");
const QString XmlConstants::ATTRIBUTE_SOURCEDIR_RECURSIVE = QString("recursive");

const QString XmlConstants::ATTRIBUTE_TAG_ID = QString("id");
const QString XmlConstants::ATTRIBUTE_TAG_NAME = QString("name");
const QString XmlConstants::ATTRIBUTE_TAG_COMMENT = QString("comment");
const QString XmlConstants::ATTRIBUTE_TAG_TYPE = QString("type");
const QString XmlConstants::ATTRIBUTE_TAG_ICON = QString("icon");
const QString XmlConstants::ATTRIBUTE_TAG_SECRET = QString("secret");

const QString XmlConstants::ATTRIBUTE_FILES_SOURCEDIRID = QString("sourcedirId");

const QString XmlConstants::ATTRIBUTE_FILE_NAME = QString("name");
const QString XmlConstants::ATTRIBUTE_FILE_ROTATE = QString("rotate");

const QString XmlConstants::ATTRIBUTE_TAGASSOC_TAGID = QString("tagId");
const QString XmlConstants::ATTRIBUTE_TAGASSOC_VALUE = QString("value");
