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

#include "filternodeopnegate.h"

#include "file.h"
#include "../tracer/tracer.h"


Tracer* FilterNodeOpNegate::tracer = Tracer::getInstance("kde.kphotobook.engine", "FilterNodeOpNegate");


FilterNodeOpNegate::FilterNodeOpNegate(FilterNode* parent) :
    FilterNode(parent)
{
}


FilterNodeOpNegate::~FilterNodeOpNegate()
{
}

    
bool FilterNodeOpNegate::evaluate(File* file) throw(FilterException*)
{
    FilterNode* child = m_children->first();
    if (!child) {
        QString msg = QString("Negate operator with no child found!");
        tracer->serror(__func__) << msg << endl;
        throw new FilterException(msg, "no detailmessage");
    }

    if (m_children->next()) {
        QString msg = QString("Negate operator with more than one child found!");
        tracer->serror(__func__) << msg << endl;
        throw new FilterException(msg, "no detailmessage");
    }

    return !child->evaluate(file);
}


void FilterNodeOpNegate::dump(QString indention) const
{
    dumper->sdebug("dump") << indention << "!" << endl;

    // loop over all children and call this method recursively
    indention.append("  ");
    FilterNode* child;
    for ( child = m_children->first(); child; child = m_children->next() ) {

        child->dump(indention);
    }
}

    
#include "filternodeopnegate.moc"
