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

#include "filternode.h"


Tracer* FilterNode::dumper = Tracer::getInstance("kde.kphotobook.engine", "FilterNode");


FilterNode::FilterNode(FilterNode* parent)
    : m_parent(0)
    , m_children(new QPtrList<FilterNode>)
{
    if (parent) {
        m_parent = parent;
        parent->m_children->append(this);
    }
}


FilterNode::~FilterNode()
{
    delete m_children;
}


void FilterNode::addChild(FilterNode* child) {

    if (child) {
        child->m_parent = this;
        m_children->append(child);
    }
}


#include "filternode.moc"
