/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#ifndef _KPHOTOBOOKVIEW_H_
#define _KPHOTOBOOKVIEW_H_

class DialogImageViewer;
class KPhotoBook;
class TagTreeNode;
class Tracer;

#include <kmdichildview.h>
class KFileItem;
class KFileIconView;


/**
 * This is the main view class for KPhotoBook.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * CVS-ID $Id$
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

        KFileIconView* fileView() const
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

        void storeConfiguration() const;
        void loadConfiguration();

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

