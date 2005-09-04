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

#include "filternodetagradio.h"

#include "file.h"
#include "tagnoderadio.h"


FilterNodeTagRadio::FilterNodeTagRadio(FilterNode* parent, TagNodeRadio* tagNodeRadio, bool value) :
    FilterNode(parent),
    m_tagNodeRadio(tagNodeRadio),
    m_value(value)
{
}


FilterNodeTagRadio::FilterNodeTagRadio(TagNodeRadio* tagNodeRadio, bool value) :
    FilterNode(0) ,
    m_tagNodeRadio(tagNodeRadio),
    m_value(value)
{
}


FilterNodeTagRadio::~FilterNodeTagRadio()
{
}


bool FilterNodeTagRadio::evaluate(File* file) throw(FilterException*)
{
    return m_tagNodeRadio->tagged(file) == m_value;
}


void FilterNodeTagRadio::dump(QString indention) const
{
    dumper->sdebug("dump") << indention << *m_tagNodeRadio->text() << (m_value ? " = true" : " = false") << endl;
}


#include "filternodetagradio.moc"
