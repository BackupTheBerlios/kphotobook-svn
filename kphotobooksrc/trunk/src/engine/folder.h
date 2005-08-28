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

#ifndef FOLDER_H
#define FOLDER_H

#include "../tracer/tracer.h"

#include <qdir.h>
#include <qobject.h>
#include <qptrlist.h>

class File;


/**
 * Represents a folder added to the database.
 *
 * CVS-ID $Id$
 */
class Folder : public QObject
{
    Q_OBJECT

    private:
        static Tracer* tracer;
    
    public:
        Folder(unsigned int id, QDir* dir, bool recursive);
    
        ~Folder();
    
        void setParent(Folder* parent);
    
        Folder* parent()
        {
            return m_parent;
        }
    
        QPtrList<Folder>* children()
        {
            return m_children;
        }
    
        unsigned int id()
        {
            return m_id;
        }
    
        QDir* dir()
        {
            return m_dir;
        }
    
        bool recursive()
        {
            return m_recursive;
        }
    
        void setFound(bool found)
        {
            m_found = found;
        }
        bool found()
        {
            return m_found;
        }
    
        void appendFile(File* file)
        {
            m_files->append(file);
        }
        QPtrList<File>* files()
        {
            return m_files;
        }
        void removeFile(File* file)
        {
            if (!m_deleteInProgress) {
                m_files->remove(file);
            }
        }
    
        void setInclude(bool include)
        {
            m_include = include;
        }
        bool include()
        {
            return m_include;
        }
    
        QString toString()
        {
            return QString("id: %1, folder: >%2<, recursive: %3").arg(m_id).arg(m_dir->absPath()).arg(m_recursive);
        }
    
    signals:
        void signalSelectedChanged(bool selected);
    
    
    private:
        /**
        * This member will be set to true when the destructor was invoked.
        * This is a hack to prevent that a file is removing itself from the files list
        * while destructing this object.
        */
        bool m_deleteInProgress;
    
        /**
        * The unique id of this folder.
        */
        unsigned int m_id;
    
        /**
        * The directory representing this folder.
        */
        QDir* m_dir;
    
        /**
        * True if the files in the subfolders of m_dir belong to this
        * folder too.
        */
        bool m_recursive;
    
        /**
        * Indicactes if this folder existed while startup oder the last rescan
        * of the filesystem;
        */
        bool m_found;
    
        /**
        * The parent of this folder.
        * If the parent is not null, this folder is not added be the user explicitely.
        * But it was added because the topmost parent was added recursively.
        */
        Folder* m_parent;
    
        /**
        * The list of all implicitely added subdirectories.
        */
        QPtrList<Folder>* m_children;
    
        /**
        * The list of all files in this folder.
        */
        QPtrList<File>* m_files;
    
        /**
        * Determines if the files in this folder are displayed...
        */
        bool m_include;
};

#endif
