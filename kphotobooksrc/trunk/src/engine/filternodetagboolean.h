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

#ifndef FILTERNODETAGBOOLEAN_H
#define FILTERNODETAGBOOLEAN_H

#include "filternode.h"
#include "file.h"
#include "tagnodeboolean.h"

#include <qobject.h>

/**
 *
 * CVS-ID $Id: tagnode.h 307 2005-04-17 12:47:26Z starcube $
 */
class FilterNodeTagBoolean : public FilterNode {

Q_OBJECT

public:
    FilterNodeTagBoolean(FilterNode* parent, TagNodeBoolean* tagNodeBoolean, bool value);
    FilterNodeTagBoolean(TagNodeBoolean* tagNodeBoolean, bool value);

    ~FilterNodeTagBoolean();

    /**
     * Evaluates if this subtree or leafnode matches the given file.
     */
    virtual bool evaluate(File* file) throw(FilterException*);

    /**
     * Prints the value of this Filternode and calls this method with a increased
     * indention on each child.
     * The method is intended for debugging the filter.
     *
     * @indention the indention to put before the text.
     */
    virtual void dump(QString indention = QString(""));

private:
    TagNodeBoolean* m_tagNodeBoolean;
    bool m_value;
};

#endif
