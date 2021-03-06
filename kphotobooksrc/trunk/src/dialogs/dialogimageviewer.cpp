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

#include "dialogimageviewer.h"

#include "../engine/file.h"
#include "../widgets/imageviewer.h"

#include <qlayout.h>


DialogImageViewer::DialogImageViewer(QWidget* parent, KFileIconView* fileView) :
    KDialog(parent, "", false), m_imageViewer(new ImageViewer(this, fileView))
{
    setWFlags(Qt::WStyle_StaysOnTop | Qt::WNoAutoErase | Qt::WPaintClever);

    connect(m_imageViewer, SIGNAL(signalEnterPressed()), SLOT(slotToggleFullscreen()));
}


DialogImageViewer::~DialogImageViewer()
{
    delete m_imageViewer;
}

void DialogImageViewer::showImage(File* selectedImage)
{
    m_imageViewer->showImage(selectedImage);
    KDialog::show();
}

bool DialogImageViewer::close ( bool alsoDelete )
{
    m_imageViewer->free();
    return KDialog::close(alsoDelete);
}

void DialogImageViewer::resizeEvent ( QResizeEvent * e)
{
    m_imageViewer->resize(e->size());
}

void DialogImageViewer::updateImages()
{
    m_imageViewer->updateImageList();
}

void DialogImageViewer::mouseDoubleClickEvent ( QMouseEvent* /* e */ )
{
    slotToggleFullscreen();
}

void DialogImageViewer::slotToggleFullscreen()
{
    //switch fullscreen mode...
    if (isFullScreen()) {
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    } else {
        setWindowState(windowState() | Qt::WindowFullScreen);
    }
}


#include "dialogimageviewer.moc"
