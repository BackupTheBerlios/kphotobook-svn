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

#ifndef TREEHELPER_H
#define TREEHELPER_H

#include <qdatetime.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qrect.h>


/**
 * Static helper class for KListView (Tree) subclasses.
 * This class is not instantiable and contains static helper methods only.
 *
 * CVS-ID $Id$
 */
class TreeHelper
{
    public:
        enum TRISTATE {
            NOT_CHECKED = -1,
            UNDEFINED = 0,
            CHECKED = 1
        };

    public:
        static void drawCheckBox(QPainter* p, const QColorGroup& cg, const QColor& bg, int width, int height, bool enabled, TRISTATE tristate);
        static void drawRadioButton(QPainter* p, const QColorGroup& cg, const QColor& bg, int width, int height, bool enabled, TRISTATE tristate);

        /**
         * Formats the given datetime according the currently set locale.
         */
        static QString formatDateTime(const QDateTime& dateTime);

    protected:
        TreeHelper()
        {}

        ~TreeHelper()
        {}

    private:
        // constants for tree
        static const int TREE_CHECKBOX_MAXSIZE;
};

#endif
