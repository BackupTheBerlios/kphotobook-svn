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
#include "../engine/file.h"
#include "../engine/filternodeopor.h"
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

    // the filter set on a radiogroup must always be 'or'-linked
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

    return filterRootNode;
}


void TagTreeNodeRadioGroup::rightClicked(__attribute__((unused)) TagTree* tagTree, int column) {
    
    switch (column) {
    case TagTree::COLUMN_TEXT :
        if (m_contextMenu) {
            m_contextMenu->exec(QCursor::pos());
        }
        break;
        
    case TagTree::COLUMN_FILTER :
        break;
    }
}

/**
 * sets the selected tag to src
 *
 * This does the following: loop over all children and unset them _except_ for src
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
