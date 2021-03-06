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

#ifndef _IMAGEVIEWER_H_
#define _IMAGEVIEWER_H_

#include "ptrringbuffer.h"
class File;
class KFileIconView;
class Tracer;

#include <qimage.h>
#include <qwidget.h>
class QLabel;
class QMenuBar;
class QPopupMenu;
class QTimer;


class XImage
{
    private:
        static Tracer* tracer;

    public:
        XImage(int maxWidth = -1, int maxHeight = -1);
        virtual ~XImage();

        void     setFile(File* file);
        File*    file() { return m_file; };

        void     free();

        QPixmap* scaled()                { return &m_scaled; };

        void     setSmoothScale(bool);

        void     setMaxDimensions(int maxWidth, int maxHeight);

        bool     isValid();
        bool     workLeft();

        bool     doWork(bool forceFull = false);
        void     scale(int desiredWidth, int desiredHeight, bool forceDoWork = false);


    private:
        bool     loadImage();
        bool     convertImage();
        bool     scaleImage();

        bool     m_smoothScale;

        int      m_maxWidth;
        int      m_maxHeight;
        int      m_desiredWidth;
        int      m_desiredHeight;

        File*    m_file;

        QImage   m_image;
        QPixmap  m_pixmap;
        QPixmap  m_scaled;

        int      m_alloc_context;
};


class ImageViewer : public QWidget
{
    Q_OBJECT

    private:
        static Tracer* tracer;

    public:
        ImageViewer( QWidget *parent, KFileIconView* fileView, const char *name=0);
        virtual ~ImageViewer();

        void  updateImageList();
        void  show(File* selectedFile);

        void  free();

    signals:
        void signalEnterPressed();

    protected:
        void  paintEvent( QPaintEvent  * e);
        void  resizeEvent( QResizeEvent * e);
        void  wheelEvent ( QWheelEvent  * e );
        void  keyPressEvent ( QKeyEvent * e );

        void  contextMenuEvent ( QContextMenuEvent * e );


    private slots:
        void  slotShowNextImage();
        void  slotShowPrevImage();

        void  slotWorkTimerFired();
        void  slotSlideshowTimerFired();

        void  slotToggleSmoothScaling();
        void  slotToggleShowImageCounter();
        void  slotToggleShowInfoOverlay();

        void  slotStartSlideshow(int secs);
        void  slotClose();

    private:
        void  generateImageCounterOverlay();
        void  generateInfoOverlay();

        void createOverlayBackground(QPixmap* targetPM, QSize size);

        void  buildPtrList(KFileIconView* view, PtrRingBuffer<File>& ringbuffer);

        int   m_screenWidth;
        int   m_screenHeight;

        QTimer*  m_workTimer;
        QTimer*  m_timerSlideshow;

        KFileIconView*      m_fileView;
        PtrRingBuffer<File> m_lstImages;

        XImage*  m_curImage;
        XImage*  m_nxtImage;
        XImage*  m_prvImage;

        XImage   m_imageData1;
        XImage   m_imageData2;
        XImage   m_imageData3;

        QPixmap  m_imageCounterOverlay;
        QPixmap  m_imageCounterOverlayBG;
        QPixmap  m_infoOverlay;
        QPixmap  m_infoOverlayBG;

        QPixmap m_bgPixmap;
};

#endif
