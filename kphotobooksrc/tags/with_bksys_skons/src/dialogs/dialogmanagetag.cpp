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

#include "dialogmanagetag.h"

#include "../engine/tagnode.h"
#include "../engine/tagnodetitle.h"
#include "../engine/tagnodeboolean.h"
#include "../settings/settings.h"
#include "../uitrees/tagtreenode.h"

#include <kcombobox.h>
#include <kglobal.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>

#include <qcheckbox.h>
#include <qfile.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qvaluelist.h>
#include <qwhatsthis.h>


DialogManageTag::DialogManageTag(QWidget *parent, Mode mode, TagTreeNode* parentNode, TagTreeNode* tagTreeNode, KPhotoBook* photobook, const char *name) :
    KDialogBase(parent, name, true, "", KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, false ),
    m_mode(mode),
    m_parentNode(parentNode),
    m_tagTreeNode(tagTreeNode),
    m_photobook(photobook),
    m_typeComboBox(0),
    m_typeComboBoxEntries(0),
    m_nameLineEdit(0),
    m_commentLineEdit(0),
    m_iconLineEdit(0),
    m_iconButton(0),
    m_chkSecret(0)
{
    if (mode == MODE_CREATE_TAG) {
        if (parentNode) {
            this->setCaption(i18n("Create tag"));
        } else {
            this->setCaption(i18n("Create toplevel tag"));
        }
    } else {
        this->setCaption(i18n("Edit tag"));
    }

    QWidget* mainPanel = new QWidget(this, "mainPanel");
    setMainWidget(mainPanel);
    QVBoxLayout* mainPanelLayout = new QVBoxLayout(mainPanel, 0, 5, "mainPanelLayout");

    // parent
    if (mode == MODE_CREATE_TAG && parentNode) {
        // newTagGroup
        QGroupBox* parentTagGroup = new QGroupBox(i18n("Parent tag"), mainPanel, "parentTagGroup");
        mainPanelLayout->add(parentTagGroup);
        QGridLayout* parentTagGroupLayout = new QGridLayout(parentTagGroup, 5, 3, 20, 5, "parentTagGroupLayout");

        parentTagGroupLayout->setRowSpacing(0, 10);

        // type
        QLabel* typeLabel = new QLabel(i18n("Type"), parentTagGroup, "typeLabel");
        parentTagGroupLayout->addWidget(typeLabel, 0, 0);

        KComboBox* typeComboBox = new KComboBox(false, parentTagGroup, "typeComboBox");
        typeComboBox->insertItem(parentNode->tagNode()->typeName());
        typeComboBox->setEnabled(false);
        parentTagGroupLayout->addMultiCellWidget(typeComboBox, 0, 0, 1, 2);

        // name
        QLabel* nameLabel = new QLabel(i18n("Name"), parentTagGroup, "nameLabel");
        parentTagGroupLayout->addWidget(nameLabel, 1, 0);

        KLineEdit* nameLineEdit = new KLineEdit(parentTagGroup, "nameLineEdit");
        nameLineEdit->setText(*parentNode->tagNode()->text());
        nameLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        nameLineEdit->setReadOnly(true);
        parentTagGroupLayout->addMultiCellWidget(nameLineEdit, 1, 1, 1, 2);

        // comment
        QLabel* commentLabel = new QLabel(i18n("Comment"), parentTagGroup, "commentLabel");
        parentTagGroupLayout->addWidget(commentLabel, 2, 0);

        KLineEdit* commentLineEdit = new KLineEdit(parentTagGroup, "commentLineEdit");
        commentLineEdit->setText(*parentNode->tagNode()->comment());
        commentLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        commentLineEdit->setReadOnly(true);
        parentTagGroupLayout->addMultiCellWidget(commentLineEdit, 2, 2, 1, 2);

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
        QIconSet iconSet = KGlobal::iconLoader()->loadIconSet(iconLineEdit->text(), KIcon::Small, Settings::tagTreeIconSize(), true);
        iconButton->setIconSet(iconSet);
        iconButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        iconButton->setEnabled(true);
        parentTagGroupLayout->addWidget(iconButton, 3, 2);

        QLabel* iconSecret = new QLabel(i18n("Secret"), parentTagGroup, "iconSecret");
        parentTagGroupLayout->addWidget(iconSecret, 4, 0);

        QCheckBox* chk = new QCheckBox(parentTagGroup);
        parentTagGroupLayout->addWidget(chk, 4, 1);
        chk->setChecked(parentNode->tagNode()->secret());
        chk->setEnabled(false);

    }

    // newTagGroup
    QGroupBox* newTagGroup = new QGroupBox(mode == MODE_CREATE_TAG ? i18n("New tag") : i18n("Edit tag"), mainPanel, "newTagGroup");
    mainPanelLayout->add(newTagGroup);
    QGridLayout* newTagGroupLayout = new QGridLayout(newTagGroup, 5, 3, 20, 5, "newTagGroupLayout");

    newTagGroupLayout->setRowSpacing(0, 10);

    // type
    QString typeWhatsThis = i18n("<b>Type</b><p>There are several types of tags available. Chose a type that matches your needs best.</p><p><b>title</b>: A title is a kind of helper tag. You cannot filter by title tags. Their purpose is to give your tag tree some more structure.</p><p><b>boolean</b>: A boolean tag can be associated to an image or not, not supplementary data can be stored with this tag. You can filter for this tag.</p><p><b>string</b>: A string tag can hold text and numbers. You can for example write a description of your image.</p><p><b>optionlist</b>: An optin list is a list containing several options from which you can only use one.</p><p><b>datetime</b>: This tag can store a date and a time.</p>");

    QLabel* typeLabel = new QLabel(i18n("Type"), newTagGroup, "typeLabel");
    newTagGroupLayout->addWidget(typeLabel, 0, 0);
    QWhatsThis::add(typeLabel, typeWhatsThis);

    m_typeComboBox = new KComboBox(false, newTagGroup, "typeComboBox");
    // now fill the available items;
    if (mode == MODE_CREATE_TAG) {
        fillTypeCombo(parentNode);
    } else {
        m_typeComboBox->insertItem(tagTreeNode->tagNode()->typeName());
        m_typeComboBox->setEnabled(false);
    }

    newTagGroupLayout->addMultiCellWidget(m_typeComboBox, 0, 0, 1, 2);
    QWhatsThis::add(m_typeComboBox, typeWhatsThis);

    // name
    QString nameWhatsThis = i18n("<b>Name</b><p>Chose a name for the tag. There can be several tags with the same name if needed. You cannot use the name 'exif' for a tag.</p>");

    QLabel* nameLabel = new QLabel(i18n("Name"), newTagGroup, "nameLabel");
    newTagGroupLayout->addWidget(nameLabel, 1, 0);
    QWhatsThis::add(nameLabel, nameWhatsThis);

    m_nameLineEdit = new KLineEdit(newTagGroup, "nameLineEdit");
    m_nameLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QObject::connect(m_nameLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotValidate()));
    newTagGroupLayout->addMultiCellWidget(m_nameLineEdit, 1, 1, 1, 2);
    QWhatsThis::add(m_nameLineEdit, nameWhatsThis);

    // comment
    QString commentWhatsThis = i18n("<b>Comment</b><p>You can give your tags a comment. This can be used to provide further information to a tag. The comment will be displayed as toolTip when hovering over the tag name.</p>");

    QLabel* commentLabel = new QLabel(i18n("Comment"), newTagGroup, "commentLabel");
    newTagGroupLayout->addWidget(commentLabel, 2, 0);
    QWhatsThis::add(commentLabel, commentWhatsThis);

    m_commentLineEdit = new KLineEdit(newTagGroup, "commentLineEdit");
    m_commentLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    newTagGroupLayout->addMultiCellWidget(m_commentLineEdit, 2, 2, 1, 2);
    QWhatsThis::add(m_commentLineEdit, commentWhatsThis);


    // icon
    QString iconWhatsThis = i18n("<b>Icon</b><p>Chose an icon for your tag. It's easier and quicker to refind the tags. Click on the button to open the icon selection dialog.</p>");

    QLabel* iconLabel = new QLabel(i18n("Icon"), newTagGroup, "iconLabel");
    newTagGroupLayout->addWidget(iconLabel, 3, 0);
    QWhatsThis::add(iconLabel, iconWhatsThis);

    m_iconLineEdit = new KLineEdit(newTagGroup, "iconLineEdit");
    m_iconLineEdit->setMinimumWidth(300);
    m_iconLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QObject::connect(m_iconLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotValidate()));
    newTagGroupLayout->addWidget(m_iconLineEdit, 3, 1);
    QWhatsThis::add(m_iconLineEdit, iconWhatsThis);

    m_iconButton = new QPushButton(i18n("Icon"), newTagGroup, "iconButton");
    m_iconButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    newTagGroupLayout->addWidget(m_iconButton, 3, 2);
    QWhatsThis::add(m_iconButton, iconWhatsThis);

    QObject::connect(m_iconButton, SIGNAL(clicked()), this, SLOT(slotIconButtonClicked()));


    // secret
    QString secretWhatsThis = i18n("<b>Secret</b><p>If 'secret' is enabled, <i>KPhotoBook</i> will always ask you before you filter for images containing images with secret tags</i></p>");

    QLabel* iconSecret = new QLabel(i18n("Secret"), newTagGroup, "iconSecret");
    newTagGroupLayout->addWidget(iconSecret, 4, 0);
    QWhatsThis::add(iconSecret, secretWhatsThis);

    m_chkSecret = new QCheckBox(newTagGroup);
    newTagGroupLayout->addWidget(m_chkSecret, 4, 1);
    QWhatsThis::add(m_chkSecret, secretWhatsThis);

    // spacer
    mainPanelLayout->addStretch(1);

    if (mode == MODE_EDIT_TAG) {
        // set the texts
        m_nameLineEdit->setText(*tagTreeNode->tagNode()->text());
        m_commentLineEdit->setText(*tagTreeNode->tagNode()->comment());
        m_iconLineEdit->setText(*tagTreeNode->tagNode()->iconName());
        m_chkSecret->setChecked(tagTreeNode->tagNode()->secret());
    }

    // disable ok button
    slotValidate();

    // set the focus
    m_nameLineEdit->setFocus();
}


