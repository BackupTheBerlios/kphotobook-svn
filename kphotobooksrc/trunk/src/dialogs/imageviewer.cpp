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

#include <klocale.h>



Tracer* ImageViewer::tracer = Tracer::getInstance("kde.kphotobook.dialogs", "ImageViewer");



ImageViewer::ImageViewer( QWidget* parent, KFileIconView* fileView, const char* name)
: QWidget( parent, name, Qt::WDestructiveClose)
{
    tracer->invoked("ImageViewer");

    m_fileView = fileView;

    //this is necessary, to be able to recieve key events
    this->setFocusPolicy(QWidget::WheelFocus);

    //init the timer used for preloading
    m_workTimer = new QTimer(0);
    connect(m_workTimer, SIGNAL(timeout()), this, SLOT(slotWorkTimerFired()));

    m_timerSlideshow = new QTimer(0);
    connect(m_timerSlideshow, SIGNAL(timeout()), this, SLOT(slotSlideshowTimerFired()));

    //and initialize the images
    m_imageData1 = XImage();
    m_curImage = &m_imageData1;
    m_imageData2 = XImage();
    m_nxtImage = &m_imageData2;
    m_imageData3 = XImage();
    m_prvImage = &m_imageData3;

    //TODO maybe make a persistent setting of these properties?
    //smooth scaling is disabled by default. This can be made a setting later.
    m_smoothScale = false;

    //we want the contextGauge
    m_showContextGauge = true;

    // we want file infos
    m_showInfoOverlay = true;

    //this disables deletion of pixels, we make the bg drawing ourself
    this->setBackgroundMode(NoBackground);

    // retrieve screenGeometry of the screen the cursor is on
    QDesktopWidget* desktop = new QDesktopWidget();
    m_screenWidth  = desktop->screenGeometry(QCursor::pos()).width();
    m_screenHeight = desktop->screenGeometry(QCursor::pos()).height();
    delete desktop;
    tracer->sinfo("ImageViewer") << "Screengeometry is " << m_screenWidth << "x" << m_screenHeight << endl;

    //init the background pixmap
    m_bgPixmap = QPixmap(m_screenWidth, m_screenHeight);
    m_bgPixmap.fill(Qt::black);
}


