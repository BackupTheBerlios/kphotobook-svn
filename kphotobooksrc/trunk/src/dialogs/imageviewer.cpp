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

#include "imageviewer.h"

#include "../tracer/tracer.h"

#include <qmenubar.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qclipboard.h>

#include <qcursor.h>
#include <qdatetime.h>



Tracer* ImageViewer::tracer = Tracer::getInstance("kde.kphotobook.dialogs", "ImageViewer");



ImageViewer::ImageViewer( QWidget* parent, KFileIconView* fileView, const char* name)
: QWidget( parent, name, Qt::WDestructiveClose)
{
    tracer->invoked("ImageViewer");

    m_fileView = fileView;

    //init the timer used for preloading
    m_workTimer = new QTimer(0);
    connect(m_workTimer, SIGNAL(timeout()), this, SLOT(slotWorkTimerFired()));

    m_timerSlideshow = new QTimer(0);
    connect(m_timerSlideshow, SIGNAL(timeout()), this, SLOT(slotSlideshowTimerFired()));

    //and initialize the images
    m_curImage = new XImage(0L);
    m_nxtImage = new XImage(0L);
    m_prvImage = new XImage(0L);

    //smooth scaling is disabled by default. This can be made a setting later.
    m_smoothScale = false;

    //yes, we want a black background.
    this->setPaletteBackgroundColor(Qt::black);

    // retrieve screenGeometry of the screen the cursor is on
    QDesktopWidget* desktop = new QDesktopWidget();
    m_screenWidth  = desktop->screenGeometry(QCursor::pos()).width();
    m_screenHeight = desktop->screenGeometry(QCursor::pos()).height();
    delete desktop;
    tracer->sinfo("ImageViewer") << "Screengeometry is " << m_screenWidth << "x" << m_screenHeight << endl;
}


ImageViewer::~ImageViewer() {

    tracer->invoked("~ImageViewer") ;

    delete m_workTimer;
    delete m_curImage;
    delete m_nxtImage;
    delete m_prvImage;
}



void ImageViewer::free()
{
    m_curImage->free();
    m_nxtImage->free();
    m_prvImage->free();
}



/**
 * Sets the List of images this viewer should show
 *
 * @param images The list of images to show
 */
void ImageViewer::updateImageList()
{
    // remember the current for later reuse...
    File* tmp = m_lstImages.current();

    buildPtrList(m_fileView, m_lstImages);

    //if no file is given, try to make the images, that was previously current the current ;-)
    if (m_lstImages.findRef(tmp) < 0) {
        m_lstImages.first();
    }


    if (isVisible()) {

        m_curImage->setFile(m_lstImages.current());
        m_curImage->setImageContext(m_lstImages.curIdx(), m_lstImages.count());
        m_curImage->setDesiredDimensions(width(), height());
        m_curImage->doWork(true);
        update();

        m_nxtImage->setFile(m_lstImages.getNext());
        m_nxtImage->setImageContext(m_lstImages.nxtIdx(), m_lstImages.count());
        m_nxtImage->setDesiredDimensions(width(), height());
        m_nxtImage->setMaxDimensions(m_screenWidth, m_screenHeight);

        m_prvImage->setFile(m_lstImages.getPrev());
        m_prvImage->setImageContext(m_lstImages.prvIdx(), m_lstImages.count());
        m_prvImage->setDesiredDimensions(width(), height());
        m_prvImage->setMaxDimensions(m_screenWidth, m_screenHeight);

        //start preloader...
        m_workTimer->start(100, true);
    }
}



