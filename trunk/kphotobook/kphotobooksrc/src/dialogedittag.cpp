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

#include "dialogedittag.h"

#include "configuration.h"
#include "settings.h"
#include "tagtreenode.h"
#include "tagnode.h"

#include <klocale.h>
#include <kicondialog.h>
#include <kglobal.h>
#include <kiconloader.h>

#include <qgroupbox.h>
#include <qlayout.h>
#include <qsizepolicy.h>
#include <qlabel.h>
#include <qfile.h>


DialogEditTag::DialogEditTag(QWidget *parent, TagTreeNode* tagTreeNode, KPhotoBook* photobook, const char *name)
    : KDialogBase(parent, name, true, "", KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, false )
    , m_tagTreeNode(tagTreeNode)
    , m_photobook(photobook) {

    this->setCaption(i18n("Edit tag"));

    QWidget* mainPanel = new QWidget(this, "mainPanel");
    setMainWidget(mainPanel);
    QVBoxLayout* mainPanelLayout = new QVBoxLayout(mainPanel, 0, 5, "mainPanelLayout");
    mainPanelLayout->setAutoAdd(true);

    // newTagGroup
    QGroupBox* newTagGroup = new QGroupBox(i18n("Edit tag"), mainPanel, "newTagGroup");
    QGridLayout* newTagGroupLayout = new QGridLayout(newTagGroup, 4, 4, 20, 5, "newTagGroupLayout");

    newTagGroupLayout->setRowSpacing(0, 10);

    // type
    QLabel* typeLabel = new QLabel(i18n("Type"), newTagGroup, "typeLabel");
    newTagGroupLayout->addWidget(typeLabel, 1, 0);

    m_typeComboBox = new KComboBox(false, newTagGroup, "typeComboBox");
    m_typeComboBox->insertItem(tagTreeNode->tagNode()->typeName());
    m_typeComboBox->setEnabled(false);
    newTagGroupLayout->addMultiCellWidget(m_typeComboBox, 1, 1, 1, 2);

    // name
    QLabel* nameLabel = new QLabel(i18n("Name"), newTagGroup, "nameLabel");
    newTagGroupLayout->addWidget(nameLabel, 2, 0);

    m_nameLineEdit = new KLineEdit(newTagGroup, "nameLineEdit");
    m_nameLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QObject::connect(m_nameLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotNameChanged(const QString&)));
    newTagGroupLayout->addMultiCellWidget(m_nameLineEdit, 2, 2, 1, 2);

    // icon
    QLabel* iconLabel = new QLabel(i18n("Icon"), newTagGroup, "iconLabel");
    newTagGroupLayout->addWidget(iconLabel, 3, 0);

    m_iconLineEdit = new KLineEdit(newTagGroup, "iconLineEdit");
    m_iconLineEdit->setMinimumWidth(300);
    m_iconLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QObject::connect(m_iconLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotIconTextChanged(const QString&)));
    newTagGroupLayout->addWidget(m_iconLineEdit, 3, 1);

    m_iconButton = new QPushButton(i18n("Icon"), newTagGroup, "iconButton");
    m_iconButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    newTagGroupLayout->addWidget(m_iconButton, 3, 2);

    QObject::connect(m_iconButton, SIGNAL(clicked()), this, SLOT(slotIconButtonClicked()));

    // spacer
    QWidget* spacer = new QWidget(mainPanel, "spacer");
    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // set the texts
    m_nameLineEdit->setText(*tagTreeNode->tagNode()->text());
    m_iconLineEdit->setText(*tagTreeNode->tagNode()->iconName());

    // validate
    validate();

    // set the focus
    m_nameLineEdit->setFocus();
}


DialogEditTag::~DialogEditTag() {

}


void DialogEditTag::slotNameChanged(__attribute__((unused)) const QString& text) {

    validate();
}


void DialogEditTag::slotIconTextChanged(__attribute__((unused)) const QString& text) {

    validate();
}


void DialogEditTag::slotIconButtonClicked() {

    KIconDialog* iconDialog = new KIconDialog(this, "iconDialog");
    iconDialog->setup(
        KIcon::Small,
        KIcon::Application,
        true // strictIconSize
    );
    QString iconName = iconDialog->openDialog();

    if (!iconName.isEmpty()) {
        m_iconLineEdit->setText(iconName);
    }

    validate();
}


void DialogEditTag::validate() {

    QIconSet folderIconSet = KGlobal::iconLoader()->loadIconSet(m_iconLineEdit->text(), KIcon::Small, Settings::tagTreeIconSize(), true);

    if (!folderIconSet.isNull()) {
        m_iconButton->setIconSet(folderIconSet);
        m_iconButton->setText(QString::null);
    } else {
        QIconSet nullIconSet = QIconSet();
        m_iconButton->setIconSet(nullIconSet);
        m_iconButton->setText(i18n("Icon"));
    }

    QString currentName(*m_tagTreeNode->tagNode()->text());
    QString newName(m_nameLineEdit->text());
    // the name is valid if it was not changed
    bool nameIsValid = (newName == currentName);
    if (!nameIsValid) {
        // the name is valid only, if there is no sibling with the same name
        nameIsValid = !newName.isEmpty() && m_photobook->isTagTextValid(dynamic_cast<TagTreeNode*>(m_tagTreeNode->parent()), newName);
    }

    this->enableButtonOK(nameIsValid && (m_iconLineEdit->text().isEmpty() || !folderIconSet.isNull()));
}

#include "dialogedittag.moc"
