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

#include "dialogcreatetag.h"

#include "configuration.h"
#include "tagtreenode.h"
#include "tagnode.h"
#include "tagnodetitle.h"
#include "tagnodeboolean.h"

#include <klocale.h>
#include <kicondialog.h>
#include <kglobal.h>
#include <kiconloader.h>

#include <qgroupbox.h>
#include <qlayout.h>
#include <qsizepolicy.h>
#include <qlabel.h>
#include <qfile.h>


DialogCreateTag::DialogCreateTag(QWidget *parent, TagTreeNode* parentNode, const char *name)
    : KDialogBase(parent, name, true, "", KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, false )
    , m_parentNode(parentNode) {

    if (parentNode) {
        this->setCaption(i18n("Create tag"));
    } else {
        this->setCaption(i18n("Create toplevel tag"));
    }

    QWidget* mainPanel = new QWidget(this, "mainPanel");
    setMainWidget(mainPanel);
    QVBoxLayout* mainPanelLayout = new QVBoxLayout(mainPanel, 0, 5, "mainPanelLayout");
    mainPanelLayout->setAutoAdd(true);

    // parent
    if (parentNode) {
        // newTagGroup
        QGroupBox* parentTagGroup = new QGroupBox(i18n("Parent tag"), mainPanel, "parentTagGroup");
        QGridLayout* parentTagGroupLayout = new QGridLayout(parentTagGroup, 4, 4, 20, 5, "parentTagGroupLayout");

        parentTagGroupLayout->setRowSpacing(0, 10);

        // type
        QLabel* typeLabel = new QLabel(i18n("Type"), parentTagGroup, "typeLabel");
        parentTagGroupLayout->addWidget(typeLabel, 1, 0);

        KComboBox* typeComboBox = new KComboBox(false, parentTagGroup, "typeComboBox");
        typeComboBox->insertItem(parentNode->tagNode()->typeName());
        typeComboBox->setEnabled(false);
        parentTagGroupLayout->addMultiCellWidget(typeComboBox, 1, 1, 1, 2);

        // name
        QLabel* nameLabel = new QLabel(i18n("Name"), parentTagGroup, "nameLabel");
        parentTagGroupLayout->addWidget(nameLabel, 2, 0);

        KLineEdit* nameLineEdit = new KLineEdit(parentTagGroup, "nameLineEdit");
        nameLineEdit->setText(*parentNode->tagNode()->text());
        nameLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        nameLineEdit->setReadOnly(true);
        parentTagGroupLayout->addMultiCellWidget(nameLineEdit, 2, 2, 1, 2);

        // icon
        QLabel* iconLabel = new QLabel(i18n("Icon"), parentTagGroup, "iconLabel");
        parentTagGroupLayout->addWidget(iconLabel, 3, 0);

        KLineEdit* iconLineEdit = new KLineEdit(parentTagGroup, "iconLineEdit");
        iconLineEdit->setText(*parentNode->tagNode()->iconName());
        iconLineEdit->setMinimumWidth(300);
        iconLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        iconLineEdit->setReadOnly(true);
        parentTagGroupLayout->addWidget(iconLineEdit, 3, 1);

        QPushButton* iconButton = new QPushButton(i18n("Icon"), parentTagGroup, "iconButton");
        QIconSet iconSet = KGlobal::iconLoader()->loadIconSet(iconLineEdit->text(), KIcon::Small, Configuration::getInstance()->tagtreeIconSize(), true);
        iconButton->setIconSet(iconSet);
        iconButton->setText(QString::null);
        iconButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        iconButton->setEnabled(true);
        parentTagGroupLayout->addWidget(iconButton, 3, 2);
    }

    // newTagGroup
    QGroupBox* newTagGroup = new QGroupBox(i18n("New tag"), mainPanel, "newTagGroup");
    QGridLayout* newTagGroupLayout = new QGridLayout(newTagGroup, 4, 4, 20, 5, "newTagGroupLayout");

    newTagGroupLayout->setRowSpacing(0, 10);

    // type
    QLabel* typeLabel = new QLabel(i18n("Type"), newTagGroup, "typeLabel");
    newTagGroupLayout->addWidget(typeLabel, 1, 0);

    m_typeComboBox = new KComboBox(false, newTagGroup, "typeComboBox");
    m_typeComboBoxEntries = new QValueList<int>;
    if (!parentNode) {
        m_typeComboBox->insertItem(TagNode::tagNodeTypeName(TagNode::TYPE_TITLE));
        m_typeComboBoxEntries->append(TagNode::tagNodeTypeId(TagNode::TYPE_TITLE));
    }
    m_typeComboBox->insertItem(TagNode::tagNodeTypeName(TagNode::TYPE_BOOLEAN));
    m_typeComboBoxEntries->append(TagNode::tagNodeTypeId(TagNode::TYPE_BOOLEAN));
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

    // disable ok button
    validate();

    // set the focus
    m_nameLineEdit->setFocus();
}


DialogCreateTag::~DialogCreateTag() {
    delete m_typeComboBoxEntries;
}


int DialogCreateTag::tagType() {

    return (*m_typeComboBoxEntries)[m_typeComboBox->currentItem()];
}


void DialogCreateTag::slotNameChanged(__attribute__((unused)) const QString& text) {

    validate();
}


void DialogCreateTag::slotIconTextChanged(__attribute__((unused)) const QString& text) {

    validate();
}


void DialogCreateTag::slotIconButtonClicked() {

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


void DialogCreateTag::validate() {

    QString name(m_nameLineEdit->text());
    QIconSet folderIconSet = KGlobal::iconLoader()->loadIconSet(m_iconLineEdit->text(), KIcon::Small, Configuration::getInstance()->tagtreeIconSize(), true);

    if (!folderIconSet.isNull()) {
        m_iconButton->setIconSet(folderIconSet);
        m_iconButton->setText(QString::null);
    } else {
        QIconSet nullIconSet = QIconSet();
        m_iconButton->setIconSet(nullIconSet);
        m_iconButton->setText(i18n("Icon"));
    }

    this->enableButtonOK(!name.isEmpty() && (m_iconLineEdit->text().isEmpty() || !folderIconSet.isNull()));
}

#include "dialogcreatetag.moc"
