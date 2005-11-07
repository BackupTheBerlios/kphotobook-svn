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

#include "filternodeopand.h"

#include "file.h"
#include "../tracer/tracer.h"


Tracer* FilterNodeOpAnd::tracer = Tracer::getInstance("kde.kphotobook.engine", "FilterNodeOpAnd");


FilterNodeOpAnd::FilterNodeOpAnd(FilterNode* parent) :
    FilterNode(parent)
{
}


FilterNodeOpAnd::~FilterNodeOpAnd()
{
}

    
bool FilterNodeOpAnd::evaluate(File* file) throw(FilterException*)
{
    FilterNode* child;
    for ( child = m_children->first(); child; child = m_children->next() ) {

        // if at least one child evaluates to false --> the whole expression is false!
        if (!(child->evaluate(file))) {
            return false;
        }
    }
    
    return true;
}


void FilterNodeOpAnd::dump(QString indention) const
{
    dumper->sdebug("dump") << indention << "&&" << endl;

    // loop over all children and call this method recursively
    indention.append("  ");
    FilterNode* child;
    for ( child = m_children->first(); child; child = m_children->next() ) {

        child->dump(indention);
    }
}


#include "filternodeopand.moc"
