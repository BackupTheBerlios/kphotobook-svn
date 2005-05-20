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

#ifndef IMAGEVIEWER_H_
#define IMAGEVIEWER_H_

#include "../tracer/tracer.h"
#include "ptrringbuffer.h"
#include "../engine/file.h"

#include <qwidget.h>
#include <qimage.h>
#include <qptrlist.h>
#include <qtimer.h>
#include <kfileiconview.h>


class QLabel;
class QMenuBar;
class QPopupMenu;
class File;


class XImage
{
public:

    XImage(File* file, int desiredWidth = 0, int desiredHeight = 0,
           int maxWidth = 0, int maxHeight = 0);
    ~XImage();

    void   free();

    void  setFile(File* file, int desiredWidth = 0, int desiredHeight = 0,
                  int maxWidth = 0, int maxHeight = 0);

    QImage*    image()                 { return m_image;        };
    QPixmap*   pixmap()                { return m_pixmap;       };
    QPixmap*   scaled()                { return m_scaledPixmap; };

    bool       isValid();

    void       scale(int desiredWidth, int desiredHeight) ;

    bool       doWork(bool forceFull = false);
    bool       workLeft();



private:
    bool      loadImage();
    bool      convertImage();
    bool      scaleImage();

    void      drawFileInfos();

    int      m_maxWidth;
    int      m_maxHeight;
    int      m_desiredWidth;
    int      m_desiredHeight;

    File*    m_file;

    QImage*  m_image;
    QPixmap* m_pixmap;
    QPixmap* m_scaledPixmap;

    int      m_alloc_context;
};







class ImageViewer : public QWidget
{
Q_OBJECT

private:
    static Tracer* tracer;

public:
    ImageViewer( QWidget *parent, KFileIconView* fileView, const char *name=0);
    ~ImageViewer();
    bool        loadImage( File* , XImage* img);

    void        updateImageList();
    void        show(File* selectedFile);

    void        free();

protected:
    void        paintEvent( QPaintEvent  * e);
    void        resizeEvent( QResizeEvent * e);
//     void        mouseDoubleClickEvent ( QMouseEvent * e );
    void        contextMenuEvent ( QContextMenuEvent * e );
    void        wheelEvent ( QWheelEvent  * e );

private slots:
    void        slotWorkTimerFired();
    void        slotToggleSmoothScaling();

private:
    int         m_screenWidth;
    int         m_screenHeight;

    void        updateStatus();
    void        setMenuItemFlags();

    void buildPtrList(KFileIconView* view, PtrRingBuffer<File>& ringbuffer);

    bool m_smoothScale;


    QTimer*     m_workTimer;


    KFileIconView*      m_fileView;

    PtrRingBuffer<File> m_lstImages;


    XImage*     m_curImage;
    XImage*     m_nxtImage;
    XImage*     m_prvImage;
};




#endif

