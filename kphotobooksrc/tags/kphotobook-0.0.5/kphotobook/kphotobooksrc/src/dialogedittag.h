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

#ifndef DIALOGEDITTAG_H
#define DIALOGEDITTAG_H

#include "kphotobook.h"

#include <kdialogbase.h>
#include <klineedit.h>
#include <kcombobox.h>

#include <qstring.h>
#include <qpushbutton.h>

class TagTreeNode;

/**
 * The dialog to create a now tag.
 *
 * CVS-ID $Id$
 */
class DialogEditTag : public KDialogBase {

Q_OBJECT

public:
    DialogEditTag(QWidget *parent, TagTreeNode* parentNode, KPhotoBook* photobook, const char *name);

    ~DialogEditTag();

    QString tagName() {
        return m_nameLineEdit->text();
    }

    QString tagIcon() {
        return m_iconLineEdit->text();
    }

private slots:
    void slotNameChanged(const QString& text);
    void slotIconTextChanged(const QString& text);
    void slotIconButtonClicked();

private:
    TagTreeNode* m_tagTreeNode;
    KPhotoBook* m_photobook;

    KComboBox* m_typeComboBox;
    KLineEdit* m_nameLineEdit;
    KLineEdit* m_iconLineEdit;
    QPushButton* m_iconButton;

    void validate();
};

#endif
