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
#ifndef IMAGEIMPORTER_H_
#define IMAGEIMPORTER_H_

#include "../tracer/tracer.h"


class KPhotoBook;
class DlgImageImporterStatus;

#include <qvariant.h>
#include <kdialog.h>
#include <qdir.h>

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


#include <libexif/exif-content.h>
#include <libexif/exif-tag.h>





class ImageImporter : public KDialog
{
    Q_OBJECT

private:
    static Tracer* tracer;

public:
    ImageImporter( KPhotoBook* parent, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ImageImporter();

signals:
    void newImagesImported(const QString& dir);

private slots:
    void slotUpdateMountStatus();
    void slotMountDevice();

    void slotBtnSourceFolder();
    void slotBtnArchiveBaseFolder();
    void slotBtnDestBaseFolder();
    void slotImport();
    void slotQuit();
    void slotSaveSettings();

    void slotUpdateArchiveExample();
    void slotUpdateDestExample();

private:
    void initGUI();
    void initData();

    QString replaceStrings(const QString& src, const QFileInfo* fileinfo, QRegExp regexp );
    void importFiles(QFileInfoList* fileList, const QString& baseFolder, const QString& subfolders, const QString& fileName,
                     bool lowercaseFileNames, bool moveFiles, DlgImageImporterStatus* dlgStatus);

    QString helpText(const QString& txt);

    QString getCurrentMountDevice();
    QString getCurrentMountPoint();

    /**
     * inserts the item str into the combobox cmb, if its not only contained
     */
    void insertItemTo(QComboBox* cmb, const QString& str);

    QString getExifData(const QString& filename, ExifTag tag);
    QString getEntry(ExifContent* c, ExifTag tag);

    QDateTime toQDateTime(const QString& data);

    static bool mkpath(QString dirName);
    static bool move_file(const QString& src, const QString& dest);
    static bool copy_file(const QString& src, const QString& dest);

private:

    KPhotoBook*         m_photobook;

    QGroupBox*          m_grpSource;
    QPushButton*        m_btnSourceFolder;
    QComboBox*          m_cmbSourceFolder;
    QComboBox*          m_cmbSourceDevice;
    QPushButton*        m_btnMountDevice;
    QLineEdit*          m_txtSourceFilename;
    QCheckBox*          m_chkIgnoreCase;
    QCheckBox*          m_chkSrcIncludeSubfolders;
    QCheckBox*          m_chkSrcRemoveFilesFromSrc;
    QGroupBox*          m_groupArchive;
    QLineEdit*          m_txtArchiveBaseFolder;
    QLineEdit*          m_txtArchiveSubfolders;
    QLineEdit*          m_txtArchiveFilename;
    QCheckBox*          m_chkArchiveLowercase;
    KSqueezedTextLabel* m_lblArchiveExampleString;
    QGroupBox*          m_groupDest;
    QComboBox*          m_cmbDestBasefolder;
    QLineEdit*          m_txtDestSubfolders;
    QLineEdit*          m_txtDestFilename;
    QCheckBox*          m_chkDestLowercase;
    KSqueezedTextLabel* m_lblDestExampleString;

    bool m_bCurrentDeviceMounted;

protected:
    QVBoxLayout*   m_ImageImporterLayout;
    QGridLayout*   m_grpSourceLayout;
    QGridLayout*   m_groupArchiveLayout;
    QGridLayout*   m_groupDestLayout;
};

#endif
