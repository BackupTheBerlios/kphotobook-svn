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

#ifndef SOURCEDIRTREE_H
#define SOURCEDIRTREE_H

#include "../tracer/tracer.h"

#include <klistview.h>
#include <kpopupmenu.h>
#include <kfileitem.h>

#include <qptrlist.h>
#include <qstring.h>
#include <qlistview.h>
#include <qpoint.h>
#include <qintdict.h>

class File;
class KPhotoBook;

class SourceDir;
class SourceDirTreeNode;

/**
 * The SourceDirTree (can display checkboxes in the columns).
 *
 * CVS-ID $Id$
 */
class SourceDirTree : public KListView {

Q_OBJECT

private:
    static Tracer* tracer;

public:
    static const int COLUMN_TEXT = 0;
    static const int COLUMN_SELECTED = 1;
    static const int COLUMN_INCLUDED = 2;

    SourceDirTree(QWidget* parent, KPhotoBook* photobook, const char* name);

    ~SourceDirTree() {
        delete m_sourceDirNodeDict;
    }

    void expandCurrent(bool recursive = true);
    void collapseCurrent(bool recursive = true);
    void expandAll();
    void collapseAll();

    void includeWholeSourceDir();
    void excludeWholeSourceDir();
    void invertSourceDir();
    void includeAllSourceDirs();
    void excludeAllSourceDirs();
    void invertAllSourceDirs();

    void addSourceDirs(QPtrList<SourceDir>* rootNodeList);
    void addSourceDir(SourceDir* rootNode);
    void removeSourceDir(SourceDirTreeNode* node);

    void reflectSelectedFiles(const KFileItemList* selectedFiles);

    KPhotoBook* photobook() {
        return m_photobook;
    }

    SourceDirTreeNode* selectedSourceDir();

    void doRepaintAll();

    /**
     * Returns a list containing the ids of the opened nodes.
     */
    QStringList* getOpenNodes();

    /**
     * Opens the nodes with the specified ids.
     */
    void openNodes(QStringList* nodes);

    /**
     * Returns a dictionary containing the string representation of the filter
     * of every node.
     * The dictionary contains the node-id as key and the filter as value.
     */
    QIntDict<QString>* getFilter();

    /**
     * Applies the specified filters to the nodes in the sourcedirtree.
     * The dictionary must contain the node-id as key and the filter as value.
     */
    void applyFilter(QIntDict<QString>* filterList);

public slots:
    void slotLoadSettings();

private slots:
    void slotListViewClicked(int button, QListViewItem* item, const QPoint& point, int column);
    void slotListViewDoubleClicked(QListViewItem* item, const QPoint& point, int column);

private:
    void buildSourceDirTree(SourceDirTreeNode* parent, QPtrList<SourceDir>* children);

    KPhotoBook* m_photobook;

    QIntDict<SourceDirTreeNode>* m_sourceDirNodeDict;
};

#endif
