/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#include "kphotobookview.h"

#include "constants.h"
#include "configuration.h"
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
    : QWidget(parent) {
//    , m_sliderPressed(false) {

    // store casted pointer to the photobook
    m_photobook = dynamic_cast<KPhotoBook*>(parent);

    // setup our layout manager to automatically add our widgets
    QHBoxLayout* topLayout = new QHBoxLayout(this, 5, 5, "topLayout");
    topLayout->setAutoAdd(true);

    // splitter
    m_split = new QSplitter(Qt::Horizontal, this, "split");
    m_split->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // prepare the tagtree container
    QWidget* outerPanel = new QWidget(m_split, "outerPanel");
    QVBoxLayout* outerPanelLayout = new QVBoxLayout(outerPanel, 5, 5, "outerPanelLayout");
    outerPanelLayout->setAutoAdd(true);

    // treesplitter
    m_treesplit = new QSplitter(Qt::Vertical, outerPanel, "treesplit");
    m_treesplit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // append the tagtree
    m_tagtree = new TagTree(m_treesplit, m_photobook, "tagtree");

    // append the sourcedirTree
    m_sourcedirTree = new SourceDirTree(m_treesplit, m_photobook, "sourcedirTree");

    // main panel
    QWidget* mainPanel = new QWidget(m_split, "mainPanel");
    QVBoxLayout* mainLayout = new QVBoxLayout(mainPanel, 5, 5, "mainLayout");
    mainLayout->setAutoAdd(true);

    // file preview
    m_fileView = new KFileIconView(mainPanel, "fileIconView");
    if (Settings::imagePreviewSelectionMode() == Settings::EnumImagePreviewSelectionMode::Extended) {
        m_fileView->setSelectionMode(KFile::Extended);
    } else {
        m_fileView->setSelectionMode(KFile::Multi);
    }
    m_fileView->setResizeMode(KFileIconView::Adjust);
    m_fileView->showPreviews();
    m_fileView->setWordWrapIconText(false);
    updateCurrentImageSize();

    QObject::connect(m_fileView, SIGNAL(doubleClicked(QIconViewItem*, const QPoint&)), this, SLOT(slotShowCurrentImage()));
    QObject::connect(m_fileView, SIGNAL(selectionChanged ()), m_photobook, SLOT(slotFileSelectionChanged()));

    // set splitter position
    m_split->setSizes(Configuration::getInstance()->mainSplitterSizes());
    m_treesplit->setSizes(Configuration::getInstance()->treeSplitterSizes());
}


KPhotoBookView::~KPhotoBookView() {

    kdDebug() << "KPhotoBookView::~KPhotoBookView() invoked..." << endl;

    // remove the current previewed files
    //m_fileView->clearView();

    // all components are deleted automagically by the destructor of QWidget
}


SourceDirTreeNode* KPhotoBookView::selectedSourceDir() {

    QListViewItem* currentItem = m_sourcedirTree->currentItem();
    if (typeid(*currentItem) != typeid(SourceDirTreeNode)) {
        kdDebug() << "[KPhotoBookView::selectedSourceDir] sourcedirTree contains a node of other type than 'SourceDirTreeNode'" << endl;
        return 0;
    }

    return dynamic_cast<SourceDirTreeNode*>(currentItem);
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
    // store splitter sizes
    Configuration::getInstance()->setMainSplitterSizes(m_split->sizes());
    Configuration::getInstance()->setTreeSplitterSizes(m_treesplit->sizes());
}


void KPhotoBookView::increasePreviewSize() {

    Settings::setImagePreviewSize(Settings::imagePreviewSize() + 8);

    m_photobook->enableZoomIn(Settings::imagePreviewSize() < Constants::SETTINGS_MAX_PREVIEW_SIZE);
    m_photobook->enableZoomOut(Settings::imagePreviewSize() > Constants::SETTINGS_MIN_PREVIEW_SIZE);

    updateCurrentImageSize();
}


void KPhotoBookView::decreasePreviewSize() {

    Settings::setImagePreviewSize(Settings::imagePreviewSize() - 8);

    m_photobook->enableZoomIn(Settings::imagePreviewSize() < Constants::SETTINGS_MAX_PREVIEW_SIZE);
    m_photobook->enableZoomOut(Settings::imagePreviewSize() > Constants::SETTINGS_MIN_PREVIEW_SIZE);

    updateCurrentImageSize();
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

    kdDebug() << "Showing file in kuickshow. url=<" << file.url() << ">, path=<" << file.path() << ">" << endl;

    KProcess *proc = new KProcess();

    *proc << "kuickshow";
    *proc << file.path();
    proc->start(KProcess::DontCare);
}


//
// private methods
//
void KPhotoBookView::updateCurrentImageSize() {

    // remember all selected files
    QPtrList<KFileItem> selectedFiles(*m_fileView->selectedItems());

    // remove all displayed images
    removeAllFiles();

    m_fileView->setPreviewSize(Settings::imagePreviewSize());
    m_fileView->showPreviews();
    m_fileView->setGridX(Settings::imagePreviewSize()+10);
    m_fileView->setGridY(Settings::imagePreviewSize()+10);

    // force redrawing all files by removing them and adding them again
    updateFiles(&selectedFiles);
}


void KPhotoBookView::removeAllFiles() {

    // we must deselct all files before removing it to inprove the speed
    m_fileView->clearSelection();

    QPtrList<QIconViewItem>* itemList = new QPtrList<QIconViewItem>();

    for (QIconViewItem* item = m_fileView->firstItem(); item; item = item->nextItem()) {
        itemList->append(item);
    }

    for (QIconViewItem* item = itemList->first(); item; item = itemList->next()) {
        m_fileView->takeItem(item);
    }

    delete itemList;
}


#include "kphotobookview.moc"
