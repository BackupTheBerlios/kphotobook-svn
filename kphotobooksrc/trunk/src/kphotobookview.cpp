/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#include "kphotobookview.h"

#include "constants.h"
#include "kphotobook.h"
#include "dialogs/dialogimageviewer.h"
#include "engine/engine.h"
#include "engine/file.h"
#include "settings/settings.h"
#include "tracer/tracer.h"
#include "uitrees/sourcedirtree.h"
#include "uitrees/sourcedirtreenode.h"
#include "uitrees/tagtree.h"
#include "uitrees/tagtreenode.h"

#include <kfile.h>
#include <kfileiconview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <krun.h>

#include <qlayout.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qstring.h>


Tracer* KPhotoBookView::tracer = Tracer::getInstance("kde.kphotobook", "KPhotoBookView");


KPhotoBookView::KPhotoBookView(QWidget *parent) :
    KMdiChildView(parent),
    m_photobook(0),
    m_fileView(0),
    m_dlgImageViewer(0),
    m_currentImagePreviewSize(-1)
{
    // store casted pointer to the photobook
    m_photobook = dynamic_cast<KPhotoBook*>(parent);

    // main panel
    QVBoxLayout* mainLayout = new QVBoxLayout(this, 0, 0, "mainLayout");

    // file preview
    m_fileView = new KFileIconView(this, "fileIconView");
    mainLayout->add(m_fileView);
    if (Settings::imagePreviewSelectionMode() == Settings::EnumImagePreviewSelectionMode::Extended) {
        m_fileView->setSelectionMode(KFile::Extended);
    } else {
        m_fileView->setSelectionMode(KFile::Multi);
    }

    // image viewer
    m_dlgImageViewer = new DialogImageViewer(this, m_fileView);
    mainLayout->add(m_dlgImageViewer);

    // rest of fileview
    m_fileView->setResizeMode(KFileIconView::Adjust);
    m_fileView->setFont(Settings::imagePreviewFont());
    m_fileView->showPreviews();
    m_fileView->setWordWrapIconText(false);
    updateCurrentImageSize();

    QObject::connect(m_fileView, SIGNAL(doubleClicked(QIconViewItem*, const QPoint&)), this, SLOT(slotShowCurrentImage()));
    QObject::connect(m_fileView, SIGNAL(selectionChanged ()), m_photobook, SLOT(slotFileSelectionChanged()));
}


KPhotoBookView::~KPhotoBookView()
{
    tracer->invoked(__func__);

    ///@todo remove the current previewed files
    //m_fileView->clearView();

    // all components are deleted automagically by the destructor of QWidget
    delete m_dlgImageViewer;
}


void KPhotoBookView::removeTagNode(TagTreeNode* node)
{
    // the node's destructor removes itself from the tree
    delete node;
}


void KPhotoBookView::updateFiles(QPtrList<KFileItem> *selectedFiles)
{
    tracer->invoked(__func__, "updating the displayed images...");

    // remember all selected files if no selected files are specified
    QPtrList<KFileItem> temp;
    if (!selectedFiles) {
        temp = QPtrList<KFileItem>(*m_fileView->selectedItems());
        selectedFiles = &temp;
    }

    // remove all displayed images
    removeAllFiles();

    // add the images
    QPtrList<File>* fileList = m_photobook->files(0);

    File* file;
    for ( file = fileList->first(); file; file = fileList->next() ) {
        m_fileView->insertItem(file);
    }

    // reselect all previously selected files
    QPtrListIterator<KFileItem> it(*selectedFiles);
    for (; it.current(); ++it) {
        m_fileView->setSelected(it.current(), true);
    }

    //these steps are only necessary, if the internal viewer is used
    if (Settings::toolsUseInternalImageViewer()) {
        //force a redraw before the imageView is updated
        m_fileView->updateView(true);
        // and then update the viewer
        m_dlgImageViewer->updateImages();
    }
}


void KPhotoBookView::storeConfiguration() const
{
    KConfig* config = KGlobal::config();

    QString group = QString("ImageViewer:%1").arg(*(m_photobook->engine()->uid()));
    config->setGroup(group);
    config->writeEntry("DialogSize", m_dlgImageViewer->size());

    // force writing
    config->sync();
}


void KPhotoBookView::loadConfiguration()
{
    KConfig* config = KGlobal::config();

    QString group = QString("ImageViewer:%1").arg(*(m_photobook->engine()->uid()));
    config->setGroup(group);
    QSize defaultSize = QSize(640, 480);
    m_dlgImageViewer->resize(config->readSizeEntry("DialogSize", &defaultSize));
}


void KPhotoBookView::updateCurrentImageSize()
{
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
void KPhotoBookView::slotLoadSettings()
{
    updateCurrentImageSize();

    if (Settings::imagePreviewSelectionMode() == Settings::EnumImagePreviewSelectionMode::Extended) {
        m_fileView->setSelectionMode(KFile::Extended);
    } else {
        m_fileView->setSelectionMode(KFile::Multi);
    }

    m_fileView->setFont(Settings::imagePreviewFont());
}


//
// protected
//
void KPhotoBookView::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Control) {
        m_photobook->startTemporaryUnlockTagging();
    }

    e->ignore();
}


void KPhotoBookView::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Control) {
        m_photobook->stopTemporaryUnlockTagging();
    }

    e->ignore();
}


void KPhotoBookView::focusOutEvent(QFocusEvent* /* e */)
{
    m_photobook->stopTemporaryUnlockTagging();
}


//
// private slots
//
void KPhotoBookView::slotShowCurrentImage()
{
    if (Settings::toolsUseInternalImageViewer()) {
        tracer->sdebug(__func__) << "Opening internal Imageviewer ... " << endl;

        m_dlgImageViewer->updateImages();
        m_dlgImageViewer->showImage(dynamic_cast<File*>(m_fileView->currentFileItem()));
    } else {
        QString tool = Settings::toolsDefaultExternalTool();

        KFileItem* item = m_fileView->currentFileItem();
        KURL file = item->url();

        tracer->debug(__func__, "Showing file in %s. url=<%s>, path=<%s>", tool.ascii(), file.url().ascii(), file.path().ascii());

        KProcess *proc = new KProcess();

        *proc << tool << file.path();
        proc->start(KProcess::DontCare);
    }
}


//
// private methods
//
void KPhotoBookView::removeAllFiles()
{
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
