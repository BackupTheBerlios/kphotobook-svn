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

#ifndef DIALOGMANAGETAG_H
#define DIALOGMANAGETAG_H

#include "../kphotobook.h"

#include <kdialogbase.h>
#include <klineedit.h>
#include <kcombobox.h>

#include <qstring.h>
#include <qpushbutton.h>
#include <qvaluelist.h>
#include <qcheckbox.h>

class TagTreeNode;

/**
 * The dialog to create a now tag.
 *
 * CVS-ID $Id: dialogcreatetag.h 284 2005-03-31 20:03:08Z choenig $
 */
class DialogManageTag : public KDialogBase {

Q_OBJECT

public:
    enum Mode {
        MODE_CREATE_TAG,
        MODE_EDIT_TAG
    };

public:
    DialogManageTag(QWidget *parent, Mode mode, TagTreeNode* parentNode, TagTreeNode* tagTreeNode, KPhotoBook* photobook, const char *name);

    ~DialogManageTag();

    TagNode::Type tagType();

    QString tagName() {
        return m_nameLineEdit->text();
    }

    QString tagComment() {
        return m_commentLineEdit->text();
    }

    QString tagIcon() {
        return m_iconLineEdit->text();
    }

    bool tagSecret() {
        return m_chkSecret->isChecked();
    }


private slots:
    void slotNameChanged(const QString& text);
    void slotIconTextChanged(const QString& text);
    void slotIconButtonClicked();

private:

    void fillTypeCombo(TagTreeNode* parentNode);

    Mode m_mode;

    TagTreeNode* m_parentNode;
    TagTreeNode* m_tagTreeNode;

    KPhotoBook* m_photobook;

    KComboBox* m_typeComboBox;
    QValueList<TagNode::Type>* m_typeComboBoxEntries;

    KLineEdit* m_nameLineEdit;
    KLineEdit* m_commentLineEdit;
    KLineEdit* m_iconLineEdit;
    QPushButton* m_iconButton;
    QCheckBox* m_chkSecret;

    void validate();
};

#endif