ImageViewer::~ImageViewer() {

    tracer->invoked("~ImageViewer") ;

    //delete the timers
    delete m_workTimer;
    delete m_timerSlideshow;
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
 */
void ImageViewer::updateImageList()
{
    // remember the current image for later reuse...
    File* tmp = m_lstImages.current();

    //now buildup the ringbuffer from the images shown in the listview
    buildPtrList(m_fileView, m_lstImages);

    //try to make the images, that was previously current the current again ;-)
    if (m_lstImages.findRef(tmp) < 0) {
        m_lstImages.first();
    }

    // if we are visible, we update images on changes in the listview
    if (isVisible()) {
        //first the current image
        m_curImage->setFile(m_lstImages.current());
        m_curImage->scale(width(), height());
        m_curImage->doWork(true);
        update();

        //and then the other cached ones
        m_nxtImage->setFile(m_lstImages.getNext());
        m_nxtImage->scale(width(), height());
        m_prvImage->setFile(m_lstImages.getPrev());
        m_prvImage->scale(width(), height());

        //start preloader, but give it some time to rerender the listview...
        m_workTimer->start(100, true);
    }
}



/**
 * @param selectedFile the file that is currently selected (ie doubleclickted) in
 * the listview
 */
void ImageViewer::show(File* selectedFile)
{
    // if a NEW image should be shown....
    if (m_lstImages.current() != selectedFile) {

        // and a non null one...
        if (selectedFile != 0) {
            // try to set it as the current file
            if (m_lstImages.find(selectedFile) < 0) {
                m_lstImages.first();
            }
            generateContextCounterOverlay();
            generateInfoOverlay();
            m_curImage->setFile(m_lstImages.current());
            m_curImage->scale(width(), height());
            m_curImage->doWork(true);
            repaint();
        }
    }

    //... else only update the surrounding images
    m_nxtImage->setFile(m_lstImages.getNext());
    m_nxtImage->scale(width(), height());

    m_prvImage->setFile(m_lstImages.getPrev());
    m_prvImage->scale(width(), height());

    //start preloader...
    m_workTimer->start(0, true);
}



void ImageViewer::buildPtrList(KFileIconView* view, PtrRingBuffer<File>& ringbuffer)
{
    ringbuffer.clear();

    // loop over all files and add them to the ringbuffer
    QPtrListIterator<KFileItem> it(*view->items());

    for (; it.current(); ++it) {
        File* selectedFile = dynamic_cast<File*>(it.current());
        if (selectedFile) {
            ringbuffer.append(selectedFile);
        }
    }

    ringbuffer.first();
}




/**
 * this slot is called, when the working timer fires
 */
void ImageViewer::slotWorkTimerFired()
{
    tracer->sdebug("slotWorkTimerFired") << endl;

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






void ImageViewer::slotStartSlideshow(int secs)
{
    if (secs <= 0) {
        m_timerSlideshow->stop();
    }
    else {
        m_timerSlideshow->start(secs*1000);
    }
}


void ImageViewer::slotSlideshowTimerFired()
{
    slotShowNextImage();
}


void ImageViewer::slotToggleSmoothScaling()
{
    m_smoothScale = !m_smoothScale;

    m_curImage->setSmoothScale(m_smoothScale);
    m_prvImage->setSmoothScale(m_smoothScale);
    m_nxtImage->setSmoothScale(m_smoothScale);
}


void ImageViewer::slotToggleShowContextGauge()
{
    m_showContextGauge = !m_showContextGauge;

    if (m_showContextGauge) {
        generateContextCounterOverlay();
    }

    update();
}

void ImageViewer::slotToggleShowInfoOverlay()
{
    m_showInfoOverlay = !m_showInfoOverlay;

    if (m_showInfoOverlay) {
        generateInfoOverlay();
    }

    update();
}


void ImageViewer::wheelEvent (QWheelEvent* e ) {

    //stop the timer, if it is running
    if (m_workTimer->isActive()) {
        m_workTimer->stop();
    }

    bool up = e->delta() > 0;

    if (up) {
        slotShowNextImage();
    } else {
        slotShowPrevImage();
    }
}


void ImageViewer::slotShowNextImage()
{
    if (m_nxtImage->isValid()) {
        tracer->sdebug("showNextImage") << "precached nextImage found, using it!" << endl;

            //preserve the link...
        XImage* tmp = m_prvImage;
            //shift the data back by one
        m_prvImage = m_curImage;
        m_curImage = m_nxtImage;
        m_nxtImage = tmp;

        //first advance by one and then preload
        m_lstImages.next();
        generateContextCounterOverlay();
        generateInfoOverlay();

        //now force a redraw
        repaint();

        m_nxtImage->setFile(m_lstImages.getNext());
        m_nxtImage->scale(width(), height());
    } else {
        XImage* tmp = m_prvImage;
        m_prvImage = m_curImage;
        m_curImage = tmp;

        m_curImage->setFile(m_lstImages.next());
        m_curImage->scale(width(), height(), true);
        generateContextCounterOverlay();
        generateInfoOverlay();
        repaint();

        m_nxtImage->setFile(m_lstImages.getNext());
        m_nxtImage->scale(width(), height());
    }
    m_workTimer->start(0, true);

}


void ImageViewer::slotShowPrevImage()
{
    if (m_prvImage->isValid()) {
        tracer->sdebug("showPrevImage") << "precached prevImage found, using it!" << endl;

            //shift the data back by one
        XImage* tmp = m_nxtImage;
        m_nxtImage = m_curImage;
        m_curImage = m_prvImage;
        m_prvImage = tmp;

        //first advance back and then precache
        m_lstImages.prev();

        generateContextCounterOverlay();
        generateInfoOverlay();

        //now force a redraw
        repaint();

        m_prvImage->setFile(m_lstImages.getPrev());
        m_prvImage->scale(width(), height());
    } else {

        XImage*tmp = m_nxtImage;
        m_nxtImage = m_curImage;
        m_curImage = tmp;

        m_curImage->setFile(m_lstImages.prev());
        m_curImage->scale(width(), height(), true);
        generateContextCounterOverlay();
        generateInfoOverlay();
        repaint();

        m_prvImage->setFile(m_lstImages.getPrev());
        m_prvImage->scale(width(), height());
    }
    m_workTimer->start(0, true);
}


//TODO make the key stuff configurable, but I don't know how, yet
void ImageViewer::keyPressEvent ( QKeyEvent * e )
{
    switch (e->key()) {
    case Qt::Key_Right:
        slotShowNextImage();
        break;

    case Qt::Key_Left:
        slotShowPrevImage();
        break;

   //TODO
//     case Qt::Key_Enter:
//         ((QWidget*)parent())->toggleFullScreen();
//         break;

    default:
        e->ignore();
        break;
    }
}


//TODO probably move to the outer widget
void ImageViewer::contextMenuEvent ( __attribute__((unused)) QContextMenuEvent * e )
{
    QPopupMenu* popup = new QPopupMenu(this);


    QString c = QString("<font color=black><b>%1</b></font>")
        .arg(m_lstImages.current()->fileInfo()->fileName());

    QLabel* caption = new QLabel(c, this );
    caption->setAlignment( Qt::AlignCenter );
    popup->insertItem( caption );

    //TODO use kde actions/icons here, but I don't know, how to do that!
    popup->insertItem(i18n("Previous Image"), this, SLOT(slotShowPrevImage()));
    popup->insertItem(i18n("Next Image"), this, SLOT(slotShowNextImage()));

    popup->insertSeparator();
    int id = popup->insertItem(i18n("Toggle Fullscreen"), parent(), SLOT(slotToggleFullscreen()));
    popup->setItemChecked(id, ((QWidget*)parent())->isFullScreen());

    QPopupMenu* subSlideshow = new QPopupMenu(popup);
    c = QString("<font color=black><b> %1 </b></font>").arg(i18n("Slideshow"));
    caption = new QLabel(c, this );
    caption->setAlignment( Qt::AlignCenter );

    subSlideshow->insertItem( caption );
    id = subSlideshow->insertItem("Stop", 0);
    subSlideshow->setItemEnabled(id, m_timerSlideshow->isActive());

    subSlideshow->insertItem(" 5 sec", 5);
    subSlideshow->insertItem("10 sec", 10);
    subSlideshow->insertItem("20 sec", 20);
    subSlideshow->insertItem("30 sec", 30);
    subSlideshow->insertItem("40 sec", 40);
    subSlideshow->insertItem("50 sec", 50);
    subSlideshow->insertItem("60 sec", 60);

    connect(subSlideshow, SIGNAL(activated(int)), this, SLOT(slotStartSlideshow(int)));

    popup->insertSeparator();

    popup->insertItem(i18n("SlideShow"), subSlideshow);

    popup->insertSeparator();

    id = popup->insertItem(i18n("Use SmoothScaling"), this, SLOT(slotToggleSmoothScaling()));
    popup->setItemChecked(id, m_smoothScale);

    id = popup->insertItem(i18n("Show Image Counter"), this, SLOT(slotToggleShowContextGauge()));
    popup->setItemChecked(id, m_showContextGauge);

    id = popup->insertItem(i18n("Show Image Infos"), this, SLOT(slotToggleShowInfoOverlay()));
    popup->setItemChecked(id, m_showInfoOverlay);

    popup->exec(QCursor::pos());

    delete popup;

}










/**
 * draws the file counter and numbering in the edge
 */
void ImageViewer::generateContextCounterOverlay()
{
    //if we don't show it, we dont generate it!
    if (!m_showContextGauge) {
        return ;
    }

    int cur = m_lstImages.curIdx()+1;
    int max = m_lstImages.count();

    //TODO userdefinable
    QColor color(52,115,178);

    //the following code is taken from kpdfs presentation widget :-)
    int side = 70;

    side *= 2;
    QPixmap doublePixmap(side, side);
    doublePixmap.fill(Qt::black);
    QPainter pixmapPainter(&doublePixmap);

    if (max > 36) {
        int degrees = (int)( 360 * (float)(cur) / (float)max);
        pixmapPainter.setPen( 0x20 );
        pixmapPainter.setBrush( 0x60 );
        pixmapPainter.drawPie( 0, 0, side , side, 90*16, (360-degrees)*16 );
        pixmapPainter.setBrush( 0xD0 );
        pixmapPainter.drawPie( 0, 0, side, side, 90*16, -degrees*16 );

    } else {
        float oldCoord = -90;
        for ( int i = 0; i <= max; i++ ) {
            float newCoord = -90 + 360 * (float)(i) / (float)max;
        pixmapPainter.setPen( i <= cur ? 0x40 : 0x05 );
        pixmapPainter.setBrush( i <= cur ? 0xD0 : 0x60 );
            pixmapPainter.drawPie( 0, 0, side, side,
                                   (int)( -16*(oldCoord + 1) ), (int)( -16*(newCoord - (oldCoord + 2)) ) );
            oldCoord = newCoord;
        }
    }

    int ic= side * 3 / 5;
    QRect iRect = QRect((side - ic)/2, (side-ic)/2, ic, ic);

    pixmapPainter.setPen( Qt::black );
    pixmapPainter.setBrush( Qt::black);
    pixmapPainter.drawEllipse( iRect );

    // draw TEXT using maximum opacity
    QFont f( pixmapPainter.font() );

    f.setBold(true);
    if (cur < 100) {
        f.setPixelSize( side / 4 );
    }else if (cur < 1000) {
        f.setPixelSize( side / 5 );
    } else {
        f.setPixelSize( side / 6 );
    }
    pixmapPainter.setFont( f );
    pixmapPainter.setPen( 0xFF );
     // use a little offset to prettify output
    pixmapPainter.drawText( iRect.x()+2, iRect.y()+2, ic, ic * 7/12, Qt::AlignHCenter | Qt::AlignBottom, QString::number( cur ) );

    //draw the little dividing line
    pixmapPainter.setPen( QPen(0xAA, 4) );
    pixmapPainter.drawLine(iRect.x() + 3, iRect.y() + ic * 7/12 , iRect.x() + ic - 3 - 3, iRect.y() + ic * 7/12 );

    if (max < 100) {
        f.setPixelSize( side / 5 );
    }else if (max < 1000) {
        f.setPixelSize( side / 6 );
    } else {
        f.setBold(false);
        f.setPixelSize( side / 6 );
    }
    pixmapPainter.setFont(f);
    pixmapPainter.drawText(iRect.x()+2, iRect.y() + ic * 7/12, ic, ic /2, Qt::AlignHCenter | Qt::AlignTop, QString::number( max ) );

    // end drawing pixmap and halve image
    pixmapPainter.end();
    side /= 2;

    QImage image( doublePixmap.convertToImage().smoothScale( side, side));
    image.setAlphaBuffer( true );

    int red = color.red(), green = color.green(), blue = color.blue();
    int pixels = image.width() * image.height();

    unsigned int * data = (unsigned int *)image.bits();
    for( int i = 0; i < pixels; ++i ) {
        data[i] = qRgba( red, green, blue, data[i] & 0xFF );
    }

    //then save it for the next time
    m_contextOverlay.convertFromImage( image );
}




void ImageViewer::generateInfoOverlay()
{
    //if we don't show it, we don't generate it!
    if (!m_showInfoOverlay) {
        return;
    }

    //TODO userdefinable
    QColor color(52,115,178);
    int fontSize = 22;

    QFont f;
    f.setPixelSize(fontSize);
    f.setBold(true);

    QFontMetrics fm(f);

    QString str = QString(i18n("File %1")).arg(m_lstImages.current()->fileInfo()->fileName());

    QRect rect = fm.boundingRect(str);

    QPixmap pm(rect.size() + QSize(10,14));
    pm.fill(Qt::black);

    QPainter pmp(&pm);

    pmp.setBrush(0x55);
    pmp.setPen(0x88);
    pmp.drawRoundRect(0,0, pm.width(), pm.height(), (8 * 200) / pm.width(), (8 * 200) / pm.height());

    pmp.setPen(0xFF);
    pmp.setFont(f);
    pmp.drawText(3, 3 + f.pixelSize(), str);

    pmp.end();

    QImage image( pm.convertToImage());
    image.setAlphaBuffer( true );

    int red = color.red(), green = color.green(), blue = color.blue();
    int pixels = image.width() * image.height();

    unsigned int * data = (unsigned int *)image.bits();
    for( int i = 0; i < pixels; ++i ) {
        data[i] = qRgba( red, green, blue, data[i] & 0xFF );
    }

    //finally save it
    m_infoOverlay.convertFromImage( image );
}







/*
  The resize event handler, if a valid pixmap was loaded it will call
  scale() to fit the pixmap to the new widget size.
*/

void ImageViewer::resizeEvent( QResizeEvent * ) {

    if ( !m_curImage->isValid() )  {         // we couldn't load the image
        tracer->sdebug("resizeEvent") << "pixmap is null, unable to display!" << endl;
        return;
    }

    int w = width();
    int h = height();

    //prepare the bgImage
    if (m_bgPixmap.width() < w || m_bgPixmap.height() < h) {
        m_bgPixmap.resize(w,h);
        m_bgPixmap.fill(Qt::black);
    }

    //scale the main image
    m_curImage->scale(w, h, true);

    // and then the cached ones
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
        //force the current image to finish its work!
        m_curImage->doWork(true);
        // we only call repaint here, if all work is done for the curImage
        if (!m_curImage->workLeft()) {
            repaint(e->rect());
        }
        return;
    }

    int w = width();
    int h = height();

    QPainter painter(this);
    painter.setClipRect(e->rect());


    //calculate the base of the image to have it centered
    int x =( w - m_curImage->scaled()->width())  / 2;
    int y =( h - m_curImage->scaled()->height()) / 2;

    //only draw the black bg, if the image does not cover the whole widget
    if (x != 0 || y != 0) {
        //ok, we draw a black background ourself and disable the bg handling of the
        // widget (setBackgroundMode(NoBackground). this avoids flickering
        //TODO see, if this is a speed issue
        //TODO we could draw the bg only there, where it is visible, ie not behind the image
        if (m_bgPixmap.width() < w || m_bgPixmap.height() < h) {
            m_bgPixmap.resize(w,h);
            m_bgPixmap.fill(Qt::black);
        }
        painter.drawPixmap(0, 0, m_bgPixmap);
    }

    painter.drawPixmap(x, y, *m_curImage->scaled());

    if (m_showContextGauge) {
        painter.drawPixmap(w - m_contextOverlay.width() - 10, 10, m_contextOverlay);
    }
    if (m_showInfoOverlay) {
        painter.drawPixmap(10, h - m_infoOverlay.height() - 10, m_infoOverlay);
    }

    painter.end();
}







////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


XImage::XImage(int maxWidth, int maxHeight)
{
    m_file   = 0L;
    m_image  = QImage();
    m_pixmap = QPixmap();
    m_scaled = QPixmap();

    m_smoothScale = false;

    free();

    setMaxDimensions(maxWidth, maxHeight);
}



XImage::~XImage()
{
    if ( m_alloc_context )
        QColor::destroyAllocContext( m_alloc_context );
}


void XImage::free()
{
    //hide a nasty warning
    if (!m_image.isNull()) {
        m_image = m_image.scale(0,0);
    }
    m_pixmap.resize(0,0);
    m_scaled.resize(0,0);
}


/**
 * sets, if smoothScaling should be used
 */
void XImage::setSmoothScale(bool sc)
{
    m_smoothScale = sc;
}


void XImage::setFile(File* file)
{
    if (m_file == file) {
        // nothing to be done!
        return;
    }

    free();

    m_file = file;
}


void XImage::setMaxDimensions(int maxWidth, int maxHeight)
{
    m_maxWidth  = maxWidth;
    m_maxHeight = maxHeight;
}


/**
 * does the work, that needs to be done, one step at a time:
 * the steps are:
 *     o loading of the image
 *     o converting to a pixmap
 *     o scaling
 *
 * if forceFull is true, not only one of these steps is done, but all!
 * @returns true, if work is left to be done, ie the 3 steps are not all done
 */
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
    if (m_image.isNull()) {
        loadImage();

        //if we just loaded the image, then there is work left
        if (!forceFull) {
            return true;
        }
    }

    //do we already have a converted tmp image?
    if (m_pixmap.isNull()) {
        convertImage();

        if (!forceFull) {
            return workLeft();
        }
    }

    //do we need scaling to be done?
    if (m_scaled.isNull()) {
        scaleImage();
    }

    return workLeft();
}