void ImageViewer::show(File* selectedFile)
{
    if (m_lstImages.current() != selectedFile) {

        if (selectedFile != 0) {
        // if a file is given, set it as the current file
            if (m_lstImages.find(selectedFile) < 0) {
                m_lstImages.first();
            }

            m_curImage->setFile(m_lstImages.current());
            m_curImage->setImageContext(m_lstImages.curIdx(), m_lstImages.count());
            m_curImage->setDesiredDimensions(width(), height());
            m_curImage->setMaxDimensions(m_screenWidth, m_screenHeight);
            m_curImage->doWork(true);
            update();
        }
    }

    m_nxtImage->setFile(m_lstImages.getNext());
    m_nxtImage->setImageContext(m_lstImages.nxtIdx(), m_lstImages.count());
    m_nxtImage->setDesiredDimensions(width(), height());
    m_nxtImage->setMaxDimensions(m_screenWidth, m_screenHeight);

    m_prvImage->setFile(m_lstImages.getPrev());
    m_prvImage->setImageContext(m_lstImages.prvIdx(), m_lstImages.count());
    m_prvImage->setDesiredDimensions(width(), height());
    m_prvImage->setMaxDimensions(m_screenWidth, m_screenHeight);

    //start preloader...
    m_workTimer->start(0, true);
}



void ImageViewer::buildPtrList(KFileIconView* view, PtrRingBuffer<File>& ringbuffer)
{
    ringbuffer.clear();

    // loop over all selected files and add them to the ringbuffer
    QPtrListIterator<KFileItem> it(*view->items());

    for (; it.current(); ++it) {
        File* selectedFile = dynamic_cast<File*>(it.current());
        if (selectedFile) {
            ringbuffer.append(selectedFile);
        }
    }

    ringbuffer.first();
}





void ImageViewer::slotWorkTimerFired()
{
    tracer->sinfo("slotWorkTimerFired") << endl;

    setCursor(Qt::BusyCursor);

    if (m_curImage->doWork())
        m_workTimer->start(0, true);
    else if (m_nxtImage->doWork())
        m_workTimer->start(0, true);
    else if (m_prvImage->doWork())
        m_workTimer->start(0, true);

    if (!m_workTimer->isActive())
        setCursor(Qt::ArrowCursor);
}






/*
  The resize event handler, if a valid pixmap was loaded it will call
  scale() to fit the pixmap to the new widget size.
*/

void ImageViewer::resizeEvent( QResizeEvent * ) {

    if ( m_curImage->pixmap()->isNull() )  {         // we couldn't load the image
        tracer->sdebug("resizeEvent") << "pixmap is null, unable to display!" << endl;
    }

    int w = width();
    int h = height();
    if ( w != m_curImage->scaled()->width() || h != m_curImage->scaled()->height()) {
        m_curImage->scale(w, h);
        m_curImage->doWork(true);
    }
    m_nxtImage->scale(w,h);
    m_prvImage->scale(w,h);

    m_workTimer->start(0, true);
}



/*
  Draws the portion of the scaled pixmap that needs to be updated or prints
  an error message if no legal pixmap has been loaded.
*/

void ImageViewer::paintEvent( QPaintEvent *e ) {
    if ( m_curImage->scaled()->isNull() ) {
        tracer->sdebug("paintEvent") << "preScaled Image is null, unable to display!" << endl;
        return;
    }
    QPainter painter(this);
    painter.setClipRect(e->rect());

        //move the image to the center
    int x =( width()  - m_curImage->scaled()->width())  / 2;
    int y =( height() - m_curImage->scaled()->height()) / 2;
    painter.drawPixmap(x, y, *m_curImage->scaled());
}









void ImageViewer::contextMenuEvent ( __attribute__((unused)) QContextMenuEvent * e )
{
    QPopupMenu* popup = new QPopupMenu(this);

    QString c = QString("<font color=black><b>%1</b></font>")
        .arg(m_lstImages.current()->fileInfo()->fileName());

    QLabel* caption = new QLabel(c, this );
    caption->setAlignment( Qt::AlignCenter );
    popup->insertItem( caption );


    QPopupMenu* subSlideshow = new QPopupMenu(popup);
    c = "<font color=black><b> Start Slideshow </b></font>";
    caption = new QLabel(c, this );
    caption->setAlignment( Qt::AlignCenter );

    subSlideshow->insertItem( caption );
    int id = subSlideshow->insertItem("Stop", 0);
    subSlideshow->setItemEnabled(id, m_timerSlideshow->isActive());

    subSlideshow->insertItem(" 5 sec", 5);
    subSlideshow->insertItem("10 sec", 10);
    subSlideshow->insertItem("20 sec", 20);
    subSlideshow->insertItem("30 sec", 30);
    subSlideshow->insertItem("40 sec", 40);
    subSlideshow->insertItem("50 sec", 50);
    subSlideshow->insertItem("60 sec", 60);

    connect(subSlideshow, SIGNAL(activated(int)), this, SLOT(slotStartSlideshow(int)));


    popup->insertItem("SlideShow", subSlideshow);

    id = popup->insertItem("SmoothScaling", this, SLOT(slotToggleSmoothScaling()));
    popup->setItemChecked(id, m_smoothScale);


    popup->exec(QCursor::pos());

    delete popup;

}


