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

#include "tagtreenode.h"

#include "tagtree.h"
#include "treehelper.h"
#include "../engine/tagnode.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"
#include "../settings/settings.h"

#include <kglobal.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include <qcheckbox.h>
#include <qcursor.h>
#include <qlistview.h>
#include <qstyle.h>


Tracer* TagTreeNode::tracer = Tracer::getInstance("kde.kphotobook.uitrees", "TagTreeNode");


TagTreeNode::TagTreeNode(TagTree* parent, KPhotoBook* photobook, TagNode* tagNode, KPopupMenu* contextMenu) :
    KListViewItem(parent),
    m_photobook(photobook),
    m_tagNode(tagNode),
    m_contextMenu(contextMenu),
    m_filterState(TagTreeNode::FILTERSTATE_IGNORE)
{
    this->setText(TagTree::COLUMN_VALUE, "");
    this->setText(TagTree::COLUMN_FILTER, "");

    refresh();
}


TagTreeNode::TagTreeNode(TagTreeNode* parent, KPhotoBook* photobook, TagNode* tagNode, KPopupMenu* contextMenu) :
    KListViewItem(parent),
    m_photobook(photobook),
    m_tagNode(tagNode),
    m_contextMenu(contextMenu),
    m_filterState(TagTreeNode::FILTERSTATE_IGNORE)
{
    this->setText(TagTree::COLUMN_VALUE, "");
    this->setText(TagTree::COLUMN_FILTER, "");

    refresh();
}


QString TagTreeNode::filterString()
{
    QString filter;

    switch (m_filterState) {
        case TagTreeNode::FILTERSTATE_EXCLUDE: {
            filter = "exclude";
            break;
        }
        case TagTreeNode::FILTERSTATE_INCLUDE: {
            filter = "include";
            break;
        }    
        case TagTreeNode::FILTERSTATE_IGNORE:
            break;
    }

    return filter;
}


void TagTreeNode::applyFilterString(QString filter)
{
    tracer->invoked(__func__);

    // this automatically sets the filter to EXCLUDE, if it is a secret tag and the user
    // has the settings corresponding.
    if (m_tagNode->secret() && Settings::tagTreeUnsetSecretFilter() ) {
        m_filterState = TagTreeNode::FILTERSTATE_EXCLUDE;
        return;
    }

    if (filter == "exclude") {
        m_filterState = TagTreeNode::FILTERSTATE_EXCLUDE;
    } else if (filter == "include") {
        m_filterState = TagTreeNode::FILTERSTATE_INCLUDE;
    } else {
        m_filterState = TagTreeNode::FILTERSTATE_IGNORE;
    }
}


void TagTreeNode::refresh()
{
    if (Settings::tagTreeShowIcons()) {
        QIconSet iconSet = KGlobal::iconLoader()->loadIconSet(*m_tagNode->iconName(), KIcon::Small, Settings::tagTreeIconSize(), true);
        if (iconSet.isNull()) {
            if (!(*m_tagNode->iconName()).isEmpty()) {
                tracer->swarning(__func__) << "Could not load iconset with iconname: '" << *m_tagNode->iconName() << "'" << endl;
            }
            this->setPixmap(TagTree::COLUMN_TEXT, 0);
        } else {
            this->setPixmap(TagTree::COLUMN_TEXT, iconSet.pixmap());
        }
    } else {
        this->setPixmap(TagTree::COLUMN_TEXT, 0);
    }

    this->setText(TagTree::COLUMN_TEXT, QString(*m_tagNode->text()).prepend(" "));

    // last but not least update the current state of the node
    this->updateTagMatch();
}


void TagTreeNode::setOpenRecursive(bool open)
{
    setOpen(open);

    // do recursive call on every child
    TagTreeNode* child = dynamic_cast<TagTreeNode*>(this->firstChild());
    while(child) {
        child->setOpen(open);
        child = dynamic_cast<TagTreeNode*>(child->nextSibling());
    }
}

void TagTreeNode::leftClicked(__attribute__((unused)) TagTree* tagTree, int column)
{
    int button = KMessageBox::Yes;

    switch (column) {
        case TagTree::COLUMN_TEXT:
            break;
        case TagTree::COLUMN_VALUE:
            break;
        case TagTree::COLUMN_FILTER: {
            // change state of the filter: exclude -> ignore -> include -> exclude -> ...
            switch (m_filterState) {
                case TagTreeNode::FILTERSTATE_EXCLUDE: {
                    if (m_tagNode->secret()) {
                        button = KMessageBox::warningYesNo(0, i18n("<b></b>This tag is set secret!<br><br><b>Do you really don't care, if matching images are displayed now?</b>"), i18n("Secret Tag"));
                    }
                    if (button == KMessageBox::Yes) {
                        m_filterState = TagTreeNode::FILTERSTATE_IGNORE;
                    }
                    break;
                }
                case TagTreeNode::FILTERSTATE_IGNORE: {
                    if (m_tagNode->secret()) {
                        button = KMessageBox::warningYesNo(0, i18n("<b></b>This tag is set secret!<br><br><b>Do you really wan't to show matching images now?</b>"), i18n("Secret Tag"));
                    }
                    if (button == KMessageBox::Yes) {
                        m_filterState = TagTreeNode::FILTERSTATE_INCLUDE;
                    }
                    break;
                }
                case TagTreeNode::FILTERSTATE_INCLUDE: {
                    m_filterState = TagTreeNode::FILTERSTATE_EXCLUDE;
                    break;
                }
            }
            break;
        }
    }
}


