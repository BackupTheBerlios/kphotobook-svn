/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#include "kphotobookview.h"

#include "constants.h"
#include "settings.h"

#include "kphotobook.h"
#include "file.h"

#include "tagtree.h"
#include "tagtreenode.h"

#include "sourcedir.h"
#include "sourcedirtree.h"
#include "sourcedirtreenode.h"

#include "kfile.h"
#include <kmessagebox.h>
#include <krun.h>
#include <klocale.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kfileiconview.h>

#include <qobject.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qstring.h>
#include <qpopupmenu.h>


KPhotoBookView::KPhotoBookView(QWidget *parent)
    : KMdiChildView(parent)
    , m_currentImagePreviewSize(-1) {

    // store casted pointer to the photobook
    m_photobook = dynamic_cast<KPhotoBook*>(parent);

    // main panel
    QVBoxLayout* mainLayout = new QVBoxLayout(this, 0, 0, "mainLayout");
    mainLayout->setAutoAdd(true);

    // file preview
    m_fileView = new KFileIconView(this, "fileIconView");
    if (Settings::imagePreviewSelectionMode() == Settings::EnumImagePreviewSelectionMode::Extended) {
        m_fileView->setSelectionMode(KFile::Extended);
    } else {
        m_fileView->setSelectionMode(KFile::Multi);
    }
    m_fileView->setResizeMode(KFileIconView::Adjust);
    m_fileView->setFont(Settings::imagePreviewFont());
    m_fileView->showPreviews();
    m_fileView->setWordWrapIconText(false);
    updateCurrentImageSize();

    QObject::connect(m_fileView, SIGNAL(doubleClicked(QIconViewItem*, const QPoint&)), this, SLOT(slotShowCurrentImage()));
    QObject::connect(m_fileView, SIGNAL(selectionChanged ()), m_photobook, SLOT(slotFileSelectionChanged()));
}


KPhotoBookView::~KPhotoBookView() {

    kdDebug() << "KPhotoBookView::~KPhotoBookView() invoked..." << endl;

    // remove the current previewed files
    //m_fileView->clearView();

    // all components are deleted automagically by the destructor of QWidget
}


void KPhotoBookView::removeTagNode(TagTreeNode* node) {

    // the node's destructor removes itself from the tree
    delete node;
}


void KPhotoBookView::updateFiles(QPtrList<KFileItem> *selectedFiles) {

    kdDebug() << "[KPhotoBookView::updateFiles] updating the displayed images." << endl;

    // remember all selected files if no selected files are specified
    QPtrList<KFileItem> temp;
    if (!selectedFiles) {
        temp = QPtrList<KFileItem>(*m_fileView->selectedItems());
        selectedFiles = &temp;
    }

    // remove all displayed images
    removeAllFiles();

    // add the images
    QPtrList<File>* fileList = m_photobook->files(QString::null);

    File* file;
    for ( file = fileList->first(); file; file = fileList->next() ) {
        m_fileView->insertItem(file);
    }

    // reselect all previously selected files
    QPtrListIterator<KFileItem> it(*selectedFiles);
    for (; it.current(); ++it) {
        m_fileView->setSelected(it.current(), true);
    }
}


void KPhotoBookView::storeConfiguration() {

}


void KPhotoBookView::updateCurrentImageSize() {

    if (m_currentImagePreviewSize == Settings::imagePreviewSize()) {
        // preview size has not changed --> do not update the view
        return;
    }

    m_currentImagePreviewSize = Settings::imagePreviewSize();

    // remember all selected files
    QPtrList<KFileItem> selectedFiles(*m_fileView->selectedItems());

    // remove all displayed images
    removeAllFiles();

    m_fileView->setPreviewSize(m_currentImagePreviewSize);
    m_fileView->showPreviews();
    m_fileView->setGridX(Settings::imagePreviewSize()+10);
    m_fileView->setGridY(Settings::imagePreviewSize()+10);

    // force redrawing all files by removing them and adding them again
    updateFiles(&selectedFiles);
}


//
// public slots
//
void KPhotoBookView::slotLoadSettings() {

    updateCurrentImageSize();

    if (Settings::imagePreviewSelectionMode() == Settings::EnumImagePreviewSelectionMode::Extended) {
        m_fileView->setSelectionMode(KFile::Extended);
    } else {
        m_fileView->setSelectionMode(KFile::Multi);
    }

    m_fileView->setFont(Settings::imagePreviewFont());
}


//
// private slots
//
void KPhotoBookView::slotShowCurrentImage() {

    KFileItem* item = m_fileView->currentFileItem();
    KURL file = item->url();

    QString tool = Settings::toolsDefaultExternalTool();

    kdDebug() << "Showing file in " << tool << ". url=<" << file.url() << ">, path=<" << file.path() << ">" << endl;

    KProcess *proc = new KProcess();

    *proc << tool;
    *proc << file.path();
    proc->start(KProcess::DontCare);
}


//
// private methods
//
void KPhotoBookView::removeAllFiles() {

    // we must deselect all files before removing it to improve the speed
    m_fileView->clearSelection();

    QPtrList<QIconViewItem>* itemList = new QPtrList<QIconViewItem>();

    for (QIconViewItem* item = m_fileView->firstItem(); item; item = item->nextItem()) {
        itemList->append(item);
    }

    for (QIconViewItem* item = itemList->first(); item; item = itemList->next()) {
        m_fileView->takeItem(item);
    }

    delete itemList;

    m_fileView->clearView();
}


#include "kphotobookview.moc"
