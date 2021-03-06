/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#include "kphotobookview.h"

#include "configuration.h"
#include "tagtree.h"
#include "sourcedirtree.h"
#include "kphotobook.h"
#include "constants.h"
#include "sourcedir.h"
#include "file.h"
#include "tagtreenodesourcedir.h"

#include <qobject.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qstring.h>
#include <qpopupmenu.h>

#include <kurl.h>

#include <ktrader.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <krun.h>
#include <klocale.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kfileiconview.h>


KPhotoBookView::KPhotoBookView(QWidget *parent)
    : QWidget(parent) {

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

    // image size panel
    QWidget* imageSizePanel = new QWidget(mainPanel, "imageSizePanel");
    QHBoxLayout* imageSizePanelLayout = new QHBoxLayout(imageSizePanel, 5, 5, "imageSizePanelLayout");
    imageSizePanelLayout->setAutoAdd(true);

    new QLabel(i18n("Image size:"), imageSizePanel, "imageSizeLabel");

    m_imageSizeSlider = new QSlider(4, 20, 1, Configuration::getInstance()->previewSize()/8, Qt::Horizontal, imageSizePanel, "imageSizeSlider");
    m_imageSizeSlider->setTickmarks(QSlider::Above);
    QObject::connect(m_imageSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(slotImageSizeSliderChanged(int)));
    QObject::connect(m_imageSizeSlider, SIGNAL(sliderPressed()), this, SLOT(slotImageSizeSliderPressed()));
    QObject::connect(m_imageSizeSlider, SIGNAL(sliderReleased()), this, SLOT(slotImageSizeSliderReleased()));

    m_currentImageSize = new QLabel("", imageSizePanel, "currentImageSize");
    updateCurrentImageSizeLabel();

    // file preview
    m_fileView = new KFileIconView(mainPanel, "fileIconView");
    m_fileView->setSelectionMode(KFile::Multi);
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


TagTreeNodeSourceDir* KPhotoBookView::selectedSourceDir() {

    QListViewItem* currentItem = m_sourcedirTree->currentItem();
    if (typeid(*currentItem) != typeid(TagTreeNodeSourceDir)) {
        kdDebug() << "[KPhotoBookView::selectedSourceDir] sourcedirTree contains a node of other type than 'TagTreeNodeSourceDir'" << endl;
        return 0;
    }

    return dynamic_cast<TagTreeNodeSourceDir*>(currentItem);
}


void KPhotoBookView::removeTagNode(TagTreeNode* node) {

    // the node's destructor removes itself from the tree
    delete node;
}


void KPhotoBookView::updateFiles() {

    kdDebug() << "[KPhotoBookView::updateFiles] updating the displayed images." << endl;

    // remove all displayed images
    removeAllFiles();

    // add the images
    QPtrList<File>* fileList = m_photobook->files(QString::null);

    File* file;
    for ( file = fileList->first(); file; file = fileList->next() ) {
        m_fileView->insertItem(file);
    }
}


void KPhotoBookView::storeConfiguration() {
    // store slider position
    Configuration::getInstance()->setPreviewSize(8*m_imageSizeSlider->value());
    // store splitter sizes
    Configuration::getInstance()->setMainSplitterSizes(m_split->sizes());
    Configuration::getInstance()->setTreeSplitterSizes(m_treesplit->sizes());
}


//
// private slots
//
void KPhotoBookView::slotImageSizeSliderPressed() {

    m_sliderPressed = true;
}


void KPhotoBookView::slotImageSizeSliderReleased() {

    m_sliderPressed = false;

    updateCurrentImageSizeLabel();

    // slider released after dragging it --> redraw previews
    updateCurrentImageSize();
}


void KPhotoBookView::slotImageSizeSliderChanged(int size) {
    size = size;

    updateCurrentImageSizeLabel();

    // slider position changed without dragging it --> redraw previews
    if (!m_sliderPressed) {
        updateCurrentImageSize();
    }
}


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
void KPhotoBookView::updateCurrentImageSizeLabel() {

    QString text = QString("%1 x %2").arg(8*m_imageSizeSlider->value()).arg(8*m_imageSizeSlider->value());
    m_currentImageSize->setText(text);
}


void KPhotoBookView::updateCurrentImageSize() {

    // TODO: remember all selected files

    // remove all displayed images
    removeAllFiles();

    m_fileView->setPreviewSize(8*m_imageSizeSlider->value());
    m_fileView->showPreviews();
    m_fileView->setGridX(8*m_imageSizeSlider->value()+10);
    m_fileView->setGridY(8*m_imageSizeSlider->value()+10);

    // force redrawing all files by removing them and adding them again
    updateFiles();

    // TODO: reselect all previously selected files
}


void KPhotoBookView::removeAllFiles() {

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
