/***************************************************************************
 *   Copyright (C) 2003 by Michael Christen                                *
 *   starcube@my-mail.ch                                                   *
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

#include "dialogdatetimechooser.h"

#include "../widgets/datetimewidget.h"
#include "../tracer/tracer.h"

#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qiconset.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>


Tracer* DialogDateTimeChooser::tracer = Tracer::getInstance("kde.kphotobook.widgets", "DialogDateTimeChooser");


DialogDateTimeChooser::DialogDateTimeChooser(QWidget* parent, const char* name, QDateTime dateTime) :
    KDialogBase(parent, name, true, i18n("Datetime chooser"), KDialogBase::Default|KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true),
    dateTimeWidget(0)
{
    tracer->invoked(__func__);
    
    this->setButtonText(KDialogBase::Default, i18n("Clear"));
    this->setButtonTip(KDialogBase::Default, i18n("Clears the entered date"));
    
    QWidget* mainPanel = new QWidget(this, "dirPanel");
    QVBoxLayout* mainPanelLayout = new QVBoxLayout(mainPanel, 0, 5, "mainPanelLayout");
    mainPanelLayout->setAutoAdd(true);
    
    dateTimeWidget = new DateTimeWidget(mainPanel, dateTime);
    connect(this, SIGNAL(defaultClicked()), dateTimeWidget, SLOT(slotClear()));
    connect(dateTimeWidget, SIGNAL(dateTimeValid(bool)), this, SLOT(enableButtonOK(bool)));
    
    // spacer
    QWidget* spacer = new QWidget(mainPanel, "spacer");
    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    
    this->setMainWidget(mainPanel);
}


DialogDateTimeChooser::~DialogDateTimeChooser()
{
}


QDateTime DialogDateTimeChooser::dateTime() const {
    return dateTimeWidget->dateTime();
}


bool DialogDateTimeChooser::noDateTimeSet() const {
    return dateTimeWidget->noDateSet();
}


#include "dialogdatetimechooser.moc"
