/***************************************************************************
 *   Copyright (C) 5005 by Christian Hoenig                                *
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


#ifndef DIALOGIMAGEVIEWER_H
#define DIALOGIMAGEVIEWER_H

#include "imageviewer.h"
#include "../engine/file.h"

#include <kdialog.h>
#include <kfileiconview.h>

class DialogImageViewer : public KDialog
{
Q_OBJECT

public:
    DialogImageViewer(QWidget* parent, KFileIconView* fileView);
    ~DialogImageViewer();

    void updateImages();

    void        mouseDoubleClickEvent ( QMouseEvent * e );
//  void        contextMenuEvent ( QContextMenuEvent * e );
//  void        wheelEvent ( QWheelEvent  * e );

    virtual void show (File* selectedImage);

    virtual bool close ( bool alsoDelete );

public slots:
    void    slotToggleFullscreen();

protected:
    virtual void resizeEvent ( QResizeEvent * e);



private:
    ImageViewer* m_imageViewer;

};

#endif
