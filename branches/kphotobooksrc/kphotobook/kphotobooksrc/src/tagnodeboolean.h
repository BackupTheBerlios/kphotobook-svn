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

#ifndef TAGNODEBOOLEAN_H
#define TAGNODEBOOLEAN_H

#include "tagnode.h"

#include <qstring.h>
#include <qfile.h>

/**
 * Concrete tagnode implementation representing a boolean tag.
 *
 * CVS-ID $Id: tagnodeboolean.h,v 1.1 2004/03/07 18:52:42 starcube Exp $
 */
class TagNodeBoolean : public TagNode {

public:
    TagNodeBoolean(unsigned int id, const QString& text, const QString& iconName, TagNode* parent = 0);

    ~TagNodeBoolean() {
    }

    void setTagged(File* file, bool tagged) {
        m_tagged = tagged;
    }
    bool tagged(File* file) {
        return m_tagged;
    }

    void setFiltered(bool filtered) {
        m_filtered = filtered;
    }
    bool filtered() {
        return m_filtered;
    }

    /**
     * If this node is tagged (m_tagged = true) then 100 is returned.
     * Else the percentage of the tagged children is returned.
     */
    int tagLevel();

private:
    bool m_tagged;
    bool m_filtered;
};

#endif
