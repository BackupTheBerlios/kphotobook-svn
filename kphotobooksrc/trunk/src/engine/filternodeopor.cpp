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

#include "filternodeopor.h"


FilterNodeOpOr::FilterNodeOpOr(FilterNode* parent)
    :FilterNode(parent)
{
}


FilterNodeOpOr::~FilterNodeOpOr()
{
}

    
bool FilterNodeOpOr::evaluate(File* file) throw(FilterException*)
{

    // an or operator with no value is evaluated to true!
    if (m_children->count() == 0) {
        return true;
    }
    
    FilterNode* child;
    for ( child = m_children->first(); child; child = m_children->next() ) {

        // if at least one child evaluates to true --> the whole expression is true!
        if (child->evaluate(file)) {
            return true;
        }
    }
    
    return false;
}


void FilterNodeOpOr::dump(QString indention)
{
    dumper->sdebug("dump") << indention << "||" << endl;

    // loop over all children and call this method recursively
    indention.append("  ");
    FilterNode* child;
    for ( child = m_children->first(); child; child = m_children->next() ) {

        child->dump(indention);
    }
}


#include "filternodeopor.moc"