void ImageViewer::slotStartSlideshow(int id)
{
    if (id == 0) {
        m_timerSlideshow->stop();
    }
    else {
        m_timerSlideshow->start(id*1000);
    }
}


void ImageViewer::slotSlideshowTimerFired()
{
    wheelEvent(new QWheelEvent(QPoint(), QPoint(), 10, 0));
}


void ImageViewer::wheelEvent (QWheelEvent* e ) {

    bool up = e->delta() > 0;

    if (up) {

        if (m_nxtImage->isValid()) {
            tracer->sinfo("wheelEvent") << "precached nextImage found, using it!" << endl;

            //shift the data back by one
            delete m_prvImage;
            m_prvImage = m_curImage;
            m_curImage = m_nxtImage;
            //now force a redraw
            repaint();
            //first advance by one and then preload
            m_lstImages.next();
            m_nxtImage = new XImage(m_lstImages.getNext());
            m_nxtImage->setImageContext(m_lstImages.nxtIdx(), m_lstImages.count());
            m_nxtImage->setDesiredDimensions(width(), height());
         } else {

             delete m_prvImage;
             m_prvImage = m_curImage;

             m_curImage->setFile(m_lstImages.next());
             m_curImage->setImageContext(m_lstImages.curIdx(), m_lstImages.count());
             m_curImage->setDesiredDimensions(width(), height());
             m_curImage->setMaxDimensions(m_screenWidth, m_screenHeight);

             m_nxtImage->setFile(m_lstImages.getNext());
             m_nxtImage->setImageContext(m_lstImages.nxtIdx(), m_lstImages.count());
             m_nxtImage->setDesiredDimensions(width(), height());
             m_nxtImage->setMaxDimensions(m_screenWidth, m_screenHeight);
         }
    } else {
        if (m_prvImage->isValid()) {
            tracer->sinfo("wheelEvent") << "precached prevImage found, using it!" << endl;

            //shift the data back by one
            delete m_nxtImage;
            m_nxtImage = m_curImage;
            m_curImage = m_prvImage;
            //now force a redraw
            repaint();

            //first advance back and then precache
            m_lstImages.prev();

            m_prvImage = new XImage(m_lstImages.getPrev());
            m_prvImage->setImageContext(m_lstImages.prvIdx(), m_lstImages.count());
            m_prvImage->setDesiredDimensions(width(), height());
            m_prvImage->setMaxDimensions(m_screenWidth, m_screenHeight);
        } else {

            delete m_nxtImage;
            m_nxtImage = m_curImage;

            m_curImage->setFile(m_lstImages.prev());
            m_curImage->setImageContext(m_lstImages.curIdx(), m_lstImages.count());
            m_curImage->setDesiredDimensions(width(), height());
            m_curImage->setMaxDimensions(m_screenWidth, m_screenHeight);

            m_prvImage->setFile(m_lstImages.getPrev());
            m_prvImage->setImageContext(m_lstImages.prvIdx(), m_lstImages.count());
            m_prvImage->setDesiredDimensions(width(), height());
            m_prvImage->setMaxDimensions(m_screenWidth, m_screenHeight);
        }
    }
    m_workTimer->start(0, true);


}


