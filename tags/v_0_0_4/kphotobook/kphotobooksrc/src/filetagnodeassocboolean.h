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

#ifndef FILETAGNODEASSOCBOOLEAN_H
#define FILETAGNODEASSOCBOOLEAN_H

#include "filetagnodeassoc.h"

#include <qstring.h>

class File;
class TagNodeBoolean;

/**
 * Concrete class representing the association between a file and a TagNodeBoolean.
 * This associations contains a reference to the tagnode and the state of the association.
 *
 * CVS-ID $Id: filetagnodeassocboolean.h,v 1.1 2004/03/07 18:52:26 starcube Exp $
 */
class FileTagNodeAssocBoolean : public FileTagNodeAssoc {

public:
    FileTagNodeAssocBoolean(File* file, TagNodeBoolean* tagNodeBoolean, bool value = false);

    FileTagNodeAssocBoolean(File* file, TagNodeBoolean* tagNodeBoolean, QString value);

    ~FileTagNodeAssocBoolean() {
        // we don't delete anything!
    }

    TagNodeBoolean* tagNodeBoolean();

    void setValue(bool value) {
        m_value = value;
    }

    bool value() {
        return m_value;
    }

    virtual QString valueAsString() {
        return (m_value ? QString("true") : QString("false"));
    }

    /**
     * Updates the value of this node with the value of the specified association.
     */
    void update(FileTagNodeAssoc* assoc);

private:
    /**
     * The value of this association.
     */
    bool m_value;
};

#endif