/**
 * @returns true, if work has to be done
 */
bool XImage::workLeft()
{
    // is one of them null, or is the scaled one null and a width/heigt are given?
    return (m_file
            && (m_image.isNull() || m_pixmap.isNull()
                || (m_scaled.isNull() && m_desiredWidth > 0 && m_desiredHeight > 0)));
}


/**
 * @returns true, if this image is valid, that means, data could be loaded from a file
 * and are ready for transitions
 */
bool XImage::isValid()
{
    return m_file && !m_image.isNull();
}



/**
 * asks the image to be scaled.
 *
 * if forceDoWork is false, the physical scaling will happen only with the
 * next doWork() call. otherwise it will be done instantly.
 *
 *
 */
void XImage::scale(int width, int height, bool forceDoWork) {

    //if we already have that size, do nothing;
    if (m_desiredWidth == width && m_desiredHeight == height)
        return ;

    m_scaled.resize(0,0);

    m_desiredWidth  = width;
    m_desiredHeight = height;

    if (forceDoWork) {
        doWork(true);
    }
}





/**
 * loads the image from a file to a QImage
 * this is the first step of works to be done
 *
 * @return true, if all went fine
 */
bool XImage::loadImage()
{
    if (!m_file) {
        return false;
    }

    bool success = m_image.load(m_file->fileInfo()->absFilePath());

    //TODO does this give any advantage probably on low-end machines?
//     //prescale the image to the windowgeometry
//     if (success
//         && m_maxWidth > 0 && m_maxHeight > 0
//         && m_image.width() > m_maxWidth && m_image.height() > m_maxHeight) {
//             m_image = m_image.smoothScale(m_maxWidth, m_maxHeight, QImage::ScaleMin);
//     }

    return success;
}