void ImageViewer::slotToggleSmoothScaling()
{
    m_smoothScale = !m_smoothScale;
}




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


XImage::XImage(File* file, int desiredWidth, int desiredHeight, int maxWidth, int maxHeight)
{
    m_image        = new QImage();

    m_pixmap       = new QPixmap();
    m_scaledPixmap = new QPixmap();

    setFile(file);

    setDesiredDimensions(desiredWidth, desiredHeight);
    setMaxDimensions(maxWidth, maxHeight);
}



XImage::~XImage()
{
    if ( m_alloc_context )
        QColor::destroyAllocContext( m_alloc_context );

    delete m_image;
    delete m_pixmap;
    delete m_scaledPixmap;
}


void XImage::free()
{
    //hide a nasty warning
    if (!image()->isNull()) {
        *m_image = image()->scale(0,0);
    }
    if (pixmap()) {
        pixmap()->resize(0,0);
    }
    scaled()->resize(0,0);
}


void XImage::setFile(File* file)
{
    if (m_file == file) {
        // nothing to be done!
        return;
    }

    m_file = file;

    free();
    setImageContext(-2, -1);
}


void XImage::setImageContext(int imgNumber, int fromMaxImages)
{
    //FIXME here I add one, as I internally count from 1, not 0
    m_imageNumber = imgNumber + 1;
    m_fromMaxImages = fromMaxImages;
}


void XImage::setDesiredDimensions(int width, int height)
{
    m_desiredWidth = width;
    m_desiredHeight = height;
}


void XImage::setMaxDimensions(int maxWidth, int maxHeight)
{
    m_maxWidth = maxWidth;
    m_maxHeight = maxHeight;
}

bool XImage::doWork(bool forceFull)
{
    if (m_file == NULL)
        return false;

//      kdDebug() << "doWork() " << m_file->fileInfo()->fileName() << ": " << !m_image->isNull() << "<>"
//          << !m_pixmap->isNull() << "<>" << !m_scaledPixmap->isNull() << endl;


    if (!workLeft()){
        return false;
    }

    //if the image is not yet loaded... do so!
    if (image()->isNull()) {
        loadImage();

        if (!forceFull) {
            return workLeft();
        }
    }

    // TODO the big question is: do we need this middle step? as it increases mem usage pretty much
    //do we already have a converted tmp image?
    if (pixmap()->isNull()) {
        convertImage();

        if (!forceFull) {
            return workLeft();
        }
    }

    //do we need scaling to be done?
    if (scaled()->isNull()) {
        scaleImage();
    }

    return workLeft();
}




bool XImage::workLeft()
{
    return (m_file
            && (image()->isNull() || pixmap()->isNull()
                || (scaled()->isNull() && m_desiredWidth > 0 && m_desiredHeight > 0)));
}


bool XImage::isValid()
{
    return m_file && !image()->isNull();
}


void XImage::scale(int width, int height) {

    if (!scaled()->isNull()) {
        scaled()->resize(0,0);
    }

    m_desiredWidth  = width;
    m_desiredHeight = height;
}




bool XImage::loadImage()
{
    if (!m_file) {
        return false;
    }

    bool success = image()->load(m_file->fileInfo()->absFilePath());

    if (success
        && m_maxWidth > 0 && m_maxHeight > 0
        && image()->width() > m_maxWidth && image()->height() > m_maxHeight) {
            *m_image = image()->smoothScale(m_maxWidth, m_maxHeight, QImage::ScaleMin);
    }

    return success;
}





bool XImage::convertImage()
{
    if (image()->isNull()) {
        return false;
    }

    if ( m_alloc_context ) {
        QColor::destroyAllocContext( m_alloc_context );
        m_alloc_context = 0;
    }

    bool success = FALSE;
    m_alloc_context = QColor::enterAllocContext();
    {
        //try to convert the image to a pixmap
        if ( pixmap()->convertFromImage(*image()) ) {
            success = TRUE;
        } else {
            pixmap()->resize(0,0);
        }

    }
    QColor::leaveAllocContext();

    return success;
}



