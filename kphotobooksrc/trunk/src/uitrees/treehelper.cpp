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

#include "treehelper.h"

#include "../constants.h"

#include <kapp.h>

#include <qstyle.h>
#include <qcheckbox.h>
#include <qradiobutton.h>


void TreeHelper::drawCheckBox(QPainter* p, const QColorGroup& cg, QRect rect, bool checked, bool enabled)
{
    // we force the checkbox to a maximu size of Constants::TREE_CHECKBOX_MAXSIZE
    if (rect.height() > Constants::TREE_CHECKBOX_MAXSIZE) {
        rect.setTop(2 + (rect.height() - Constants::TREE_CHECKBOX_MAXSIZE) / 2);
        rect.setLeft(rect.left() + (rect.width() - Constants::TREE_CHECKBOX_MAXSIZE) / 2);
        rect.setHeight(Constants::TREE_CHECKBOX_MAXSIZE);
        rect.setWidth(Constants::TREE_CHECKBOX_MAXSIZE);
    }

    static QCheckBox checkBox(0);
    checkBox.setChecked(checked);
    checkBox.setEnabled(enabled);

    QStyle& style = KApplication::kApplication()->style();

    // copied from qcheckbox.cpp
    QStyle::SFlags flags = QStyle::Style_Default;
    if ( checkBox.isEnabled() )
        flags |= QStyle::Style_Enabled;
    if ( checkBox.hasFocus() )
        flags |= QStyle::Style_HasFocus;
    if ( checkBox.isDown() )
        flags |= QStyle::Style_Down;
    if ( checkBox.hasMouse() )
        flags |= QStyle::Style_MouseOver;
    if ( checkBox.state() == QButton::On )
        flags |= QStyle::Style_On;
    else if ( checkBox.state() == QButton::Off )
        flags |= QStyle::Style_Off;
    else if ( checkBox.state() == QButton::NoChange )
        flags |= QStyle::Style_NoChange;

    // draw the checkbox
    style.drawControl(QStyle::CE_CheckBox, p, &checkBox, rect, cg, flags);
}


void TreeHelper::drawCheckBox(QPainter* p, const QColorGroup& cg, QRect rect, int tristate, bool enabled)
{
    // we force the checkbox to a maximu size of Constants::TREE_CHECKBOX_MAXSIZE
    if (rect.height() > Constants::TREE_CHECKBOX_MAXSIZE) {
        rect.setTop(2 + (rect.height() - Constants::TREE_CHECKBOX_MAXSIZE) / 2);
        rect.setLeft(rect.left() + (rect.width() - Constants::TREE_CHECKBOX_MAXSIZE) / 2);
        rect.setHeight(Constants::TREE_CHECKBOX_MAXSIZE);
        rect.setWidth(Constants::TREE_CHECKBOX_MAXSIZE);
    }

    static QCheckBox checkBox(0);
    if (tristate == 0) {
        checkBox.setTristate(true);
        checkBox.setNoChange();
    } else {
        checkBox.setChecked(tristate > 0);
    }
    checkBox.setEnabled(enabled);

    QStyle& style = KApplication::kApplication()->style();

    // copied from qcheckbox.cpp
    QStyle::SFlags flags = QStyle::Style_Default;
    if ( checkBox.isEnabled() )
        flags |= QStyle::Style_Enabled;
    if ( checkBox.hasFocus() )
        flags |= QStyle::Style_HasFocus;
    if ( checkBox.isDown() )
        flags |= QStyle::Style_Down;
    if ( checkBox.hasMouse() )
        flags |= QStyle::Style_MouseOver;
    if ( checkBox.state() == QButton::On )
        flags |= QStyle::Style_On;
    else if ( checkBox.state() == QButton::Off )
        flags |= QStyle::Style_Off;
    else if ( checkBox.state() == QButton::NoChange )
        flags |= QStyle::Style_NoChange;

    // draw the checkbox
    style.drawControl(QStyle::CE_CheckBox, p, &checkBox, rect, cg, flags);
}


void TreeHelper::drawRadioButton(QPainter* p, const QColorGroup& cg, QRect rect, int tristate, bool enabled)
{
    rect.setTop(2 + (rect.height() - Constants::TREE_CHECKBOX_MAXSIZE) / 2);
    rect.setLeft(rect.left() + (rect.width() - Constants::TREE_CHECKBOX_MAXSIZE) / 2);
    rect.setHeight(Constants::TREE_CHECKBOX_MAXSIZE - 2);
    rect.setWidth(Constants::TREE_CHECKBOX_MAXSIZE - 2);

    static QRadioButton radioBtn(0);

    radioBtn.setEnabled(enabled);

    //TODO how to emulate tristate here?
    if (tristate == 0) {
        radioBtn.setChecked(true);
        radioBtn.setEnabled(false);
    } else {
        radioBtn.setChecked(tristate > 0);
    }
    

    QStyle& style = KApplication::kApplication()->style();

    QStyle::SFlags flags = QStyle::Style_Default;
    if ( radioBtn.isEnabled() )
        flags |= QStyle::Style_Enabled;
    if ( radioBtn.hasFocus() )
        flags |= QStyle::Style_HasFocus;
    if ( radioBtn.isDown() )
        flags |= QStyle::Style_Down;
    if ( radioBtn.hasMouse() )
        flags |= QStyle::Style_MouseOver;
    if ( radioBtn.state() == QButton::On )
        flags |= QStyle::Style_On;
    else if ( radioBtn.state() == QButton::Off )
        flags |= QStyle::Style_Off;
    else if ( radioBtn.state() == QButton::NoChange )
        flags |= QStyle::Style_NoChange;
    
    style.drawControl(QStyle::CE_RadioButton, p, &radioBtn, rect, cg, flags);
}


void TreeHelper::drawRadioButton(QPainter* p, const QColorGroup& cg, QRect rect, bool checked, bool enabled) 
{
    // we force the checkbox to a maximu size of Constants::TREE_CHECKBOX_MAXSIZE
    if (rect.height() > Constants::TREE_CHECKBOX_MAXSIZE) {
        rect.setTop(2 + (rect.height() - Constants::TREE_CHECKBOX_MAXSIZE) / 2);
        rect.setLeft(rect.left() + (rect.width() - Constants::TREE_CHECKBOX_MAXSIZE) / 2);
        rect.setHeight(Constants::TREE_CHECKBOX_MAXSIZE - 1);
        rect.setWidth(Constants::TREE_CHECKBOX_MAXSIZE - 1);
    }

    static QRadioButton radioBtn(0);
    
    radioBtn.setChecked(checked);
    radioBtn.setEnabled(enabled);
    
    QStyle& style = KApplication::kApplication()->style();
    
    // copied from qcheckbox.cpp
    QStyle::SFlags flags = QStyle::Style_Default;
    if ( radioBtn.isEnabled() )
        flags |= QStyle::Style_Enabled;
    if ( radioBtn.hasFocus() )
        flags |= QStyle::Style_HasFocus;
    if ( radioBtn.isDown() )
        flags |= QStyle::Style_Down;
    if ( radioBtn.hasMouse() )
        flags |= QStyle::Style_MouseOver;
    if ( radioBtn.state() == QButton::On )
        flags |= QStyle::Style_On;
    else if ( radioBtn.state() == QButton::Off )
        flags |= QStyle::Style_Off;
    else if ( radioBtn.state() == QButton::NoChange )
        flags |= QStyle::Style_NoChange;
    
    // draw the checkbox
    style.drawControl(QStyle::CE_RadioButton, p, &radioBtn, rect, cg, flags); 
}

