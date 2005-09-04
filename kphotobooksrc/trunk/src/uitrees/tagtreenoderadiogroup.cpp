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
#include "tagtreenoderadio.h"
#include "treehelper.h"
#include "../engine/file.h"
#include "../engine/filternodeopor.h"
#include "../engine/filternodeopand.h"
#include "../engine/filternodetagradio.h"
#include "../engine/tagnoderadio.h"
#include "../engine/tagnoderadiogroup.h"
#include "../kphotobook.h"
#include "../kphotobookview.h"
#include "../tracer/tracer.h"

#include <kfileiconview.h>
#include <kfileitem.h>


Tracer* TagTreeNodeRadioGroup::tracer = Tracer::getInstance("kde.kphotobook.uitrees", "TagTreeNodeRadioGroup");


TagTreeNodeRadioGroup::TagTreeNodeRadioGroup(TagTree* parent, TagNodeRadioGroup* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu)
{
}


TagTreeNodeRadioGroup::TagTreeNodeRadioGroup(TagTreeNode* parent, TagNodeRadioGroup* tagNode, KPhotoBook* photobook, KPopupMenu* contextMenu) :
    TagTreeNode(parent, photobook, tagNode, contextMenu)
{
}


TagTreeNodeRadioGroup::~TagTreeNodeRadioGroup()
{
}


FilterNode* TagTreeNodeRadioGroup::filter() const
{
    tracer->invoked(__func__);

    FilterNode* rootFilter = new FilterNodeOpAnd(0);
    FilterNode* andFilter = new FilterNodeOpAnd(rootFilter);
    FilterNode* orFilter = new FilterNodeOpOr(rootFilter);

    // loop over all radio children and add them to the filterRootNode
    TagTreeNodeRadio* child = dynamic_cast<TagTreeNodeRadio*>(this->firstChild());
    while( child ) {

       // get the filter of the current child
        FilterNode* childFilter = child->subfilter();
        if (childFilter) {

            if (child->filterState() == TagTreeNode::FILTERSTATE_INCLUDE) {
                orFilter->addChild(childFilter);
            }
            else if (child->filterString() == TagTreeNode::FILTERSTATE_EXCLUDE) {
                andFilter->addChild(childFilter);
            }
        }

        // get next child
        child = dynamic_cast<TagTreeNodeRadio*>(child->nextSibling());
    }

    return rootFilter;
}


void TagTreeNodeRadioGroup::leftClicked(TagTree* tagTree, int column)
{
    tracer->invoked(__func__);

    switch (column) {
        case TagTree::COLUMN_FILTER: {
            // be sure that the filter reflects the children settings!
            updateFilterState();

            TagTreeNode::leftClicked(tagTree, column);

            //now set all children to my state
            TagTreeNodeRadio* act = NULL;
            QListViewItem * myChild = firstChild();
            while( myChild ) {
                //ignore non Radio Items (if they would be a child)
                act = dynamic_cast<TagTreeNodeRadio*>(myChild);
                if (act) {
                    act->applyFilterState(m_filterState);
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
}


void TagTreeNodeRadioGroup::rightClicked( TagTree* tagTree, int column)
{
    tracer->invoked(__func__);

    switch (column) {
        case TagTree::COLUMN_TEXT: {
            TagTreeNode::rightClicked(tagTree, column);
            break;
        }
        case TagTree::COLUMN_FILTER: {
            TagTreeNode::rightClicked(tagTree, column);

            //now set all children to my state
            TagTreeNodeRadio* act = NULL;
            QListViewItem * myChild = firstChild();
            while( myChild ) {
                //ignore non Radio Items (if they would be a child)
                act = dynamic_cast<TagTreeNodeRadio*>(myChild);
                if (act) {
                    act->applyFilterState(m_filterState);
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
}


/**
 * sets the selected tag to src
 *
 * This does the following: loop over all children and unset them _except_ for src
 * If src is null, all children will be unset.
 */
void TagTreeNodeRadioGroup::setSelectedTag(TagTreeNodeRadio* src)
{
    tracer->invoked(__func__);

    // get all selected files
    const KFileItemList* selectedFiles = m_photobook->view()->fileView()->selectedItems();

    if (selectedFiles->count()) {
        QPtrListIterator<KFileItem> it(*m_photobook->view()->fileView()->selectedItems());

        //now loop over all radio children and tag the selected items
        TagTreeNodeRadio* act = NULL;
        QListViewItem * myChild = firstChild();
        while( myChild ) {
            //ignore non Radio Items (if they would be a child)
            act = dynamic_cast<TagTreeNodeRadio*>(myChild);
            if (act) {
                //this all is a little up and down... now set the selected item to selected mode,
                // the others to UNTAGGED
                act->setState(act == src);

                //loop over all selected items...
                for (it.toFirst(); it.current(); ++it) {
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

        //now loop through all of our parents to set their state according to my changes
        TagTreeNode* node = this;
        while (node) {
            node->recursiveFindTagged();
            node->repaint();
            node = dynamic_cast<TagTreeNode*>(node->parent());
        }

        //this dirtifies the photobook ignoring if anything has been changed actually
        // if this is bad, we should check if smth changed, as it is done in the boolean tag
        m_photobook->dirtyfy();
    }
}


void TagTreeNodeRadioGroup::updateFilterState()
{
    tracer->invoked(__func__);

    int includeFound = 0;
    int excludeFound = 0;
    int ignoreFound = 0;

    QListViewItem * myChild = firstChild();
    while( myChild ) {
        TagTreeNodeRadio* child = dynamic_cast<TagTreeNodeRadio*>(myChild);

        if (child) {

            switch (child->filterState()) {
                case TagTreeNode::FILTERSTATE_INCLUDE: {
                    includeFound = 1;
                    break;
                }
                case TagTreeNode::FILTERSTATE_EXCLUDE: {
                    excludeFound = 1;
                    break;
                }
                case TagTreeNode::FILTERSTATE_IGNORE: {
                    ignoreFound = 1;
                    break;
                }
            }

            // we can abort if there are different selections
            if ((includeFound + excludeFound + ignoreFound) > 1) {
                break;
            }
        }

        // and go on with the next child
        myChild = myChild->nextSibling();
    }

    if ((includeFound + excludeFound + ignoreFound) == 1) {
        if (includeFound) {
            m_filterState = FILTERSTATE_INCLUDE;
        } else if (excludeFound) {
            m_filterState = FILTERSTATE_EXCLUDE;
        } else {
            m_filterState = FILTERSTATE_IGNORE;
        }
    } else {
        // all radios are set to ignore...
        m_filterState = FILTERSTATE_IGNORE;
    }

    repaint();
    m_photobook->dirtyfy();
}


void TagTreeNodeRadioGroup::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    switch (column) {
        case TagTree::COLUMN_TEXT:
        case TagTree::COLUMN_VALUE: {
            TagTreeNode::paintCell(p, cg, column, width, alignment);
            break;
        }
        case TagTree::COLUMN_FILTER: {
            TreeHelper::drawCheckBox(p, cg, backgroundColor(TagTree::COLUMN_FILTER), width, this->height(), true, (TreeHelper::TRISTATE)m_filterState);
            break;
        }
    }
}