DialogManageTag::~DialogManageTag()
{
    delete m_typeComboBoxEntries;
}


TagNode::Type DialogManageTag::tagType() const
{
    return (*m_typeComboBoxEntries)[m_typeComboBox->currentItem()];
}


QString DialogManageTag::tagName() const
{
    return m_nameLineEdit->text();
}


QString DialogManageTag::tagComment() const
{
    return m_commentLineEdit->text();
}


QString DialogManageTag::tagIcon() const
{
    return m_iconLineEdit->text();
}


bool DialogManageTag::tagSecret() const
{
    return m_chkSecret->isChecked();
}


void DialogManageTag::fillTypeCombo(TagTreeNode* parentNode)
{
    m_typeComboBoxEntries = new QValueList<TagNode::Type>;

    // title tag is allowed only as top-level tag
    if (!parentNode) {
        m_typeComboBox->insertItem(TagNode::tagNodeTypeName(TagNode::TYPE_TITLE));
        m_typeComboBoxEntries->append(TagNode::TYPE_TITLE);
    }

    // each tag (but the title tag) can be top-level, below title and below other booleantag
    if (!parentNode
        || parentNode->tagNode()->typeId() == TagNode::TYPE_TITLE
        || parentNode->tagNode()->typeId() == TagNode::TYPE_BOOLEAN) {

        m_typeComboBox->insertItem(TagNode::tagNodeTypeName(TagNode::TYPE_BOOLEAN));
        m_typeComboBoxEntries->append(TagNode::TYPE_BOOLEAN);

        m_typeComboBox->insertItem(TagNode::tagNodeTypeName(TagNode::TYPE_STRING));
        m_typeComboBoxEntries->append(TagNode::TYPE_STRING);

        m_typeComboBox->insertItem(TagNode::tagNodeTypeName(TagNode::TYPE_RADIOGROUP));
        m_typeComboBoxEntries->append(TagNode::TYPE_RADIOGROUP);

        m_typeComboBox->insertItem(TagNode::tagNodeTypeName(TagNode::TYPE_DATETIME));
        m_typeComboBoxEntries->append(TagNode::TYPE_DATETIME);
    }

    // RADIO is only allowed below RADIOGROUP
    if (parentNode && parentNode->tagNode()->typeId() == TagNode::TYPE_RADIOGROUP) {
        m_typeComboBox->insertItem(TagNode::tagNodeTypeName(TagNode::TYPE_RADIO));
        m_typeComboBoxEntries->append(TagNode::TYPE_RADIO);
    }

}


