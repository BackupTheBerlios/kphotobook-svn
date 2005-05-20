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

#include "tagtreenoderadiogroup.h"


#include "tagtree.h"
#include "treehelper.h"
#include "../engine/file.h"
#include "../engine/filternodeopor.h"
#include "../engine/filternodeopand.h"
#include "../engine/filternodetagradio.h"
#include "../engine/tagnoderadio.h"
#include "../engine/tagnoderadiogroup.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"

#include <kfileitem.h>

Tracer* TagTreeNodeRadioGroup::tracer = Tracer::getInstance("kde.kphotobook.uitrees", "TagTreeNodeRadioGroup");

TagTreeNodeRadioGroup::TagTreeNodeRadioGroup(TagTree* parent, TagNodeRadioGroup* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu) {
}


TagTreeNodeRadioGroup::TagTreeNodeRadioGroup(TagTreeNode* parent, TagNodeRadioGroup* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu)
    : TagTreeNode(parent, photobook, tagNode, contextMenu) {
}


TagTreeNodeRadioGroup::~TagTreeNodeRadioGroup() {
}


FilterNode* TagTreeNodeRadioGroup::filter() {

    FilterNode* rootFilter = new FilterNodeOpAnd(0);
    FilterNode* andFilter = new FilterNodeOpAnd(rootFilter);
    FilterNode* orFilter = new FilterNodeOpOr(rootFilter);

    // loop over all radio children and add them to the filterRootNode
    TagTreeNodeRadio* child = dynamic_cast<TagTreeNodeRadio*>(this->firstChild());
    while( child ) {

       // get the filter of the current child
        FilterNode* childFilter = child->subfilter();
        if (childFilter) {

            if (child->getFilterString() == "include") {
                orFilter->addChild(childFilter);
            }
            else if (child->getFilterString() == "exclude") {
                andFilter->addChild(childFilter);
            }
        }

        // get next child
        child = dynamic_cast<TagTreeNodeRadio*>(child->nextSibling());
    }


    return rootFilter;



    //========================

/*    // the filter set on a radiogroup must always be 'or'-linked
    FilterNode* filterRootNode = new FilterNodeOpOr(0);

    // loop over all radio children and add them to the filterRootNode
    TagTreeNodeRadio* child = dynamic_cast<TagTreeNodeRadio*>(this->firstChild());
    while( child ) {

        // get the filter of the current child
        FilterNode* childFilter = child->subfilter();
        if (childFilter) {
            filterRootNode->addChild(childFilter);
        }

        // get next child
        child = dynamic_cast<TagTreeNodeRadio*>(child->nextSibling());
    }

    if (filterRootNode->childrenCount()) {
        return filterRootNode;
    }

    return 0;
    */
}


void TagTreeNodeRadioGroup::leftClicked(__attribute__((unused)) TagTree* tagTree, int column) {

    switch (column) {
    case TagTree::COLUMN_FILTER :
        // change state of the filter: exclude -> ignore -> include -> exclude -> ...
        switch (m_filterState) {
        case FILTERSTATE_EXCLUDE:
            m_filterState = FILTERSTATE_INCLUDE;
            break;
        case FILTERSTATE_IGNORE:
            m_filterState = FILTERSTATE_INCLUDE;
            break;
        case FILTERSTATE_INCLUDE:
            m_filterState = FILTERSTATE_EXCLUDE;
            break;
        }

        //now set all children to my state
        TagTreeNodeRadio* act = NULL;
        QListViewItem * myChild = firstChild();
        while( myChild ) {
            //ignore non Radio Items (if they would be a child)
            act = dynamic_cast<TagTreeNodeRadio*>(myChild);
            if (act) {

                if (m_filterState == FILTERSTATE_INCLUDE) {
                    act->applyFilterString("include");
                } else if (m_filterState == FILTERSTATE_EXCLUDE) {
                    act->applyFilterString("exclude");
                }

                act->repaint();
            }
            // and go on with the next child
            myChild = myChild->nextSibling();
        }

        // force redrawing of this listviewitem
        this->repaint();

        m_photobook->autoRefreshView();
        break;
    }
}


