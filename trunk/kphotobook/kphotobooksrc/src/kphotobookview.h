/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#ifndef _KPHOTOBOOKVIEW_H_
#define _KPHOTOBOOKVIEW_H_

#include <kfileiconview.h>

#include <qwidget.h>
#include <qslider.h>
#include <qsplitter.h>
#include <qlabel.h>
#include <qptrlist.h>

class KPhotoBook;
class SourceDir;
class SourceDirTree;
class TagTreeNodeSourceDir;
class TagTree;
class TagTreeNode;
class TagTreeNodeTitle;

/**
 * This is the main view class for KPhotoBook.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * This kphotobook uses an HTML component as an example.
 *
 * @short Main view
 * @author Michael Christen <starcube@my-mail.ch>
 * @version 0.1
 */
class KPhotoBookView : public QWidget {

    Q_OBJECT

public:
    /**
     * Default constructor
     */
    KPhotoBookView(QWidget* parent);

    /**
     * Destructor
     */
    virtual ~KPhotoBookView();

    TagTree* tagTree() {
        return m_tagtree;
    }

    SourceDirTree* sourceDirTree() {
        return m_sourcedirTree;
    }

    KFileIconView* fileView() {
        return m_fileView;
    }

    TagTreeNodeSourceDir* selectedSourceDir();

    /**
     * Removes the specified node from the tagNodeTree.
     */
    void removeTagNode(TagTreeNode* node);

    /**
     * Updates the displayed images.
     */
    void updateFiles(QPtrList<KFileItem> *selectedFiles = 0);

    void storeConfiguration();

    void removeAllFiles();
signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

private slots:
    void slotImageSizeSliderChanged(int size);
    void slotImageSizeSliderPressed();
    void slotImageSizeSliderReleased();

    void slotShowCurrentImage();

private:
    bool m_sliderPressed;

    KPhotoBook* m_photobook;

    QSplitter* m_split;
    QSplitter* m_treesplit;

    TagTree* m_tagtree;
    SourceDirTree* m_sourcedirTree;
    KFileIconView* m_fileView;

    QSlider* m_imageSizeSlider;
    QLabel* m_currentImageSize;

    void updateCurrentImageSizeLabel();
    void updateCurrentImageSize();
};

#endif