bool XImage::scaleImage()
{
    if (pixmap()->isNull() || m_desiredWidth <= 0 || m_desiredHeight <= 0) {
        return false;
    }
//     QTime timer;
//     timer.start();

    // is it already scaled right?
    if ( m_desiredWidth == pixmap()->width() && m_desiredHeight == pixmap()->height() ) {
        //simply copy the image, if the already fits
        //TODO see, if this is a performance issue an can be circumvented
        *(scaled()) = *(pixmap());

    } else {

        bool m_smoothScale = false;
        if (m_smoothScale) { // SMOOTH?
            scaled()->convertFromImage(image()->smoothScale(m_desiredWidth, m_desiredHeight, QImage::ScaleMin));
        } else {

            //this is the easy but slow solution...
//             m_scaledPixmap->convertFromImage(m_image->scale(m_desiredWidth, m_desiredHeight, QImage::ScaleMin));

            double x = ((double)m_desiredWidth)  / pixmap()->width();
            double y = ((double)m_desiredHeight) / pixmap()->height();
            double scale = 0;

            if (x > 1 && y > 1) {
                scale = (x >= y ? y : x);
            } else if (x > 1) {
                scale = y;
            } else if (y > 1) {
                scale = x;
            } else {
                scale = (x >= y ? y : x);
            }

            QWMatrix m;
            m.scale(scale, scale);
            *(scaled()) = pixmap()->xForm(m);
//             m_scaledPixmap = new QPixmap(m_pixmap->xForm( m ));
        }
    }

//     kdDebug() << "resize: "<<timer.restart() << endl;

    drawFileInfos();
//     kdDebug() << "drawFI: "<<timer.restart() << endl;

    return true;
}







void drawContextCounter(QPainter* p, int x, int y, int width, int height, int cur, int max)
{
    //TODO what needs to be configurable?
    //     fonts? colors? size? if its shown at all?


    if (!p) {
        return;
    }
    // full turn
    int full = 360*16;

    // want space be at most 3/4 of arc, but less then 4degrees
    // | max * (space + arc) = 360 |
    // | space = 3/4 *arc          |
    int arc = (4 * full ) / (7 * max) ;
    int space = 3 * arc / 4 ;

    // if the space is too big, reduce it to 4 degrees
    if (space > 4 * 16) {
        space = 4 * 16;
        arc = (full - (max * space)) / max;
    }



    //now draw the arcs, but, if we got more then 40, we don't draw them each...
    if (max > 40) {

        //first draw a full circle
        p->setPen(QPen(Qt::darkGray, width / 4));
        p->drawArc(x,y, width, height, 0, 360*16);

        //then mark the current position
        p->setPen(QPen(Qt::white, width / 4));

        arc = full / max;

        int pos = max - (cur - 2);
        if (cur == 1) {
            pos = 1;
        }

        p->drawArc(x,y, width, height, ((pos-1) * (arc)) + (90*16-arc), 4*16);

    } else {
        for (int i = 1; i <= max ; ++i) {

            //this is stupid: as qt draws a circle counterclockwise, I have to
            // translate the position of the current element
            if (((i == cur) && (cur== 1)) || (cur == (max-(i-2)))) {
                p->setPen(QPen(Qt::white, width / 4));
            } else {
                p->setPen(QPen(Qt::darkGray, width / 4));
            }
            p->drawArc(x,y, width, height, ((i-1) * (space+arc)) + (90*16-arc-(space/2)), arc);
        }
    }


    //now draw the number inside the circle
    p->setPen(Qt::red);
    p->setFont(QFont("Bitstream Vera Sans", 20));

    QString str = QString::number(cur);

    p->drawText(x + width/2  - (p->fontMetrics().width(str)/2),
                y + height/2 + (p->fontInfo().pixelSize()/2), str);
}