void TagTreeNode::rightClicked(__attribute__((unused)) TagTree* tagTree, __attribute__((unused)) int column)
{
    tracer->sinvoked(__func__);

    int button = KMessageBox::Yes;

    switch (column) {
        case TagTree::COLUMN_TEXT: {
            if (m_contextMenu) {
                m_contextMenu->exec(QCursor::pos());
            }
            break;
        }
        case TagTree::COLUMN_VALUE:
            break;
        case TagTree::COLUMN_FILTER: {
            // change state of the filter: exclude -> include -> ignore -> exclude -> ...
            switch (m_filterState) {
                case TagTreeNode::FILTERSTATE_EXCLUDE: {
                    if (m_tagNode->secret()) {
                        button = KMessageBox::warningYesNo(0, i18n("<b></b>This tag is set secret!<br><br><b>Do you really wan't to show matching images now?</b>"), i18n("Secret Tag"));
                    }
                    if (button == KMessageBox::Yes) {
                        m_filterState = TagTreeNode::FILTERSTATE_INCLUDE;
                    }
                    break;
                }
                case TagTreeNode::FILTERSTATE_IGNORE: {
                    m_filterState = TagTreeNode::FILTERSTATE_EXCLUDE;
                    break;
                }
                case TagTreeNode::FILTERSTATE_INCLUDE: {
                    m_filterState = TagTreeNode::FILTERSTATE_IGNORE;
                    break;
                }
            }
            break;
        }
    }
}


void TagTreeNode::paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int alignment)
{
    switch (column) {
        case TagTree::COLUMN_TEXT: {
            //toggle bold tags
            if (Settings::tagTreeBoldMatches() && recursiveFindTagged()
                && (!Settings::tagTreeBoldMatchesCollapsedOnly() || (firstChild() && !isOpen()))) {
    
                    QFont f(p->font());
                    f.setBold(true);
                    p->setFont(f);
                }
    
            KListViewItem::paintCell(p, cg, column, width, alignment);
            break;
        }
        case TagTree::COLUMN_VALUE:
        case TagTree::COLUMN_FILTER: {
            KListViewItem::paintCell(p, cg, column, width, alignment);
            break;
        }
    }
}


QString TagTreeNode::toolTip(int column)
{
    switch (column) {
        case TagTree::COLUMN_TEXT:
        case TagTree::COLUMN_VALUE:
        case TagTree::COLUMN_FILTER:
            return *(m_tagNode->comment());
    }

    return "";
}


TagTreeNode::MatchType TagTreeNode::findTagMatch()
{
    TagTreeNode::MatchType state = TagTreeNode::UNTAGGED;

    // get all selected files
    const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

    if (selectedFiles->count()) {
        int taggedFilesCount = 0;
        int untaggedFilesCount = 0;

        // loop over all selected files and determine their state
        QPtrListIterator<KFileItem> it(*m_photobook->view()->fileView()->selectedItems());
        for (; it.current(); ++it) {
            File* selectedFile = dynamic_cast<File*>(it.current());

            if (m_tagNode->tagged(selectedFile)) {
                taggedFilesCount++;
            } else {
                untaggedFilesCount++;
            }

            // we can abort the loop if we found a tagged and utagged file
            if (taggedFilesCount && untaggedFilesCount) {
                break;
            }
        }

        // no file is tagged
        if (!taggedFilesCount && untaggedFilesCount) {
            state = TagTreeNode::UNTAGGED;
        } else if (taggedFilesCount && !untaggedFilesCount) {
            state = TagTreeNode::TAGGED;
        } else {
            state = TagTreeNode::MIXTAGGED;
        }
    } else {
        state = TagTreeNode::NOSELECT;
    }

    return state;
}


bool TagTreeNode::recursiveFindTagged()
{
    // do WE have a positive match?...
    if (m_tagCurrentMatch == TagTreeNode::TAGGED | m_tagCurrentMatch == TagTreeNode::MIXTAGGED) {
        return true;
    } else {
        //... otherwise, ask all my children and return true, if there is a positive match
        TagTreeNode* myChild = dynamic_cast<TagTreeNode*>(firstChild());
        while( myChild ) {
            if (myChild->recursiveFindTagged()) {
                return true;
            }
            myChild = dynamic_cast<TagTreeNode*>(myChild->nextSibling());
        }
    }
    //if no match is found, return false
    return false;
}

