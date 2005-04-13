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

#ifndef TAGNODESTRING_H
#define TAGNODESTRING_H

#include "tagnode.h"

#include <qstring.h>
#include <qfile.h>

/**
 * Concrete tagnode implementation representing a string tag.
 *
 * CVS-ID $Id: tagnodeboolean.h 274 2005-03-25 08:52:15Z choenig $
 */
class TagNodeString : public TagNode {

private:
    static Tracer* tracer;


public:
    TagNodeString(unsigned int id, const QString& text, const QString& comment, const QString& iconName, TagNode* parent = 0);

    ~TagNodeString() {
    }

    void setTagged(File* file, QString value);
    bool tagged(File* file, QString pattern);
};

#endif
