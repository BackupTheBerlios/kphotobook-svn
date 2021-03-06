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

#include "sourcedirtreenode.h"

#include "sourcedirtree.h"
#include "treehelper.h"
#include "../constants.h"
#include "../engine/folder.h"
#include "../kphotobook.h"
#include "../settings/settings.h"
#include "../tracer/tracer.h"

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include <qcursor.h>
#include <qdir.h>
#include <qfont.h>
#include <qpainter.h>
#include <qpixmap.h>


Tracer* SourceDirTreeNode::tracer = Tracer::getInstance("kde.kphotobook.uitrees", "SourceDirTreeNode");


SourceDirTreeNode::SourceDirTreeNode(SourceDirTree* parent, KPhotoBook* photobook, Folder* sourceDir, KPopupMenu* contextMenu) :
    KListViewItem(parent),
    m_photobook(photobook),
    m_sourceDir(sourceDir),
    m_selectedFilesCount(0),
    m_contextMenu(contextMenu)
{
    init(false);
}


SourceDirTreeNode::SourceDirTreeNode(SourceDirTreeNode* parent, KPhotoBook* photobook, Folder* sourceDir, KPopupMenu* contextMenu) :
    KListViewItem(parent),
    m_photobook(photobook),
    m_sourceDir(sourceDir),
    m_selectedFilesCount(0),
    m_contextMenu(contextMenu)
{
    init(true);
}


void SourceDirTreeNode::refreshIcon()
{
    if (Settings::sourceDirTreeShowIcons()) {
        QIconSet iconSet = KGlobal::iconLoader()->loadIconSet(Constants::ICON_SOURCEDIR, KIcon::Small, Settings::sourceDirTreeIconSize(), true);
        if (iconSet.isNull()) {
            tracer->swarning(__func__) << "Could not load iconset with iconname: '" << Constants::ICON_SOURCEDIR << "'" << endl;
            this->setPixmap(SourceDirTree::COLUMN_TEXT, 0);
        } else {
            this->setPixmap(SourceDirTree::COLUMN_TEXT, iconSet.pixmap());
        }
    } else {
        this->setPixmap(SourceDirTree::COLUMN_TEXT, 0);
    }
}


void SourceDirTreeNode::setOpenRecursive(bool open)
{
    setOpen(open);

    // do recursive call on every child
    SourceDirTreeNode* child = dynamic_cast<SourceDirTreeNode*>(this->firstChild());
    while(child) {
        child->setOpen(open);
        child = dynamic_cast<SourceDirTreeNode*>(child->nextSibling());
    }
}


void SourceDirTreeNode::setSelectedFilesCount(int selectedFilesCount)
{
    m_selectedFilesCount = selectedFilesCount;
    QString selected = QString("%1 / %2");
    selected = selected.arg(m_selectedFilesCount);
    selected = selected.arg(m_sourceDir->files()->count());
    this->setText(SourceDirTree::COLUMN_SELECTED, selected);
}


void SourceDirTreeNode::setIncluded(bool included)
{
    m_sourceDir->setInclude(included);
    repaint();
}


bool SourceDirTreeNode::included()
{
    return m_sourceDir->include();
}


void SourceDirTreeNode::invertInclusion()
{
    setIncluded(!included());
}


void SourceDirTreeNode::setIncludedRecursive(bool included)
{
    setIncluded(included);

    // do recursive call on every child
    SourceDirTreeNode* child = dynamic_cast<SourceDirTreeNode*>(this->firstChild());
    while(child) {
        child->setIncludedRecursive(included);
        child = dynamic_cast<SourceDirTreeNode*>(child->nextSibling());
    }
}


void SourceDirTreeNode::invertInclusionRecursive()
{
    invertInclusion();

    // do recursive call on every child
    SourceDirTreeNode* child = dynamic_cast<SourceDirTreeNode*>(this->firstChild());
    while(child) {
        child->invertInclusionRecursive();
        child = dynamic_cast<SourceDirTreeNode*>(child->nextSibling());
    }
}


QString SourceDirTreeNode::getFilterString()
{
    QString filter;

    if (!m_sourceDir->include()) {
        filter = "exclude";
    }

    return filter;
}


void SourceDirTreeNode::applyFilterString(QString filter)
{
    m_sourceDir->setInclude(filter != "exclude");
}


void SourceDirTreeNode::leftClicked(SourceDirTree* /* sourceDirTree */, int column)
{
    switch (column) {
        case SourceDirTree::COLUMN_TEXT:
            break;
        case SourceDirTree::COLUMN_SELECTED:
            break;
        case SourceDirTree::COLUMN_INCLUDED: {
            m_sourceDir->setInclude(!m_sourceDir->include());
            // force redrawing of this listviewitem
            this->repaint();

            m_photobook->autoRefreshView();
            break;
        }
    }
}


void SourceDirTreeNode::rightClicked(SourceDirTree* /* sourceDirTree */, int column)
{
    switch (column) {
        case SourceDirTree::COLUMN_TEXT: {
            if (m_contextMenu) {
                m_contextMenu->exec(QCursor::pos());
            }
            break;
        }
        case SourceDirTree::COLUMN_SELECTED:
            break;
        case SourceDirTree::COLUMN_INCLUDED:
            break;
    }
}


void SourceDirTreeNode::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    QColorGroup myCg(cg);
    if (!m_sourceDir->found()) {
        myCg.setColor(QColorGroup::Text, cg.mid());
    }

    switch (column) {
        case SourceDirTree::COLUMN_TEXT: {
            KListViewItem::paintCell(p, myCg, column, width, alignment);
            break;
        }
        case SourceDirTree::COLUMN_SELECTED: {
            QFont oldFont = p->font();
            if  (m_selectedFilesCount > 0) {
                QFont font = QFont(oldFont);
                font.setBold(true);
                p->setFont(font);
            }
            KListViewItem::paintCell(p, myCg, column, width, alignment);
            p->setFont(oldFont);
            break;
        }
        case SourceDirTree::COLUMN_INCLUDED: {
            TreeHelper::drawCheckBox(p, cg, backgroundColor(SourceDirTree::COLUMN_INCLUDED), width, this->height(), true, m_sourceDir->include() ? TreeHelper::CHECKED : TreeHelper::NOT_CHECKED);
            break;
        }
    }
}


//
// private
//
void SourceDirTreeNode::init(bool showRelativePath)
{
    refreshIcon();

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

