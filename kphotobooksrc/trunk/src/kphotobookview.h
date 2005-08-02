/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#ifndef _KPHOTOBOOKVIEW_H_
#define _KPHOTOBOOKVIEW_H_

#include "dialogs/dialogimageviewer.h"
#include "tracer/tracer.h"

#include <kfileiconview.h>
#include <kmdichildview.h>

#include <qlabel.h>
#include <qptrlist.h>
#include <qsplitter.h>
#include <qwidget.h>

class KPhotoBook;
class SourceDir;
class SourceDirTree;
class SourceDirTreeNode;
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
class KPhotoBookView : public KMdiChildView {

    Q_OBJECT

    private:
        static Tracer* tracer;
    
    public:
        /**
        * Default constructor
        */
        KPhotoBookView(QWidget* parent);
    
        /**
        * Destructor
        */
        virtual ~KPhotoBookView();
    
        KFileIconView* fileView()
        {
            return m_fileView;
        }
    
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
    
        void updateCurrentImageSize();
    
    signals:
        /**
        * Use this signal to change the content of the statusbar
        */
        void signalChangeStatusbar(const QString& text);
    
    public slots:
        void slotLoadSettings();
    
    protected:
        void keyPressEvent(QKeyEvent* e);
        void keyReleaseEvent(QKeyEvent *e);
        void focusOutEvent(QFocusEvent *e);
    
    private slots:
        void slotShowCurrentImage();
    
    private:
        KPhotoBook* m_photobook;
    
        KFileIconView* m_fileView;
    
        DialogImageViewer* m_dlgImageViewer;
    
        int m_currentImagePreviewSize;
};

#endif