/**
 * converts the image from the QImage to the QPixmap
 * this is the second step of works
 *
 * @return true, if all went fine
 */
bool XImage::convertImage()
{
    //but we cannot work on it, if the src is null
    if (m_image.isNull()) {
        return false;
    }

    if ( m_alloc_context ) {
        QColor::destroyAllocContext( m_alloc_context );
        m_alloc_context = 0;
    }

    m_alloc_context = QColor::enterAllocContext();

    bool success = m_pixmap.convertFromImage(m_image);
    if (!success) {
        m_pixmap.resize(0,0);
    }

    QColor::leaveAllocContext();

    return success;
}


/**
 * scales the pixmap to a scaled version
 *
 * this is the third step
 *
 * @return true, if all went fine;
 */
bool XImage::scaleImage()
{

    if (m_pixmap.isNull() || m_desiredWidth <= 0 || m_desiredHeight <= 0) {
        return false;
    }

    // is the src already scaled right?
    if ( m_pixmap.width() == m_desiredWidth && m_pixmap.height() == m_desiredHeight) {
        //simply copy the image, if the already fits
        m_scaled = m_pixmap;
    } else {
        if (m_smoothScale) {
            // if we smoothscale, we directly scale from the m_image, not the pixmap
            //FIXME so if using smoothScaling, the second step could be omitted !!
            m_scaled.convertFromImage(m_image.smoothScale(m_desiredWidth, m_desiredHeight, QImage::ScaleMin));

        } else {

            double x = ((double)m_desiredWidth)  / m_pixmap.width();
            double y = ((double)m_desiredHeight) / m_pixmap.height();
            double scale = 0;

            // now find out, how we should scale...
            if (x > 1 && y > 1) {
                scale = (x >= y ? y : x);
            } else if (x > 1) {
                scale = y;
            } else if (y > 1) {
                scale = x;
            } else {
                scale = (x >= y ? y : x);
            }

            QWMatrix sm;
            sm.scale(scale, scale);
            m_scaled = m_pixmap.xForm(sm);
        }
    }

    return true;
}



