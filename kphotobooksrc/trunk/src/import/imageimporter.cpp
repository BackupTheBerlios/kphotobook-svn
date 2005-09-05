/***************************************************************************
 *   Copyright (C) 2005 by Christian Hoenig                                *
 *   kphotobook@mail.hoenig.cc                                             *
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

#include "imageimporter.h"

#include "../settings/settings.h"
#include "../kphotobook.h"
#include "../engine/folder.h"
#include "../engine/engine.h"
#include "dlgimageimporterstatus.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <ksqueezedtextlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qurloperator.h>
#include <qdir.h>

#include <klocale.h>
#include <kdirselectdialog.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <kmountpoint.h>
#include <qregexp.h>
#include <kio/job.h>
#include <kautomount.h>

#include <libexif/exif-data.h>
#include <libexif/exif-ifd.h>
#include <libexif/exif-content.h>
#include <libexif/exif-entry.h>
#include <libexif/exif-tag.h>


///@fixme check why this sigsegvs
Tracer* ImageImporter::tracer = Tracer::getInstance("kde.kphotobook.import", "ImageImporter");

/*
 *  Constructs a ImageImporter as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ImageImporter::ImageImporter( KPhotoBook* parent, const char* name, bool modal, WFlags fl )
: KDialog( parent, name, modal, fl ), m_photobook(parent)
{
    setCaption(i18n("Image Importer"));

    m_bCurrentDeviceMounted = false;

    initGUI();
    initData();
}

ImageImporter::~ImageImporter()
{
    // no need to delete child widgets, Qt does it all for us
}



void ImageImporter::initGUI() {
    m_ImageImporterLayout = new QVBoxLayout( this, 11, 6, "ImageImporterLayout");

    m_grpSource = new QGroupBox( this, "grpSource" );
    m_grpSource->setTitle( i18n( "Source" ) );
    m_grpSource->setColumnLayout(0, Qt::Vertical );
    m_grpSource->layout()->setSpacing( 6 );
    m_grpSource->layout()->setMargin( 11 );
    m_grpSourceLayout = new QGridLayout( m_grpSource->layout() );
    m_grpSourceLayout->setAlignment( Qt::AlignTop );


    m_cmbSourceDevice = new QComboBox( false, m_grpSource, "cmbSourceDevice" );
    m_grpSourceLayout->addWidget( new QLabel(m_cmbSourceDevice, i18n( "Device:" ), m_grpSource, "lblDevice" ), 0, 0 );
    connect(m_cmbSourceDevice, SIGNAL(highlighted(const QString& )), SLOT(slotUpdateMountStatus()));
    //     m_cmbSourceDevice->setEditable( true );
    m_grpSourceLayout->addWidget( m_cmbSourceDevice, 0, 1 );
    QWhatsThis::add(m_cmbSourceDevice, i18n( "Here you can select a device which you can <i>mount</i> and then import your images from.<br>"
                                             "Mountpoints are only listed, if they are mountable by this user!" ));

    m_btnMountDevice = new QPushButton( m_grpSource, "btnMountDevice" );
    m_btnMountDevice->setText( i18n( "Mount" ) );
    //m_btnMountDevice->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)0, 0, 0, m_btnMountDevice->sizePolicy().hasHeightForWidth() ) );
    m_btnMountDevice->setToggleButton( TRUE );
    connect(m_btnMountDevice, SIGNAL(clicked()), SLOT(slotMountDevice()));
    m_grpSourceLayout->addWidget( m_btnMountDevice, 0, 2 );
    QWhatsThis::add(m_btnMountDevice, i18n( "Mounts/Unmounts the currently selected mountpoint." ));

    m_cmbSourceFolder = new QComboBox( m_grpSource, "txtSourceFolder" );
    m_cmbSourceFolder->setEditable(true);
    m_cmbSourceFolder->setDuplicatesEnabled( false );
    m_grpSourceLayout->addWidget( new QLabel(m_cmbSourceFolder, i18n( "Folder:" ), m_grpSource, "lblFolder" ), 1, 0 );
    m_grpSourceLayout->addWidget( m_cmbSourceFolder, 1, 1 );
    QWhatsThis::add(m_cmbSourceFolder, i18n( "Enter the name of the folder, from where you want to import your Images." ));

    m_btnSourceFolder = new QPushButton( m_grpSource, "btnSourceFolder" );
    m_btnSourceFolder->setText( i18n( "..." ) );
    connect(m_btnSourceFolder, SIGNAL(clicked()), SLOT(slotBtnSourceFolder()));
    m_grpSourceLayout->addWidget( m_btnSourceFolder, 1, 2 );
    QWhatsThis::add(m_btnSourceFolder, i18n( "Select the import folder." ));

    QFrame* line = new QFrame( m_grpSource, "lineSource1" );
    line->setFrameShape( QFrame::HLine );
    line->setFrameShadow( QFrame::Sunken );
    line->setFrameShape( QFrame::HLine );
    m_grpSourceLayout->addMultiCellWidget( line, 2, 2, 0, 2 );

    m_txtSourceFilename = new QLineEdit( m_grpSource, "txtSourceFilename" );
    m_grpSourceLayout->addWidget( new QLabel(m_txtSourceFilename, i18n("Filename:"), m_grpSource, "lblSourceFilename" ), 3, 0 );
    m_grpSourceLayout->addMultiCellWidget( m_txtSourceFilename, 3, 3, 1, 2 );
    QWhatsThis::add(m_txtSourceFilename, i18n( "Contains a Regular Expression to match the files you want to import."
                                               "You can use captures here '()'. These will replace $0 and $1, $2, etc... "
                                               "in the output names." ));


    m_chkIgnoreCase = new QCheckBox("Ignore case", m_grpSource);
    m_grpSourceLayout->addWidget( m_chkIgnoreCase, 4, 1 );
    QWhatsThis::add( m_chkIgnoreCase, i18n( "Toggles, whether the filename-regexp should be matched case sensitive or not" ) );

    line = new QFrame( m_grpSource, "lineSource2" );
    line->setFrameShape( QFrame::HLine );
    line->setFrameShadow( QFrame::Sunken );
    line->setFrameShape( QFrame::HLine );
    m_grpSourceLayout->addMultiCellWidget( line, 2, 2, 0, 2 );
    m_grpSourceLayout->addMultiCellWidget( line, 5, 5, 0, 2 );

    m_chkSrcIncludeSubfolders = new QCheckBox( m_grpSource, "chkSrcIncludeSubfolders" );
    m_chkSrcIncludeSubfolders->setText( i18n( "Include subfolders" ) );
    m_chkSrcIncludeSubfolders->setChecked( true );
    m_grpSourceLayout->addMultiCellWidget( m_chkSrcIncludeSubfolders, 6, 6, 1, 2 );
    QWhatsThis::add(m_chkSrcIncludeSubfolders, i18n( "If you also want to import files placed in subfolders of the given basefolder, check this." ));

    m_chkSrcRemoveFilesFromSrc = new QCheckBox( m_grpSource, "chkSrcRemoveFilesFromSource" );
    m_chkSrcRemoveFilesFromSrc->setText( i18n( "Remove successfully transfered files from given source (i.e. move the files)" ) );
    m_chkSrcRemoveFilesFromSrc->setChecked( true );
    m_grpSourceLayout->addMultiCellWidget( m_chkSrcRemoveFilesFromSrc, 7, 7, 1, 2 );
    QWhatsThis::add(m_chkSrcRemoveFilesFromSrc, i18n( "If you want the imported images to be removed from the source, check this." ));

    m_ImageImporterLayout->addWidget( m_grpSource );

    m_ImageImporterLayout->addItem(new QSpacerItem( 40, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));


    m_groupArchive = new QGroupBox( this, "groupArchive" );
    m_groupArchive->setTitle( i18n( "Archive" ) );
    m_groupArchive->setCheckable( true );
    m_groupArchive->setColumnLayout(0, Qt::Vertical );
    m_groupArchive->layout()->setSpacing( 6 );
    m_groupArchive->layout()->setMargin( 11 );
    m_groupArchiveLayout = new QGridLayout( m_groupArchive->layout() );
    m_groupArchiveLayout->setAlignment( Qt::AlignTop );
    QWhatsThis::add( m_groupArchive, i18n( "If you want to create a copy of your original images (for security reasons), select this option." ) );

    m_txtArchiveBaseFolder = new QLineEdit( m_groupArchive, "txtArchiveBaseFolder" );
    connect(m_txtArchiveBaseFolder, SIGNAL(textChanged(const QString& )), SLOT(slotUpdateArchiveExample()));
    m_groupArchiveLayout->addWidget( m_txtArchiveBaseFolder, 0, 1 );
    m_groupArchiveLayout->addWidget( new QLabel(m_txtArchiveBaseFolder, i18n( "Base Folder" ), m_groupArchive, "lblArchiveBaseFolder" ), 0, 0 );
    QPushButton* btn = new QPushButton(i18n( "..." ), m_groupArchive, "btnArhiveBaseFolder" );
    m_groupArchiveLayout->addWidget( btn, 0, 2 );
    connect(btn, SIGNAL(clicked()), this, SLOT(slotBtnArchiveBaseFolder()));
    QWhatsThis::add( m_txtArchiveBaseFolder, i18n( "This is the base folder, all your archived images will be copied to." ) );

    m_txtArchiveSubfolders = new QLineEdit( m_groupArchive, "txtArchiveSubfolders" );
    connect(m_txtArchiveSubfolders, SIGNAL(textChanged(const QString& )), SLOT(slotUpdateArchiveExample()));
    m_groupArchiveLayout->addWidget( m_txtArchiveSubfolders, 1, 1 );
    m_groupArchiveLayout->addWidget( new QLabel(m_txtArchiveSubfolders, i18n( "Subfolders" ), m_groupArchive, "blbArchiveSubfolders" ), 1, 0 );
    QWhatsThis::add(m_txtArchiveSubfolders, helpText("For every image a subfolder is created in the base folder."));

    m_txtArchiveFilename = new QLineEdit( m_groupArchive, "txtArchiveFilename" );
    connect(m_txtArchiveFilename, SIGNAL(textChanged(const QString& )), SLOT(slotUpdateArchiveExample()));
    m_groupArchiveLayout->addWidget( m_txtArchiveFilename, 2, 1 );
    m_groupArchiveLayout->addWidget( new QLabel(m_txtArchiveFilename, i18n( "File RegExp" ), m_groupArchive, "lblArchiveFilename" ), 2, 0 );
    QWhatsThis::add(m_txtArchiveFilename, helpText("This is the name of archived image in the subfolder."));

    m_chkArchiveLowercase = new QCheckBox( m_groupArchive, "chkArchiveLowercase" );
    m_chkArchiveLowercase->setText( i18n( "lowercase filenames" ) );
    m_chkArchiveLowercase->setChecked( true );
    m_groupArchiveLayout->addMultiCellWidget( m_chkArchiveLowercase, 3, 3, 1, 2 );
    QWhatsThis::add(m_chkArchiveLowercase, i18n( "Should your images be translated to lowercase finally?" ));


    line = new QFrame( m_groupArchive, "lineArchive" );
    line->setFrameShape( QFrame::HLine );
    line->setFrameShadow( QFrame::Sunken );
    line->setFrameShape( QFrame::HLine );
    m_groupArchiveLayout->addMultiCellWidget( line, 4, 4, 0, 2 );

    m_groupArchiveLayout->addWidget( new QLabel(i18n( "Example" ), m_groupArchive, "lblArchiveExample" ), 5, 0 );

    m_lblArchiveExampleString = new KSqueezedTextLabel( m_groupArchive, "kSqueezedTextLabel1" );
    m_groupArchiveLayout->addMultiCellWidget( m_lblArchiveExampleString, 5, 5, 1, 2 );

    m_ImageImporterLayout->addWidget( m_groupArchive );

    m_groupDest = new QGroupBox( this, "groupDest" );
    m_groupDest->setTitle( i18n( "Destination" ) );
    m_groupDest->setCheckable( FALSE );
    m_groupDest->setColumnLayout(0, Qt::Vertical );
    m_groupDest->layout()->setSpacing( 6 );
    m_groupDest->layout()->setMargin( 11 );
    m_groupDestLayout = new QGridLayout( m_groupDest->layout() );
    m_groupDestLayout->setAlignment( Qt::AlignTop );

    m_cmbDestBasefolder = new QComboBox( m_groupDest, "txtDestBasefolder" );
//     m_cmbDestBasefolder->setEditable( true );
    m_cmbDestBasefolder->setDuplicatesEnabled( false );
    connect(m_cmbDestBasefolder, SIGNAL(highlighted(int)), SLOT(slotUpdateDestExample()));
    m_groupDestLayout->addWidget( m_cmbDestBasefolder, 0, 1 );
    m_groupDestLayout->addWidget( new QLabel(m_cmbDestBasefolder, i18n( "Base Folder" ), m_groupDest, "lblDestBaseFolder" ), 0, 0 );
    btn = new QPushButton(i18n( "..." ), m_groupDest, "btnDestBaseFolder" );
    m_groupDestLayout->addWidget( btn, 0, 2 );
    //this is a temporary hack: it is intended, that someday the user can select an arbitrary folder to copy the images to,
    // which finally is added to the album. But currently the user only can select from folders here, which are already contained in
    // the album.
    btn->hide();
    connect(btn, SIGNAL(clicked()), SLOT(slotBtnDestBaseFolder()));


    m_txtDestSubfolders = new QLineEdit( m_groupDest, "txtDestSubfolders" );
    connect(m_txtDestSubfolders, SIGNAL(textChanged(const QString& )), SLOT(slotUpdateDestExample()));
    m_groupDestLayout->addWidget( m_txtDestSubfolders, 1, 1 );
    m_groupDestLayout->addWidget( new QLabel(m_txtDestSubfolders, i18n( "Subfolders" ), m_groupDest, "lblDestSubfolders" ), 1, 0 );
    QWhatsThis::add(m_txtDestSubfolders, helpText("Set the name of the subfolders of your images."));

    m_txtDestFilename = new QLineEdit( m_groupDest, "txtDestFilename" );
    connect(m_txtDestFilename, SIGNAL(textChanged(const QString& )), SLOT(slotUpdateDestExample()));
    m_groupDestLayout->addWidget( m_txtDestFilename, 2, 1 );
    m_groupDestLayout->addWidget( new QLabel(m_txtDestFilename, i18n( "Filename" ), m_groupDest, "lblDestFilename" ), 2, 0 );
    QWhatsThis::add(m_txtDestFilename, helpText("Set the name of your images."));

    m_chkDestLowercase = new QCheckBox( m_groupDest, "chkDestLowercase" );
    m_chkDestLowercase->setText( i18n( "lowercase filenames" ) );
    m_chkDestLowercase->setChecked( true );
    m_groupDestLayout->addMultiCellWidget( m_chkDestLowercase, 3, 3, 1, 2 );
    QWhatsThis::add(m_chkDestLowercase, i18n( "Should your images be translated to lowercase finally?" ));

    line = new QFrame( m_groupDest, "lineDest" );
    line->setFrameShape( QFrame::HLine );
    line->setFrameShadow( QFrame::Sunken );
    line->setFrameShape( QFrame::HLine );
    m_groupDestLayout->addMultiCellWidget( line, 4, 4, 0, 2 );

    m_groupDestLayout->addWidget( new QLabel(i18n( "Example" ), m_groupDest, "lblDestExample" ), 5, 0 );

    m_lblDestExampleString = new KSqueezedTextLabel( m_groupDest, "lblDestExampleString" );
    m_groupDestLayout->addMultiCellWidget( m_lblDestExampleString, 5, 5, 1, 2 );

    m_ImageImporterLayout->addWidget( m_groupDest );



    QHBoxLayout* btnBottom = new QHBoxLayout( 0, 0, 6, "btnBottom");
    btnBottom->addItem(new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));

    btn = new QPushButton(i18n( "Import" ), this, "import" );
    btnBottom->addWidget( btn );
    connect(btn, SIGNAL(clicked()), SLOT(slotImport()));

    btn = new QPushButton(i18n( "Quit" ), this, "quit" );
    btnBottom->addWidget( btn );
    connect(btn, SIGNAL(clicked()), SLOT(slotQuit()));

    btn = new QPushButton(i18n( "Cancel" ), this, "cancel" );
    btnBottom->addWidget( btn );
    connect(btn, SIGNAL(clicked()), SLOT(reject()));


    m_ImageImporterLayout->addLayout( btnBottom );

    resize( QSize(551, 519).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

void ImageImporter::initData() {
    const KMountPoint::List lst = KMountPoint::possibleMountPoints(KMountPoint::NeedMountOptions);
    for ( KMountPoint::List::ConstIterator it = lst.begin() ; it != lst.end() ; ++it ) {
        //so, I only show devices that are mountable by a user
        if (getuid() == 0 || (*it)->mountOptions().grep("user").size() > 0) {
            QString x = QString("%1 (%2)").arg((*it)->mountedFrom()).arg((*it)->mountPoint());
            m_cmbSourceDevice->insertItem(x);
        }
    }
    //after adding all available mountpoints, check the status of the current
    slotUpdateMountStatus();

    m_cmbSourceFolder->insertItem(Settings::imageImporterSourceFolder());
    m_cmbSourceFolder->setCurrentText(Settings::imageImporterSourceFolder());
    m_cmbSourceDevice->setCurrentText(Settings::imageImporterSourceDevice());
    m_txtSourceFilename->setText(Settings::imageImporterSourceFilenameRegexp());
    m_chkIgnoreCase->setChecked(Settings::imageImporterFilenameIgnoreCase());
    m_chkSrcIncludeSubfolders->setChecked(Settings::imageImporterSourceIncludeSubfolders());
    m_chkSrcRemoveFilesFromSrc->setChecked(Settings::imageImporterSourceRemoveAfterTransfer());

    m_groupArchive->setChecked(Settings::imageImporterArchiveImages());
    m_txtArchiveBaseFolder->setText(Settings::imageImporterArchiveBaseFolder());
    m_txtArchiveSubfolders->setText(Settings::imageImporterArchiveSubfolders());
    m_txtArchiveFilename->setText(Settings::imageImporterArchiveFilename());
    m_chkArchiveLowercase->setChecked(Settings::imageImporterArchiveLowercaseFilenames());

    //add all dirs from the album to the list of target folders
    Folder* sourceDir;
    for (
          sourceDir = m_photobook->engine()->sourceDirs()->first();
          sourceDir;
          sourceDir = m_photobook->engine()->sourceDirs()->next()
        )
    {
        m_cmbDestBasefolder->insertItem(sourceDir->dir()->absPath());
    }

    //m_cmbDestBasefolder->insertItem(Settings::imageImporterDestBaseFolder());
    m_cmbDestBasefolder->setCurrentText(Settings::imageImporterDestBaseFolder());
    m_txtDestSubfolders->setText(Settings::imageImporterDestSubfolders());
    m_txtDestFilename->setText(Settings::imageImporterDestFilenames());
    m_chkDestLowercase->setChecked(Settings::imageImporterDestLowercaseFilenames());

}


void ImageImporter::insertItemTo(QComboBox* cmb, const QString& str)
{
    for (int i = 0; i < cmb->count(); ++i) {
        if (cmb->text(i) == str) {
            return;
        }
    }
    cmb->insertItem(str);
}


void ImageImporter::slotUpdateMountStatus() {
    QString str = m_cmbSourceDevice->currentText();

    QString mntDev = getCurrentMountDevice();

    m_bCurrentDeviceMounted = false;
    //find out if the current selection is mounted
    const KMountPoint::List lst = KMountPoint::currentMountPoints();
    for ( KMountPoint::List::ConstIterator it = lst.begin() ; it != lst.end() ; ++it ) {
        if ((*it)->mountedFrom() == mntDev) {
            m_bCurrentDeviceMounted = true;
            break;
        }
    }
    m_btnMountDevice->setEnabled(true);
    m_btnMountDevice->setOn(m_bCurrentDeviceMounted);

    if (m_bCurrentDeviceMounted) {
        insertItemTo(m_cmbSourceFolder, getCurrentMountPoint());
        m_cmbSourceFolder->setCurrentText(getCurrentMountPoint());
    } else {
        for (int i = 0; i < m_cmbSourceFolder->count(); ++i) {
            if (m_cmbSourceFolder->text(i) == getCurrentMountPoint()) {
                m_cmbSourceFolder->removeItem( i );
                ///@todo set the Settings::blubb item current, if available
                break;
            }
        }
    }
}




void ImageImporter::slotMountDevice() {
    QString mntDev = getCurrentMountDevice();

    m_btnMountDevice->setEnabled(false);

    if (!m_bCurrentDeviceMounted) {
        KAutoMount* am = new KAutoMount(false, "", mntDev, "", "", false);
        connect(am, SIGNAL( finished() ), SLOT( slotUpdateMountStatus() ));
        connect(am, SIGNAL( error() ),    SLOT( slotUpdateMountStatus() ));
    } else {
        KAutoUnmount* am = new KAutoUnmount(mntDev, "");
        connect(am, SIGNAL( finished() ), SLOT( slotUpdateMountStatus() ));
        connect(am, SIGNAL( error() ),    SLOT( slotUpdateMountStatus() ));
    }
}


QString ImageImporter::getCurrentMountDevice() {
    QRegExp re("(.*) \\(.*\\)");
    if (re.exactMatch(m_cmbSourceDevice->currentText())) {
        return re.cap(1);
    }
    return "";
}

QString ImageImporter::getCurrentMountPoint() {
    QRegExp re("(.*) \\((.*)\\)");
    if (re.exactMatch(m_cmbSourceDevice->currentText())) {
        return re.cap(2);
    }
    return "";
}

void ImageImporter::slotBtnSourceFolder() {
    //KURL d = KDirSelectDialog::selectDirectory(Settings::imageImporterSourceFolder(), false, this);
    KURL d = KDirSelectDialog::selectDirectory(m_cmbSourceFolder->currentText(), false, this);
    if (d.isEmpty()) {
        return;
    }
    m_cmbSourceFolder->insertItem(d.path());
    m_cmbSourceFolder->setCurrentText(d.path());
    Settings::setImageImporterSourceFolder(d.path());
}


void ImageImporter::slotBtnArchiveBaseFolder() {
    KURL d = KDirSelectDialog::selectDirectory(Settings::imageImporterArchiveBaseFolder(), false, this);
    if (d.isEmpty()) {
        return;
    }
    m_txtArchiveBaseFolder->setText(d.path());
    Settings::setImageImporterArchiveBaseFolder(d.path());
}

void ImageImporter::slotBtnDestBaseFolder() {
    KURL d = KDirSelectDialog::selectDirectory(Settings::imageImporterDestBaseFolder(), false, this);
    if (d.isEmpty()) {
        return;
    }
    m_cmbDestBasefolder->insertItem(d.path());
    m_cmbDestBasefolder->setCurrentText(d.path());
    Settings::setImageImporterDestBaseFolder(d.path());
}

void ImageImporter::slotQuit() {
    slotSaveSettings();
    accept();
}



void ImageImporter::slotSaveSettings() {

    Settings::setImageImporterSourceFolder(m_cmbSourceFolder->currentText());
    Settings::setImageImporterSourceDevice(m_cmbSourceDevice->currentText());
    Settings::setImageImporterSourceFilenameRegexp(m_txtSourceFilename->text());
    Settings::setImageImporterFilenameIgnoreCase(m_chkIgnoreCase->isChecked());
    Settings::setImageImporterSourceIncludeSubfolders(m_chkSrcIncludeSubfolders->isChecked());
    Settings::setImageImporterSourceRemoveAfterTransfer(m_chkSrcRemoveFilesFromSrc->isChecked());

    Settings::setImageImporterArchiveImages(m_groupArchive->isChecked());
    Settings::setImageImporterArchiveBaseFolder(m_txtArchiveBaseFolder->text());
    Settings::setImageImporterArchiveSubfolders(m_txtArchiveSubfolders->text());
    Settings::setImageImporterArchiveFilename(m_txtArchiveFilename->text());
    Settings::setImageImporterArchiveLowercaseFilenames(m_chkArchiveLowercase->isChecked());

    Settings::setImageImporterDestBaseFolder(m_cmbDestBasefolder->currentText());
    Settings::setImageImporterDestSubfolders(m_txtDestSubfolders->text());
    Settings::setImageImporterDestFilenames(m_txtDestFilename->text());
    Settings::setImageImporterDestLowercaseFilenames(m_chkDestLowercase->isChecked());
}


QString ImageImporter::replaceStrings(const QString& src, const QFileInfo* fileinfo, QRegExp regexp )
{
    QString retval = src;

    //general replacement for keys of the form $qdt-XXX where XXX is one of the generic QDateTime
    // format strings
    QStringList qdt;
    qdt << "dddd" << "ddd" << "dd" << "d"
        << "MMMM" << "MMM" << "MM" << "M"
        << "yyyy" << "yy"
        << "hh" << "h" << "mm" << "m"
        << "ss" << "s" << "zzz" << "z"
        << "AP" << "ap";

    QStringList dateTimeVars;
    QStringList dateTimeRepl;

    dateTimeVars << "year" << "month" << "day" << "hour" << "min" << "sec";
    dateTimeRepl << "yyyy" << "MM"    << "dd"  << "hh"   << "mm"  << "ss" ;

    for (QStringList::iterator it = qdt.begin(); it != qdt.end(); ++it) {
        dateTimeVars << QString("$qdt-%1").arg(*it);
        dateTimeRepl << QString(*it);
    }

    if (fileinfo == 0L) {
        QDateTime dt = QDateTime::currentDateTime();
        if (dt.isValid()) {
            retval = retval.replace("$year" , dt.toString("yyyy"));
            retval = retval.replace("$month", dt.toString("MM"));
            retval = retval.replace("$day"  , dt.toString("dd"));
            retval = retval.replace("$hour" , dt.toString("hh"));
            retval = retval.replace("$min"  , dt.toString("mm"));
            retval = retval.replace("$sec"  , dt.toString("ss"));

            for (QStringList::iterator it = qdt.begin(); it != qdt.end(); ++it) {
                retval = retval.replace(QString("$qdt-%1").arg(*it) , dt.toString(*it));
            }
        }

        retval = retval.replace("$ext"  , "[ext]");
        retval = retval.replace("$model"  , "[model]");
        retval = retval.replace("$make"  , "[make]");

        retval = retval.replace("$0"  , "[orig-filename]");
        for (int i = 1; i < 10; ++i) {
            retval = retval.replace(QString("$%1").arg(i)    , QString("[capture-%1]").arg(i));
        }


    } else {

        //replace the captures ...
        if (regexp.exactMatch(fileinfo->fileName())) {
            for (int i = 0; i <= regexp.numCaptures(); ++i) {
                retval = retval.replace(QString("$%1").arg(i)    , regexp.cap(i));
            }
        }

        // we begin with model, as it is a nice way to find out, if exif data is available
        // model
        QString model = getExifData(fileinfo->absFilePath(), EXIF_TAG_MODEL);

        // I assume: No exifdata available if model is not set
        if (model.isEmpty()) {
            return "";
        }
        retval = retval.replace("$model"  , model);

            // make
        QString make = getExifData(fileinfo->absFilePath(), EXIF_TAG_MAKE);
        retval = retval.replace("$make"  , make);

            // dateTime original
        QDateTime dt = QDateTime::fromString(getExifData(fileinfo->absFilePath(), EXIF_TAG_DATE_TIME_ORIGINAL)
                                             .replace(":", "-"), Qt::ISODate);
        if (dt.isValid()) {
            retval = retval.replace("$year" , dt.toString("yyyy"));
            retval = retval.replace("$month", dt.toString("MM"));
            retval = retval.replace("$day"  , dt.toString("dd"));
            retval = retval.replace("$hour" , dt.toString("hh"));
            retval = retval.replace("$min"  , dt.toString("mm"));
            retval = retval.replace("$sec"  , dt.toString("ss"));

            for (QStringList::iterator it = qdt.begin(); it != qdt.end(); ++it) {
                retval = retval.replace(QString("$qdt-%1").arg(*it) , dt.toString(*it));
            }
        }

        // extension
        int pos = fileinfo->fileName().findRev('.') + 1;
        if (pos > 0) {
            retval = retval.replace("$ext"  , fileinfo->fileName().mid(pos));
        }
    }

    //remove all unreplaced $variables
    if (retval.contains("$") ) {
        ///@todo remove all $variables that could not be replaced
    }

    //finally remove multiple slashes
    while (retval.contains("//")) {
        retval = retval.replace("//", "/");
    }

    return retval;
}


void ImageImporter::slotUpdateArchiveExample() {
    QString tmp = QString("<b></b>%1/<b>%2</b>/%3")
                  .arg(m_txtArchiveBaseFolder->text())
                  .arg(m_txtArchiveSubfolders->text())
                  .arg(m_txtArchiveFilename->text());

    m_lblArchiveExampleString->setText(replaceStrings(tmp, 0L,
        QRegExp(m_txtSourceFilename->text(), !m_chkIgnoreCase->isChecked())));
}

void ImageImporter::slotUpdateDestExample() {
    QString tmp = QString("<b></b>%1/<b>%2</b>/%3")
                  .arg(m_cmbDestBasefolder->currentText())
                  .arg(m_txtDestSubfolders->text())
                  .arg(m_txtDestFilename->text());

    m_lblDestExampleString->setText(replaceStrings(tmp, 0L,
        QRegExp(m_txtSourceFilename->text(), !m_chkIgnoreCase->isChecked())));
}



QString ImageImporter::helpText(const QString& txt) {

    return QString(i18n("%1<br>"
                        "<br>"
                        "The following replacements will be made (if sufficient information is avaiable):<br>"
                        "<b>$year</b>  replaced by the <i>year</i> the image was taken (src: exif)<br>"
                        "<b>$month</b> replaced by the <i>month</i> the image was taken (src: exif)<br>"
                        "<b>$day</b>   replaced by the <i>day</i> the image was taken (src: exif)<br>"
                        "<b>$hour</b>  replaced by the <i>hour</i> the image was taken (src: exif)<br>"
                        "<b>$min</b>   replaced by the <i>minute</i> the image was taken (src: exif)<br>"
                        "<b>$sec</b>   replaced by the <i>second</i> the image was taken (src: exif)<br>"
                        "<b>$qdt-XXXX</b> where <i>XXXX</i> is one of the QDateTime format strings (src: exif)<br>"
                        "<b>$ext</b>   replaced by the <i>file extention</i> of the original image (src: string after last '.')<br>"
                        "<b>$0</b>     replaced by the <i>original image name</i> (src: regexp)<br>"
                        "<b>$1, $2, ...</b> replaced by the <i>captures</i> of the source regular expression (src: regexp)<br>"
                        "<b>$model</b> replaced by the camera model (src: exif)<br>"
                        "<b>$make</b>  replaced by the camera make (src: exif)<br>"
                        "")).arg(txt) ;
}


void ImageImporter::slotImport()
{
    //first save all
    slotSaveSettings();

    //then init the regular expression
    QRegExp re(m_txtSourceFilename->text(), !m_chkIgnoreCase->isChecked());

    //first find all possible files
    //listdir is used as a stack containing the directories to parse
    QStringList lstDirs = m_cmbSourceFolder->currentText();

    //the list of files found in the directories
    QFileInfoList lstFiles;
    lstFiles.setAutoDelete(true);

    DlgImageImporterStatus* dlgStatus = new DlgImageImporterStatus(this, "StatusDialog");
    dlgStatus->enableImageArchive(m_groupArchive->isChecked());
    dlgStatus->show();

    dlgStatus->appendStatusMessage(i18n("Starting..."));

    dlgStatus->setCurrentMode( DlgImageImporterStatus::ModeImport,
                               i18n("Scanning for available Images..."));

    //now go thru all folders and collect all files that match the file regexp...
    while (!lstDirs.isEmpty()) {
        QDir d( lstDirs.front() );
        lstDirs.pop_front();

        dlgStatus->addFolder();

        d.setMatchAllDirs(true);

        const QFileInfoList* list = d.entryInfoList();
        if (list) {

            QFileInfoListIterator it( *list );
            QFileInfo* fi;

            for ( ; ( fi = it.current() ) != 0; ++it )
            {
                if ( fi->fileName() == "." || fi->fileName() == ".."  ) {
                    continue;
                } else if ( fi->isDir() && m_chkSrcIncludeSubfolders->isChecked())    {
                    lstDirs.append(fi->absFilePath());
                } else if( fi->isFile() ) {
                    dlgStatus->addFile();
                    if (re.exactMatch(fi->fileName())) {
                        dlgStatus->addMatch();
                        //save a copy of all FileInfos
                        lstFiles.append(new QFileInfo(*fi));
                    }
                }
                // we return here and break all importing!
                if (dlgStatus->wasCanceled()) {
                    return;
                }
            }
        }
        // we return here and break all importing!
        if (dlgStatus->wasCanceled()) {
            return;
        }
    }

    //archive the images, if requested ...
    if (m_groupArchive->isChecked())
    {
        dlgStatus->setCurrentMode(DlgImageImporterStatus::ModeArchive,
                                  i18n("Archiving found images..."));

        importFiles(&lstFiles,
                    m_txtArchiveBaseFolder->text(),
                    m_txtArchiveSubfolders->text(),
                    m_txtArchiveFilename->text(),
                    m_chkArchiveLowercase->isChecked(),
                    false,
                    dlgStatus);

        if (dlgStatus->wasCanceled()) {
            //either canceled by user or error
            return;
        }
    } else {
        dlgStatus->appendStatusMessage(i18n("Archiving found images... skipped"));
    }

    QString msg = i18n("Moving found images...");
    if (!m_chkSrcRemoveFilesFromSrc->isChecked()) {
        msg = i18n("Copying found images...");
    }
    dlgStatus->setCurrentMode(DlgImageImporterStatus::ModeDestination, msg);

    // ... then copy/ move the images to their destinaion
    importFiles(&lstFiles, m_cmbDestBasefolder->currentText(), m_txtDestSubfolders->text(),
                m_txtDestFilename->text(), m_chkDestLowercase->isChecked(),
                m_chkSrcRemoveFilesFromSrc->isChecked(), dlgStatus);

    if (dlgStatus->wasCanceled()) {
        //either canceled by user or error
        return;
    }

    //yes, we are done :)
    dlgStatus->setCurrentMode( DlgImageImporterStatus::ModeFinished, i18n("Done."));

    //now update the images
    emit newImagesImported(m_cmbDestBasefolder->currentText());

    ///@todo what happens if the dest dir is not a current kphotobook folder? add the new folder?
    /// or a subfolder of a kphotobook folder?
}



void ImageImporter::importFiles(QFileInfoList* lstFiles, const QString& baseFolder,
                                const QString& subfolder, const QString& fileName,
                                bool lowercaseFileNames, bool moveFiles, DlgImageImporterStatus* dlgStatus)
{
    QRegExp re(m_txtSourceFilename->text(), !m_chkIgnoreCase->isChecked());
    if (!re.isValid()) {
        dlgStatus->appendErrorMessage(i18n("Invalid Regular Expression!"));
        dlgStatus->setCurrentMode(DlgImageImporterStatus::ModeAbort, i18n("Aborted!"));
        return;
    }


    //check if the basedir exists
    QDir bfd(baseFolder);
    if (!bfd.exists()) {
        if (!ImageImporter::mkpath(bfd.absPath())) {
            dlgStatus->appendErrorMessage(i18n("Could not create base path!"));
            dlgStatus->setCurrentMode(DlgImageImporterStatus::ModeAbort, i18n("Aborted!"));
            return;
        }
    }



    QFileInfo* fi;
    for ( fi = lstFiles->first(); fi; fi = lstFiles->next() )
    {
        dlgStatus->incProgress();

        QString sf = replaceStrings( subfolder, fi, re );
        if (sf.isEmpty()) {
            dlgStatus->appendErrorMessage(QString(i18n("Image \'%1\' does not contain exif data, skipping!"))
                                          .arg(fi->absFilePath()));
            continue;
        }
        QString fn = replaceStrings( fileName,  fi, re );

        if (lowercaseFileNames) {
            fn = fn.lower();
        }

        //now check if the directories exists ... or create them...
        QDir sfd(bfd.absPath() + "/" + sf);
        if (!sfd.exists()) {
            if (!ImageImporter::mkpath(sfd.absPath())) {
                dlgStatus->appendErrorMessage(QString(i18n("Could not create subfolder for image \'%1\'"))
                                              .arg(fi->absFilePath()));
                continue;
            }
        }

        QFileInfo toFile(sfd.absPath() + "/" + fn);
        if (toFile.exists()) {
            dlgStatus->appendErrorMessage(QString(i18n("Image \'%1\' already exists, not imported!"))
                                          .arg(fi->absFilePath()));
            continue;
        }

        if (moveFiles) {
            ImageImporter::move_file( fi->absFilePath(), toFile.absFilePath() );
        } else {
            ImageImporter::copy_file( fi->absFilePath(), toFile.absFilePath() );
        }

        if (dlgStatus->wasCanceled()) {
            return;
        }
    }
}







// taken (and slightly modified) from qt-4.0.1 "corelib/io/qfsfileengine_unix.cpp"
bool ImageImporter::mkpath(QString dirName)
{
    dirName = QDir::cleanDirPath(dirName);
    for(int oldslash = -1, slash = 0; slash != -1; oldslash = slash) {
        slash = dirName.find(QDir::separator(), oldslash+1);
        if(slash == -1) {
            if(oldslash == (int)dirName.length())
                break;
            slash = dirName.length();
        }
        if(slash) {
            QByteArray chunk = QFile::encodeName(dirName.left(slash));

            if (!QDir(chunk).exists()) {
                if (!QDir(chunk).mkdir( chunk, true ) ) {
                    return false;
                }
            }
        }
    }
    return true;
}


///@todo change this implementation to something nativ, not using system()
bool ImageImporter::move_file(const QString& src, const QString& dest)
{
    QString call = QString("mv \"%1\" \"%2\"").arg(src).arg(dest);
    system(call);

    QFileInfo srcFi(src);
    QFileInfo destFi(dest);
    return (!srcFi.exists() && destFi.exists());
}


///@todo change this implementation to something nativ, not using system()
bool ImageImporter::copy_file(const QString& src, const QString& dest)
{
    QString call = QString("cp \"%1\" \"%2\"").arg(src).arg(dest);
    system(call);

    QFileInfo srcFi(src);
    QFileInfo destFi(dest);
    return (srcFi.exists() && destFi.exists());
}











///@todo create a dedicated EXIF class
QString ImageImporter::getExifData(const QString& filename, ExifTag tag)
{
    ExifData* ed = exif_data_new_from_file(filename.ascii());
    ExifContent* c0, * c1, * cExif;

    c0    = ed->ifd[EXIF_IFD_0];
    c1    = ed->ifd[EXIF_IFD_1];
    cExif = ed->ifd[EXIF_IFD_EXIF];

    switch (tag) {
    case EXIF_TAG_MAKE:
    case EXIF_TAG_MODEL:
    case EXIF_TAG_SOFTWARE:
        return getEntry(c0, tag);
        break;

    case EXIF_TAG_EXPOSURE_TIME:
    case EXIF_TAG_FNUMBER:
    case EXIF_TAG_PIXEL_X_DIMENSION:
    case EXIF_TAG_PIXEL_Y_DIMENSION:
    case EXIF_TAG_FLASH:
    case EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM:
    case EXIF_TAG_ISO_SPEED_RATINGS:
    case EXIF_TAG_DATE_TIME_ORIGINAL:
        return getEntry(cExif, tag);
        break;

    default:
        return "";
        break;
    }

    exif_data_free(ed);
}


///@todo create a dedicated EXIF class
QString ImageImporter::getEntry(ExifContent* c, ExifTag tag)
{

    QString retval;
    ExifEntry* entry = exif_content_get_entry(c, tag);

    if (entry) {
        const int len = 256;
        char charData[len];
        exif_entry_get_value(entry, charData, len);
        retval = QString(charData);
    }

    return retval;
}
