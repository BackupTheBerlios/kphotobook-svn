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

#include "tagtreenodesourcedir.h"

#include "constants.h"
#include "configuration.h"
#include "kphotobook.h"
#include "kphotobookview.h"
#include "sourcedirtree.h"
#include "sourcedir.h"
#include "tagtreenode.h"

#include "kglobal.h"
#include "kiconloader.h"
#include "klocale.h"

#include "qpixmap.h"

TagTreeNodeSourceDir::TagTreeNodeSourceDir(KListView* parent, SourceDir* sourceDir, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, QString::null, photobook, 0, contextMenu)
    , m_sourceDir(sourceDir)
    , m_selectedFilesCount(0) {

    init(false);
}

TagTreeNodeSourceDir::TagTreeNodeSourceDir(KListViewItem* parent, SourceDir* sourceDir, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, QString::null, photobook, 0, contextMenu)
    , m_sourceDir(sourceDir)
    , m_selectedFilesCount(0) {

    init(true);
}


void TagTreeNodeSourceDir::setSelectedFilesCount(int selectedFilesCount) {
    m_selectedFilesCount = selectedFilesCount;
    QString selected = QString("%1 / %2");
    selected = selected.arg(m_selectedFilesCount);
    selected = selected.arg(m_sourceDir->files()->count());
    this->setText(SourceDirTree::COLUMN_SELECTED, selected);
}


void TagTreeNodeSourceDir::setIncluded(bool included) {
    m_sourceDir->setInclude(included);
    repaint();
}


bool TagTreeNodeSourceDir::included() {
    return m_sourceDir->include();
}


void TagTreeNodeSourceDir::invertInclusion() {
    setIncluded(!included());
}


void TagTreeNodeSourceDir::setIncludedRecursive(bool included) {
    setIncluded(included);

    // do recursive call on every child
    TagTreeNodeSourceDir* child = dynamic_cast<TagTreeNodeSourceDir*>(this->firstChild());
    while(child) {
        child->setIncludedRecursive(included);
        child = dynamic_cast<TagTreeNodeSourceDir*>(child->nextSibling());
    }
}


void TagTreeNodeSourceDir::invertInclusionRecursive() {
    invertInclusion();

    // do recursive call on every child
    TagTreeNodeSourceDir* child = dynamic_cast<TagTreeNodeSourceDir*>(this->firstChild());
    while(child) {
        child->invertInclusionRecursive();
        child = dynamic_cast<TagTreeNodeSourceDir*>(child->nextSibling());
    }
}


void TagTreeNodeSourceDir::setOpenRecursive(bool open) {
    setOpen(open);

    // do recursive call on every child
    TagTreeNodeSourceDir* child = dynamic_cast<TagTreeNodeSourceDir*>(this->firstChild());
    while(child) {
        child->setOpen(open);
        child = dynamic_cast<TagTreeNodeSourceDir*>(child->nextSibling());
    }
}


void TagTreeNodeSourceDir::columnClicked(__attribute__((unused)) TagTree* tagTree, int column) {

    switch (column) {
    case SourceDirTree::COLUMN_TEXT:
        break;

    case SourceDirTree::COLUMN_SELECTED:
        break;

    case SourceDirTree::COLUMN_INCLUDED:
        m_sourceDir->setInclude(!m_sourceDir->include());
        // force redrawing of this listviewitem
        this->repaint();

        m_photobook->autoRefreshView();
        break;
    }
}


void TagTreeNodeSourceDir::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {

    QColorGroup myCg(cg);
    if (!m_sourceDir->found()) {
        myCg.setColor(QColorGroup::Text, cg.mid());
    }

    switch (column) {
    case SourceDirTree::COLUMN_TEXT :
        KListViewItem::paintCell(p, myCg, column, width, alignment);
        break;

    case SourceDirTree::COLUMN_SELECTED :
        KListViewItem::paintCell(p, myCg, column, width, alignment);
        break;

    case SourceDirTree::COLUMN_INCLUDED :
        // paint the cell with the alternating background color
        p->fillRect(0, 0, width, this->height(), backgroundColor());

        // draw the checkbox in the center of the cell
        QRect rect((width-this->height()+4)/2, 2, this->height()-4, this->height()-4);

        drawCheckBox(p, myCg, rect, m_sourceDir->include());

        break;
    }
}


//
// private
//
void TagTreeNodeSourceDir::init(bool showRelativePath) {

    static QPixmap directoryIcon = QPixmap(KGlobal::iconLoader()->loadIcon(Constants::ICON_SOURCEDIR, KIcon::Small, Configuration::getInstance()->tagtreeIconSize()));
    this->setPixmap(SourceDirTree::COLUMN_TEXT, directoryIcon);

    // create and set text for text column
    QString text = m_sourceDir->dir()->absPath();
    if (showRelativePath) {
        // we are a subdir of a sourcedir --> display relative path only
        text = m_sourceDir->dir()->dirName();
    }
    text.prepend(" ");

    if (!m_sourceDir->found()) {
        text.append(" (");
        text.append(i18n("not found"));
        text.append(")");
    }
    this->setText(SourceDirTree::COLUMN_TEXT, text);

    // create and set text for selected column
    QString selected = QString("%1 / %2");
    selected = selected.arg(m_selectedFilesCount);
    selected = selected.arg(m_sourceDir->files()->count());
    this->setText(SourceDirTree::COLUMN_SELECTED, selected);
}

