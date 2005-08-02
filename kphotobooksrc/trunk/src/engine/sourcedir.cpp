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

#include "sourcedir.h"

#include "file.h"


Tracer* SourceDir::tracer = Tracer::getInstance("kde.kphotobook.engine", "SourceDir");


SourceDir::SourceDir(unsigned int id, QDir* dir, bool recursive) :
    m_deleteInProgress(false),
    m_id(id),
    m_dir(dir),
    m_recursive(recursive),
    m_found(false),
    m_parent(0),
    m_children(0),
    m_files(new QPtrList<File>()),
    m_include(true) {
}


SourceDir::~SourceDir()
{
    tracer->sinvoked(__func__) << "For sourcedir '" << m_dir->absPath() << "'." << endl;

    m_deleteInProgress = true;

    // remove this sourcedir from the children list of the parent
    if (m_parent) {
        m_parent->m_children->remove(this);
        m_parent = 0;
    }

    // set the parent of all children to 0
    SourceDir* child;
    if (m_children) {
        for (child = m_children->first(); child; child = m_children->next()) {
            child->m_parent = 0;
        }
    }

    // delete members
    delete m_dir;
    delete m_children;

    m_files->setAutoDelete(true);
    delete m_files;
}


void SourceDir::setParent(SourceDir* parent)
{
    if (m_parent) {
        tracer->swarning(__func__) << "SourceDir '" << this->toString() << "' already has a parent directory: '" << parent->toString() << "'." << endl;
        return;
    }

    m_parent = parent;

    // initialize children pointerlist if necessary
    if (!parent->m_children) {
        parent->m_children = new QPtrList<SourceDir>;
    }

    // add this sourcedir as child of the parent
    parent->m_children->append(this);
}

#include "sourcedir.moc"
