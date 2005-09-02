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

#ifndef FILE_H
#define FILE_H

class Engine;
class FileTagNodeAssoc;
class Folder;
class TagNode;
class Tracer;

#include <kfileitem.h>

class QFileInfo;
template<class type> class QPtrListStdIterator;



/**
 * Represents a file/image.
 *
 * CVS-ID $Id$
 */
class File : public KFileItem
{
    private:
        static Tracer* tracer;

    public:
        /**
        * This constructor is used to instantiate a File-object representing
        * a file on the filesystem.
        *
        * The parameter fileInfo must not be null! If it is, a NullPointer occurs.
        */
        File(Engine* engine, Folder* sourceDir, QFileInfo* fileInfo, int rotate = 0);

        ~File();

        /**
        * Return the folder this file belongs to.
        */
        Folder* sourceDir() const
        {
            return m_sourceDir;
        }

        /**
        * Returns the QFileInfo specified during instantiating this object.
        */
        QFileInfo* fileInfo() const
        {
            return m_fileInfo;
        }

        void setRotate(int degrees)
        {
            m_rotate = degrees;
        }

        int rotate() const
        {
            return m_rotate;
        }

        void setFound(bool found)
        {
            m_found = found;
        }
        bool found() const
        {
            return m_found;
        }

        /**
        * Appends the specified assoc to the list of associations for this file.
        */
        void appendAssoc(FileTagNodeAssoc* assoc);

        /**
        * Removes the specified assoc from the list of assocs.
        */
        void removeAssoc(FileTagNodeAssoc* assoc);

        /**
        * Returns the association between this file and the specified tagNode, if there
        * exists one.
        */
        FileTagNodeAssoc* getAssoc(TagNode* tagNode) const;

        /**
        * Returns all associations this file has to tagNodes.
        */
        QPtrList<FileTagNodeAssoc>* assocs() const
        {
            return m_assocs;
        }

    private:
        /**
        * The engine we belong to.
        */
        Engine* m_engine;

        /**
        * The folder we belong too.
        */
        Folder* m_sourceDir;

        /**
        * The represented file.
        */
        QFileInfo* m_fileInfo;

        /**
        * The degrees to rotate the image with.
        * This is the information read from the xml file, NOT from EXIF.
        */
        int m_rotate;

        /**
        * Indicates if this file existed while the startup or the last rescan
        * of the filesystem;
        */
        bool m_found;

        /**
        * The list of all associations this file has a reference to.
        */
        QPtrList<FileTagNodeAssoc>* m_assocs;
};

#endif
