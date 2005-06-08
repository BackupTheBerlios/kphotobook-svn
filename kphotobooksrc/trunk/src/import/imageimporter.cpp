/****************************************************************************
** Form implementation generated from reading ui file 'importer.ui'
**
** Created: Mon May 30 20:26:56 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "imageimporter.h"

#include "../settings/settings.h"
#include "../tracer/tracer.h"

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
#include <kdirselectdialog.h>

#include <stdio.h>
#include <mntent.h>

#include <kmountpoint.h>
#include <qregexp.h>
#include "kio/job.h"
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

    m_bCurrentDeviceMounted = false;

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
    connect(m_btnSourceFolder, SIGNAL(clicked()), this, SLOT(slotBtnSourceFolder()));
    m_grpSourceLayout->addWidget( m_btnSourceFolder, 0, 2 );

    m_radioDevice = new QRadioButton( m_grpSource, "radioDevice" );
    m_radioDevice->setText( i18n( "Device" ) );
    m_grpSourceLayout->addWidget( m_radioDevice, 1, 0 );
    m_cmbSourceDevice = new QComboBox( false, m_grpSource, "cmbSourceDevice" );
    connect(m_cmbSourceDevice, SIGNAL(highlighted(const QString& )), this, SLOT(slotSourceDeviceTextChanged(const QString&)));
//     m_cmbSourceDevice->setEditable( true );
    m_grpSourceLayout->addWidget( m_cmbSourceDevice, 1, 1 );
    m_btnMountDevice = new QPushButton( m_grpSource, "btnMountDevice" );
    m_btnMountDevice->setText( i18n( "Mount" ) );
    m_btnMountDevice->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)0, 0, 0, m_btnMountDevice->sizePolicy().hasHeightForWidth() ) );
//     m_btnMountDevice->setToggleButton( TRUE );
    connect(m_btnMountDevice, SIGNAL(clicked()), this, SLOT(slotMountDevice()));
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
    connect(m_txtArchiveBaseFolder, SIGNAL(textChanged(const QString& )), this, SLOT(slotUpdateArchiveExample()));
    m_groupArchiveLayout->addWidget( m_txtArchiveBaseFolder, 0, 1 );
    m_groupArchiveLayout->addWidget( new QLabel(m_txtArchiveBaseFolder, i18n( "Base Folder" ), m_groupArchive, "lblArchiveBaseFolder" ), 0, 0 );
    QPushButton* btn = new QPushButton(i18n( "..." ), m_groupArchive, "btnArhiveBaseFolder" );
    m_groupArchiveLayout->addWidget( btn, 0, 2 );
    connect(btn, SIGNAL(clicked()), this, SLOT(slotBtnArchiveBaseFolder()));

    m_txtArchiveSubfolders = new QLineEdit( m_groupArchive, "txtArchiveSubfolders" );
    connect(m_txtArchiveSubfolders, SIGNAL(textChanged(const QString& )), this, SLOT(slotUpdateArchiveExample()));
    m_groupArchiveLayout->addWidget( m_txtArchiveSubfolders, 1, 1 );
    m_groupArchiveLayout->addWidget( new QLabel(m_txtArchiveSubfolders, i18n( "Subfolders" ), m_groupArchive, "blbArchiveSubfolders" ), 1, 0 );

    m_txtArchiveFilename = new QLineEdit( m_groupArchive, "txtArchiveFilename" );
    connect(m_txtArchiveFilename, SIGNAL(textChanged(const QString& )), this, SLOT(slotUpdateArchiveExample()));
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

    m_txtDestBasefolder = new QLineEdit( m_groupDest, "txtDestBasefolder" );
    connect(m_txtDestBasefolder, SIGNAL(textChanged(const QString& )), this, SLOT(slotUpdateDestExample()));
    m_groupDestLayout->addWidget( m_txtDestBasefolder, 0, 1 );
    m_groupDestLayout->addWidget( new QLabel(m_txtDestBasefolder, i18n( "Base Folder" ), m_groupDest, "lblDestBaseFolder" ), 0, 0 );
    btn = new QPushButton(i18n( "..." ), m_groupDest, "btnDestBaseFolder" );
    m_groupDestLayout->addWidget( btn, 0, 2 );
    connect(btn, SIGNAL(clicked()), this, SLOT(slotBtnDestBaseFolder()));


    m_txtDestSubfolders = new QLineEdit( m_groupDest, "txtDestSubfolders" );
    connect(m_txtDestSubfolders, SIGNAL(textChanged(const QString& )), this, SLOT(slotUpdateDestExample()));
    m_groupDestLayout->addWidget( m_txtDestSubfolders, 1, 1 );
    m_groupDestLayout->addWidget( new QLabel(m_txtDestSubfolders, i18n( "Subfolders" ), m_groupDest, "lblDestSubfolders" ), 1, 0 );

    m_txtDestFilename = new QLineEdit( m_groupDest, "txtDestFilename" );
    connect(m_txtDestFilename, SIGNAL(textChanged(const QString& )), this, SLOT(slotUpdateDestExample()));
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

    m_lblDestExampleString = new KSqueezedTextLabel( m_groupDest, "lblDestExampleString" );
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

    resize( QSize(551, 519).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

}

void ImageImporter::initData()
{
    const KMountPoint::List lst = KMountPoint::possibleMountPoints(KMountPoint::NeedMountOptions);
    for ( KMountPoint::List::ConstIterator it = lst.begin() ; it != lst.end() ; ++it ) {
        //so, I only show devices that are mountable by a user and are not nfs
        ///@todo check if the user is superuser (id=0)
        if ((*it)->mountOptions().grep("user").size() > 0
            && (*it)->mountType() != "nfs") {

            QString x = QString("%1 (%2)").arg((*it)->mountedFrom()).arg((*it)->mountPoint());
            m_cmbSourceDevice->insertItem(x);
         }

    }


    ///@todo make this a setting, too
    m_radioDevice->setChecked(true);

    m_txtSourceFolder->setText(Settings::imageImporterSourceFolder());
    m_cmbSourceDevice->setCurrentText(Settings::imageImporterSourceDevice());
    m_txtSourceFilename->setText(Settings::imageImporterSourceFilenameRegexp());
    m_chkSrcIncludeSubfolders->setChecked(Settings::imageImporterSourceIncludeSubfolders());

    m_groupArchive->setChecked(Settings::imageImporterArchiveImages());
    m_txtArchiveBaseFolder->setText(Settings::imageImporterArchiveBaseFolder());
    m_txtArchiveSubfolders->setText(Settings::imageImporterArchiveSubfolders());
    m_txtArchiveFilename->setText(Settings::imageImporterArchiveFilename());
    m_chkArchiveLowercase->setChecked(Settings::imageImporterArchiveLowercaseFilenames());

    m_txtDestBasefolder->setText(Settings::imageImporterDestBaseFolder());
    m_txtDestSubfolders->setText(Settings::imageImporterDestSubfolders());
    m_txtDestFilename->setText(Settings::imageImporterDestFilenames());
    m_chkDestLowercase->setChecked(Settings::imageImporterDestLowercaseFilenames());

}


void ImageImporter::slotSourceDeviceTextChanged(const QString& str)
{
    QRegExp re("(.*) \\(.*\\)");
    re.match(str);

    m_bCurrentDeviceMounted = false;
    //find out if the current selection is mounted
    const KMountPoint::List lst = KMountPoint::currentMountPoints();
    for ( KMountPoint::List::ConstIterator it = lst.begin() ; it != lst.end() ; ++it ) {
        if ((*it)->mountedFrom() == re.cap(1)) {
            m_bCurrentDeviceMounted = true;
            break;
        }
    }
    m_btnMountDevice->setText(m_bCurrentDeviceMounted ? i18n("Unmount") : i18n("Mount"));
}


void ImageImporter::slotMountDevice()
{
    ///@todo after mounting the button text should be changed accordingly
    QRegExp re("(.*) \\(.*\\)");
    re.match(m_cmbSourceDevice->currentText());
    if (!m_bCurrentDeviceMounted) {
        KIO::mount(false, 0L, re.cap(1), 0L, false);
    } else {
        KIO::unmount(re.cap(1), true);
    }
}



void ImageImporter::slotBtnSourceFolder()
{
    KURL d = KDirSelectDialog::selectDirectory(Settings::imageImporterSourceFolder(), false, this);
    if (d.isEmpty()) {
        return;
    }
    m_txtSourceFolder->setText(d.path());
    Settings::setImageImporterSourceFolder(d.path());
}


void ImageImporter::slotBtnArchiveBaseFolder()
{
    KURL d = KDirSelectDialog::selectDirectory(Settings::imageImporterArchiveBaseFolder(), false, this);
    if (d.isEmpty()) {
        return;
    }
    m_txtArchiveBaseFolder->setText(d.path());
    Settings::setImageImporterArchiveBaseFolder(d.path());
}

void ImageImporter::slotBtnDestBaseFolder()
{
    KURL d = KDirSelectDialog::selectDirectory(Settings::imageImporterDestBaseFolder(), false, this);
    if (d.isEmpty()) {
        return;
    }
    m_txtDestBasefolder->setText(d.path());
    Settings::setImageImporterDestBaseFolder(d.path());
}

void ImageImporter::slotImport()
{
    slotSaveSettings();
    ///todo do import here :-)
}

void ImageImporter::slotQuit()
{
    slotSaveSettings();
    accept();
}



void ImageImporter::slotSaveSettings()
{
    Settings::setImageImporterSourceFolder(m_txtSourceFolder->text());
    Settings::setImageImporterSourceDevice(m_cmbSourceDevice->currentText());
    Settings::setImageImporterSourceFilenameRegexp(m_txtSourceFilename->text());
    Settings::setImageImporterSourceIncludeSubfolders(m_chkSrcIncludeSubfolders->isChecked());

    Settings::setImageImporterArchiveImages(m_groupArchive->isChecked());
    Settings::setImageImporterArchiveBaseFolder(m_txtArchiveBaseFolder->text());
    Settings::setImageImporterArchiveSubfolders(m_txtArchiveSubfolders->text());
    Settings::setImageImporterArchiveFilename(m_txtArchiveFilename->text());
    Settings::setImageImporterArchiveLowercaseFilenames(m_chkArchiveLowercase->isChecked());

    Settings::setImageImporterDestBaseFolder(m_txtDestBasefolder->text());
    Settings::setImageImporterDestSubfolders(m_txtDestSubfolders->text());
    Settings::setImageImporterDestFilenames(m_txtDestFilename->text());
    Settings::setImageImporterDestLowercaseFilenames(m_chkDestLowercase->isChecked());
}


QString replaceStrings(QString src)
{
    QString retval = src;

    QStringList validExpressions;
    validExpressions << "$year" << "$month" << "$day" << "$hour" << "$min" << "$sec" << "$ext";

    retval = retval.replace("$year" , QDateTime::currentDateTime().toString("yyyy"));
    retval = retval.replace("$month", QDateTime::currentDateTime().toString("MM"));
    retval = retval.replace("$day"  , QDateTime::currentDateTime().toString("d"));
    retval = retval.replace("$hour" , QDateTime::currentDateTime().toString("hh"));
    retval = retval.replace("$min"  , QDateTime::currentDateTime().toString("mm"));
    retval = retval.replace("$sec"  , QDateTime::currentDateTime().toString("ss"));
    retval = retval.replace("$ext"  , "jpg");

    while (retval.contains("//")) {
        retval = retval.replace("//", "/");
    }

    return retval;
}


void ImageImporter::slotUpdateArchiveExample()
{
    QString tmp = QString("<b></b>%1/<b>%2</b>/%3")
        .arg(m_txtArchiveBaseFolder->text())
        .arg(m_txtArchiveSubfolders->text())
        .arg(m_txtArchiveFilename->text());

    m_lblArchiveExampleString->setText(replaceStrings(tmp));
}

void ImageImporter::slotUpdateDestExample()
{
    QString tmp = QString("<b></b>%1/<b>%2</b>/%3")
        .arg(m_txtDestBasefolder->text())
        .arg(m_txtDestSubfolders->text())
        .arg(m_txtDestFilename->text());

    m_lblDestExampleString->setText(replaceStrings(tmp));
}


QString helpText() {

QString help = "Allowed are:\n"
         "$year  "
         "$month "
         "$day   "
         "$hour  "
         "$min   "
         "$sec   "
         "$ext   ";

    return help;
}