void XImage::drawFileInfos()
{
    if (scaled()->isNull())
        return;


    //Playing with a painter...
    QPainter p(scaled());

    p.setBrush(Qt::blue);
    p.setPen(Qt::blue);
    if (m_imageNumber >= 0 && m_fromMaxImages >= 0) {
        drawContextCounter(&p, scaled()->width()-60, 10, 50, 50, m_imageNumber, m_fromMaxImages);
    }

    p.setPen(Qt::red);
    p.setFont(QFont("Bitstream Vera Sans", 20));

    int x = 20;
    int y = scaled()->height() - 20;

    //TODO draw the data from the imgs somehow
    p.drawText(x, y, QString("File: %1").arg(m_file->fileInfo()->fileName()), -1);

    p.end();
}














/*
  This function loads an image from a file and resizes the widget to
  exactly fit the image size. If the file was not found or the image
  format was unknown it will resize the widget to fit the errorText
  message (see above) displayed in the current font.

  Returns TRUE if the image was successfully loaded.


bool ImageViewer::loadImage( File* file, XImage* img)
{
    if ( !file ) {
        return false;
    }

    bool ok = false;

    // this might take time
    QApplication::setOverrideCursor( Qt::BusyCursor );


    //load the file into the qimage
    ok = img->image()->load(file->fileInfo()->absFilePath(), 0);

    // convert the loaded image now to a pixmap
    if ( ok ) {
        ok = convertImage(img);
    }

    if ( ok ) {
        ok = scale(img);
    }

    if ( ! ok ) {
        // couldn't load image
        img->pixmap()->resize(0,0);
    }

    QApplication::restoreOverrideCursor();

    return ok;
}

*/


/*

bool ImageViewer::convertImage(XImage* img) {

    if ( !img->image() ) {
        return FALSE;
    }

    if ( m_alloc_context ) {
        QColor::destroyAllocContext( m_alloc_context );
        m_alloc_context = 0;
    }

    //CHECKME: whatever that is exactly used for?
    bool useColorContext =  true;

//     QApplication::setOverrideCursor( Qt::BusyCursor );

    //this clears the widget. it seems, that this is not necessary!
    if (useColorContext) {
        m_alloc_context = QColor::enterAllocContext();
    }

    bool success = FALSE;

    //try to convert the image to a pixmap
    if ( success = img->pixmap()->convertFromImage(*(img->image())) ) {
        success = TRUE;
    } else {
        img->pixmap()->resize(0,0);
    }

    if ( useColorContext ) {
        QColor::leaveAllocContext();
    }

//     QApplication::restoreOverrideCursor();

    // TRUE if loaded OK
    return success;
}




*/



/*
  This functions scales the pixmap in the member variable "pm" to fit the
  widget size and  puts the resulting pixmap in the member variable "pmScaled".


bool ImageViewer::scale(XImage* img) {

    if ( !img->image() )
        return false;

//     QApplication::setOverrideCursor( Qt::BusyCursor );

    // is it already scaled right?
    if ( width() == img->pixmap()->width() && height() == img->pixmap()->height() ) {
        *(img->scaled()) = *(img->pixmap());

    } else {

        if (m_smoothScale) { // SMOOTH?
            img->scaled()->convertFromImage(img->image()->smoothScale(width(), height(), QImage::ScaleMin));
        } else {
            img->scaled()->convertFromImage(img->image()->scale(width(), height(), QImage::ScaleMin));

//             double x = ((double)width())/img->pixmap->width();
//             double y = ((double)height())/img->pixmap->height();
//             double scale = 0;
//
//             tracer->xdbg("scale") << "x: " << x << " y: " << y << endl;
//
//             if (x < 1 && y < 1)
//             {
//                 scale = (x >= y ? y : x);
//             } else if (x < 1) {
//                 scale = y;
//             } else if (y < 1) {
//                 scale = x;
//             } else {
//                 scale = (x >= y ? x : y);
//             }
//
//             QWMatrix m;
//             m.scale(((double)1)/scale, ((double)1)/scale);
//             *(img->pixmapScaled) = img->pixmap->xForm( m );
        }
    }

    drawFileInfos(img);

//     QApplication::restoreOverrideCursor();

    return true;
}

*/


