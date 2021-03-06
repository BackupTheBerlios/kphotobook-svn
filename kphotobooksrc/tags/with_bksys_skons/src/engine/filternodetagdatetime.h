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

#ifndef FILTERNODETAGDATETIME_H
#define FILTERNODETAGDATETIME_H

#include "filternode.h"
class DateTimeFilterData;
class File;
class TagNodeDateTime;


/**
 *
 * CVS-ID $Id: tagnode.h 307 2005-04-17 12:47:26Z starcube $
 */
class FilterNodeTagDateTime : public FilterNode
{
    Q_OBJECT
    
    public:
        FilterNodeTagDateTime(FilterNode* parent, TagNodeDateTime* tagNodeDateTime, DateTimeFilterData* data);
        FilterNodeTagDateTime(TagNodeDateTime* tagNodeDateTime, DateTimeFilterData* data);
    
        ~FilterNodeTagDateTime();
    
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
        virtual void dump(QString indention = QString("")) const;
    
    private:
        TagNodeDateTime* m_tagNodeDateTime;
        DateTimeFilterData* m_data;
};

#endif
