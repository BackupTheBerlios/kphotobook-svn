/****************************************************************************
** Form implementation generated from reading ui file 'importer.ui'
**
** Created: Mon May 30 20:26:56 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "imageimporter.h"

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

#include <klocale.h>

/*
 *  Constructs a ImageImporter as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ImageImporter::ImageImporter( QWidget* parent, const char* name, bool modal, WFlags fl )
        : KDialog( parent, name, modal, fl )
{
    setCaption(i18n("Image Importer"));
    initGUI();
    initData();
}

ImageImporter::~ImageImporter() {
    // no need to delete child widgets, Qt does it all for us
}



void ImageImporter::initGUI()
{
    m_ImageImporterLayout = new QVBoxLayout( this, 11, 6, "ImageImporterLayout");

    m_grpSource = new QButtonGroup( this, "grpSource" );
    m_grpSource->setTitle( i18n( "Source" ) );
    m_grpSource->setColumnLayout(0, Qt::Vertical );
    m_grpSource->layout()->setSpacing( 6 );
    m_grpSource->layout()->setMargin( 11 );
    m_grpSourceLayout = new QGridLayout( m_grpSource->layout() );
    m_grpSourceLayout->setAlignment( Qt::AlignTop );

    m_radioFolder = new QRadioButton( m_grpSource, "radioFolder" );
    m_radioFolder->setText( i18n( "Folder" ) );
    m_radioFolder->setChecked( true );
    m_grpSourceLayout->addWidget( m_radioFolder, 0, 0 );
    m_txtSourceFolder = new QLineEdit( m_grpSource, "txtSourceFolder" );
    m_grpSourceLayout->addWidget( m_txtSourceFolder, 0, 1 );
    m_btnSourceFolder = new QPushButton( m_grpSource, "btnSourceFolder" );
    m_btnSourceFolder->setText( i18n( "..." ) );
    m_grpSourceLayout->addWidget( m_btnSourceFolder, 0, 2 );

    m_radioDevice = new QRadioButton( m_grpSource, "radioDevice" );
    m_radioDevice->setText( i18n( "Device" ) );
    m_grpSourceLayout->addWidget( m_radioDevice, 1, 0 );
    m_cmbSourceDevice = new QComboBox( false, m_grpSource, "cmbSourceDevice" );
    m_cmbSourceDevice->setEditable( true );
    m_grpSourceLayout->addWidget( m_cmbSourceDevice, 1, 1 );
    m_btnMountDevice = new QPushButton( m_grpSource, "btnMountDevice" );
    m_btnMountDevice->setText( i18n( "Mount" ) );
    m_btnMountDevice->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)0, 0, 0, m_btnMountDevice->sizePolicy().hasHeightForWidth() ) );
    m_btnMountDevice->setToggleButton( TRUE );
    m_grpSourceLayout->addWidget( m_btnMountDevice, 1, 2 );

    QFrame* line = new QFrame( m_grpSource, "lineSource" );
    line->setFrameShape( QFrame::HLine );
    line->setFrameShadow( QFrame::Sunken );
    line->setFrameShape( QFrame::HLine );
    m_grpSourceLayout->addMultiCellWidget( line, 2, 2, 0, 2 );

    m_txtSourceFilename = new QLineEdit( m_grpSource, "txtSourceFilename" );
    m_grpSourceLayout->addMultiCellWidget( m_txtSourceFilename, 3, 3, 1, 2 );

    m_grpSourceLayout->addWidget( new QLabel(m_txtSourceFilename, i18n("Filename"), m_grpSource, "lblSourceFilename" ), 3, 0 );


    m_chkSrcIncludeSubfolders = new QCheckBox( m_grpSource, "chkSrcIncludeSubfolders" );
    m_chkSrcIncludeSubfolders->setText( i18n( "include subfolders" ) );
    m_chkSrcIncludeSubfolders->setChecked( true );
    m_grpSourceLayout->addMultiCellWidget( m_chkSrcIncludeSubfolders, 4, 4, 0, 2 );

    m_ImageImporterLayout->addWidget( m_grpSource );


    m_ImageImporterLayout->addItem(new QSpacerItem( 40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding ));


    m_groupArchive = new QGroupBox( this, "groupArchive" );
    m_groupArchive->setTitle( i18n( "Archive" ) );
    m_groupArchive->setCheckable( true );
    m_groupArchive->setColumnLayout(0, Qt::Vertical );
    m_groupArchive->layout()->setSpacing( 6 );
    m_groupArchive->layout()->setMargin( 11 );
    m_groupArchiveLayout = new QGridLayout( m_groupArchive->layout() );
    m_groupArchiveLayout->setAlignment( Qt::AlignTop );

    m_txtArchiveBaseFolder = new QLineEdit( m_groupArchive, "txtArchiveBaseFolder" );
    m_groupArchiveLayout->addWidget( m_txtArchiveBaseFolder, 0, 1 );
    m_groupArchiveLayout->addWidget( new QLabel(m_txtArchiveBaseFolder, i18n( "Base Folder" ), m_groupArchive, "lblArchiveBaseFolder" ), 0, 0 );
    QPushButton* btn = new QPushButton(i18n( "..." ), m_groupArchive, "btnArhiveBaseFolder" );
    m_groupArchiveLayout->addWidget( btn, 0, 2 );
    connect(btn, SIGNAL(clicked()), this, SLOT(slotBtnArchiveBaseFolder));

    m_txtArchiveSubfolders = new QLineEdit( m_groupArchive, "txtArchiveSubfolders" );
    m_groupArchiveLayout->addWidget( m_txtArchiveSubfolders, 1, 1 );
    m_groupArchiveLayout->addWidget( new QLabel(m_txtArchiveSubfolders, i18n( "Subfolders" ), m_groupArchive, "blbArchiveSubfolders" ), 1, 0 );

    m_txtArchiveFilename = new QLineEdit( m_groupArchive, "txtArchiveFilename" );
    m_groupArchiveLayout->addWidget( m_txtArchiveFilename, 2, 1 );
    m_groupArchiveLayout->addWidget( new QLabel(m_txtArchiveFilename, i18n( "Filename" ), m_groupArchive, "lblArchiveFilename" ), 2, 0 );

    m_chkArchiveLowercase = new QCheckBox( m_groupArchive, "chkArchiveLowercase" );
    m_chkArchiveLowercase->setText( i18n( "lowercase filenames" ) );
    m_chkArchiveLowercase->setChecked( true );
    m_groupArchiveLayout->addMultiCellWidget( m_chkArchiveLowercase, 3, 3, 0, 2 );

    line = new QFrame( m_groupArchive, "lineArchive" );
    line->setFrameShape( QFrame::HLine );
    line->setFrameShadow( QFrame::Sunken );
    line->setFrameShape( QFrame::HLine );
    m_groupArchiveLayout->addMultiCellWidget( line, 4, 4, 0, 2 );

    m_groupArchiveLayout->addWidget( new QLabel(i18n( "Example" ), m_groupArchive, "lblArchiveExample" ), 5, 0 );

    m_kSqueezedTextLabel1 = new KSqueezedTextLabel( m_groupArchive, "kSqueezedTextLabel1" );
    m_groupArchiveLayout->addMultiCellWidget( m_kSqueezedTextLabel1, 5, 5, 1, 2 );
    m_kSqueezedTextLabel1->setText("ich bin <b>ein Berliner</b>!");

    m_ImageImporterLayout->addWidget( m_groupArchive );

    m_groupDest = new QGroupBox( this, "groupDest" );
    m_groupDest->setTitle( i18n( "Destination" ) );
    m_groupDest->setCheckable( FALSE );
    m_groupDest->setColumnLayout(0, Qt::Vertical );
    m_groupDest->layout()->setSpacing( 6 );
    m_groupDest->layout()->setMargin( 11 );
    m_groupDestLayout = new QGridLayout( m_groupDest->layout() );
    m_groupDestLayout->setAlignment( Qt::AlignTop );

    m_txtDestBasefolder = new QLineEdit( m_groupDest, "txtDestBasefolder" );
    m_groupDestLayout->addWidget( m_txtDestBasefolder, 0, 1 );
    m_groupDestLayout->addWidget( new QLabel(m_txtDestBasefolder, i18n( "Base Folder" ), m_groupDest, "lblDestBaseFolder" ), 0, 0 );
    btn = new QPushButton(i18n( "..." ), m_groupDest, "btnDestBaseFolder" );
    m_groupDestLayout->addWidget( btn, 0, 2 );
    connect(btn, SIGNAL(clicked()), this, SLOT(slotBtnDestBaseFolder));


    m_txtDestSubfolders = new QLineEdit( m_groupDest, "txtDestSubfolders" );
    m_groupDestLayout->addWidget( m_txtDestSubfolders, 1, 1 );
    m_groupDestLayout->addWidget( new QLabel(m_txtDestSubfolders, i18n( "Subfolders" ), m_groupDest, "lblDestSubfolders" ), 1, 0 );

    m_txtDestFilename = new QLineEdit( m_groupDest, "txtDestFilename" );
    m_groupDestLayout->addWidget( m_txtDestFilename, 2, 1 );
    m_groupDestLayout->addWidget( new QLabel(m_txtDestFilename, i18n( "Filename" ), m_groupDest, "lblDestFilename" ), 2, 0 );

    m_chkDestLowercase = new QCheckBox( m_groupDest, "chkDestLowercase" );
    m_chkDestLowercase->setText( i18n( "lowercase filenames" ) );
    m_chkDestLowercase->setChecked( true );
    m_groupDestLayout->addMultiCellWidget( m_chkDestLowercase, 3, 3, 0, 2 );

    line = new QFrame( m_groupDest, "lineDest" );
    line->setFrameShape( QFrame::HLine );
    line->setFrameShadow( QFrame::Sunken );
    line->setFrameShape( QFrame::HLine );
    m_groupDestLayout->addMultiCellWidget( line, 4, 4, 0, 2 );

    m_groupDestLayout->addWidget( new QLabel(i18n( "Example" ), m_groupDest, "lblDestExample" ), 5, 0 );

    m_lblDestExampleString = new QLabel( m_groupDest, "lblDestExampleString" );
    m_groupDestLayout->addMultiCellWidget( m_lblDestExampleString, 5, 5, 1, 2 );

    m_ImageImporterLayout->addWidget( m_groupDest );



    QHBoxLayout* btnBottom = new QHBoxLayout( 0, 0, 6, "btnBottom");
    btnBottom->addItem(new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));

    btn = new QPushButton(i18n( "Import" ), this, "import" );
    btnBottom->addWidget( btn );
    connect(btn, SIGNAL(clicked()), this, SLOT(slotImport()));

    btn = new QPushButton(i18n( "Quit" ), this, "quit" );
    btnBottom->addWidget( btn );
    connect(btn, SIGNAL(clicked()), this, SLOT(slotQuit()));

    btn = new QPushButton(i18n( "Cancel" ), this, "cancel" );
    btnBottom->addWidget( btn );
    connect(btn, SIGNAL(clicked()), this, SLOT(reject()));


    m_ImageImporterLayout->addLayout( btnBottom );


    // signals and slots connections
    connect( m_radioFolder, SIGNAL( toggled(bool) ), m_txtSourceFolder, SLOT( setEnabled(bool) ) );
    connect( m_radioFolder, SIGNAL( toggled(bool) ), m_btnSourceFolder, SLOT( setEnabled(bool) ) );
    connect( m_radioDevice, SIGNAL( toggled(bool) ), m_cmbSourceDevice, SLOT( setEnabled(bool) ) );
    connect( m_radioDevice, SIGNAL( toggled(bool) ), m_btnMountDevice, SLOT( setEnabled(bool) ) );

    languageChange();
    resize( QSize(551, 519).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

}

void ImageImporter::initData()
{
    m_radioDevice->setChecked(true);
}


void ImageImporter::slotBtnArchiveBaseFolder()
{


}

void ImageImporter::slotBtnDestBaseFolder()
{

}

void ImageImporter::slotImport()
{

}

void ImageImporter::slotQuit()
{
    //TODO save settings and then exit;
    accept();
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ImageImporter::languageChange() {

//     txtSourceFolder->setText( i18n( "/home/choenig/Photos/incoming" ) );
//     cmbSourceDevice->clear();
//     cmbSourceDevice->insertItem( i18n( "/mnt/dimage7i" ) );
//     txtSourceFilename->setText( i18n( "pict([0-9]{4,4}).jpg" ) );
//     txtArchiveBaseFolder->setText( i18n( "/home/choenig/Photos/archiveFolder" ) );
//     txtArchiveSubfolders->setText( i18n( "$year/$month" ) );
//     txtArchiveFilename->setText( i18n( "$year.$month.$day-$hour:$min-$1.$ext" ) );
//     kSqueezedTextLabel1->setText( i18n( "/home/choenig/Photos/archiveDir/2004/12/2004.12.24-23:01-2457.jpg" ) );
//     txtDestBasefolder->setText( i18n( "/home/choenig/Photos/destinationFolder" ) );
//     txtDestSubfolders->setText( i18n( "$year/$month/$day" ) );
//     txtDestFilename->setText( i18n( "$year.$month.$day-$hour:$min-$1.$ext" ) );
//     lblDestExampleString->setText( i18n( "/home/choenig/Photos/archiveDir/<b>2004/12/24/</b>2004.12.24-23:01-2457.jpg" ) );

}

