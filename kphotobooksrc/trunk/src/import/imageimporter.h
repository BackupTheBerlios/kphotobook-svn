/****************************************************************************
** Form interface generated from reading ui file 'importer.ui'
**
** Created: Mon May 30 20:26:38 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef IMAGEIMPORTER_H
#define IMAGEIMPORTER_H

#include <qvariant.h>
#include <kdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QButtonGroup;
class QPushButton;
class QRadioButton;
class QLineEdit;
class QComboBox;
class QLabel;
class QFrame;
class QGroupBox;
class QCheckBox;
class KSqueezedTextLabel;

class ImageImporter : public KDialog
{
    Q_OBJECT

public:
    ImageImporter( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ImageImporter();


private slots:
    void slotBtnArchiveBaseFolder();
    void slotBtnDestBaseFolder();
    void slotImport();
    void slotQuit();

private:
    void initGUI();
    void initData();

    /////////////////////////////


private:
    QButtonGroup*  m_grpSource;
    QPushButton*   m_btnSourceFolder;
    QRadioButton*  m_radioFolder;
    QRadioButton*  m_radioDevice;
    QLineEdit*     m_txtSourceFolder;
    QComboBox*     m_cmbSourceDevice;
    QPushButton*   m_btnMountDevice;
    QLineEdit*     m_txtSourceFilename;
    QCheckBox*     m_chkSrcIncludeSubfolders;
    QGroupBox*     m_groupArchive;
    QLineEdit*     m_txtArchiveBaseFolder;
    QLineEdit*     m_txtArchiveSubfolders;
    QLineEdit*     m_txtArchiveFilename;
    QCheckBox*     m_chkArchiveLowercase;
    KSqueezedTextLabel* m_kSqueezedTextLabel1;
    QGroupBox*     m_groupDest;
    QLineEdit*     m_txtDestBasefolder;
    QLineEdit*     m_txtDestSubfolders;
    QLineEdit*     m_txtDestFilename;
    QCheckBox*     m_chkDestLowercase;
    QLabel*        m_lblDestExampleString;

protected:
    QVBoxLayout*   m_ImageImporterLayout;
    QGridLayout*   m_grpSourceLayout;
    QGridLayout*   m_groupArchiveLayout;
    QGridLayout*   m_groupDestLayout;

protected slots:
    virtual void languageChange();

};

#endif // IMAGEIMPORTER_H