void TagTreeNodeRadioGroup::rightClicked(__attribute__((unused)) TagTree* tagTree, int column) {

    switch (column) {
    case TagTree::COLUMN_TEXT :
        if (m_contextMenu) {
            m_contextMenu->exec(QCursor::pos());
        }
        break;

    case TagTree::COLUMN_FILTER :
         // change state of the filter: exclude -> include -> ignore -> exclude -> ...
        switch (m_filterState) {
        case FILTERSTATE_EXCLUDE:
            m_filterState = FILTERSTATE_INCLUDE;
            break;
        case FILTERSTATE_IGNORE:
            m_filterState = FILTERSTATE_EXCLUDE;
            break;
        case FILTERSTATE_INCLUDE:
            m_filterState = FILTERSTATE_EXCLUDE;
            break;
        }

        //now set all children to my state
        TagTreeNodeRadio* act = NULL;
        QListViewItem * myChild = firstChild();
        while( myChild ) {
            //ignore non Radio Items (if they would be a child)
            act = dynamic_cast<TagTreeNodeRadio*>(myChild);
            if (act) {

                if (m_filterState == FILTERSTATE_INCLUDE) {
                    act->applyFilterString("include");
                } else if (m_filterState == FILTERSTATE_EXCLUDE) {
                    act->applyFilterString("exclude");
                }

                act->repaint();
            }
            // and go on with the next child
            myChild = myChild->nextSibling();
        }

        // force redrawing of this listviewitem
        this->repaint();

        m_photobook->autoRefreshView();
        break;
    }
}

/**
 * sets the selected tag to src
 *
 * This does the following: loop over all children and unset them _except_ for src
 * If src is null, all children will be unset.
 */
void TagTreeNodeRadioGroup::setSelectedTag(TagTreeNodeRadio* src)
{
    // get all selected files
    const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

    if (selectedFiles->count())
    {
        QPtrListIterator<KFileItem> it(*m_photobook->view()->fileView()->selectedItems());

        //now loop over all radio children and tag the selected items
        TagTreeNodeRadio* act = NULL;
        QListViewItem * myChild = firstChild();
        while( myChild ) {
            //ignore non Radio Items (if they would be a child)
            act = dynamic_cast<TagTreeNodeRadio*>(myChild);
            if (act) {
                it.toFirst();

                //loop over all selected items...
                for (; it.current(); ++it) {
                    TagNodeRadio* tagNode = dynamic_cast<TagNodeRadio*>(act->tagNode());
                    if (tagNode) {
                        File* selectedFile = dynamic_cast<File*>(it.current());
                        // ... and tag only the src (act == src!) and untag the others
                        tagNode->setTagged(selectedFile, (act == src));
                    }
                }
                //force a repaint
                act->repaint();
            }
            // and go on with the next child
            myChild = myChild->nextSibling();
        }
        //this dirtifies the photobook ignoring if anything has been changed actually
        // if this is bad, we should check if smth changed, as it is done in the boolean tag
        m_photobook->dirtyfy();
    }
}


void TagTreeNodeRadioGroup::updateFilterState(){

    int include = 0, exclude = 0;
    TagTreeNodeRadio* act = NULL;
    QListViewItem * myChild = firstChild();
    while( myChild ) {
        act = dynamic_cast<TagTreeNodeRadio*>(myChild);
        if (act) {

            QString s = act->getFilterString();

            if (s == "include")
                ++include;
            else if (s = "exclude")
                ++exclude;

            if (exclude > 0 && include > 0)
                break;
        }
            // and go on with the next child
        myChild = myChild->nextSibling();
    }

    if (include > 0 && exclude > 0)
        m_filterState = FILTERSTATE_IGNORE;
    else if (include > 0)
        m_filterState = FILTERSTATE_INCLUDE;
    else
        m_filterState = FILTERSTATE_EXCLUDE;

    repaint();
    m_photobook->dirtyfy();
}


void TagTreeNodeRadioGroup::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {

    switch (column) {
    case TagTree::COLUMN_TEXT :
        KListViewItem::paintCell(p, cg, column, width, alignment);
        break;

    case TagTree::COLUMN_FILTER :
        // paint the cell with the alternating background color
        p->fillRect(0, 0, width, this->height(), backgroundColor(2));

        // draw the checkbox in the center of the cell in the size of the font
        int size = p->fontInfo().pixelSize()+2;
        QRect rect((width - size + 4)/2, (  this->height()-size )/2, size, size);

        TreeHelper::drawCheckBox(p, cg, rect, m_filterState, true);

        break;
    }
}