void DialogManageTag::slotIconButtonClicked()
{
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

    slotValidate();
}


void DialogManageTag::slotValidate()
{
    QIconSet folderIconSet = KGlobal::iconLoader()->loadIconSet(m_iconLineEdit->text(), KIcon::Small, Settings::tagTreeIconSize(), true);

    if (!folderIconSet.isNull()) {
        m_iconButton->setIconSet(folderIconSet);
        m_iconButton->setText(QString::null);
    } else {
        QIconSet nullIconSet = QIconSet();
        m_iconButton->setIconSet(nullIconSet);
        m_iconButton->setText(i18n("Icon"));
    }

    bool nameIsValid = false;
    if (m_mode == MODE_CREATE_TAG) {
        QString name(m_nameLineEdit->text());
        nameIsValid = !name.isEmpty() && m_photobook->isTagTextValid(m_parentNode, name);
    } else {
        QString currentName(*m_tagTreeNode->tagNode()->text());
        QString newName(m_nameLineEdit->text());
        // the name is valid if it was not changed
        nameIsValid = (newName == currentName);
        if (!nameIsValid) {
            // the name is valid only, if there is no sibling with the same name
            nameIsValid = !newName.isEmpty() && m_photobook->isTagTextValid(m_parentNode, newName);
        }
    }

    this->enableButtonOK(nameIsValid && (m_iconLineEdit->text().isEmpty() || !folderIconSet.isNull()));
}

#include "dialogmanagetag.moc"
