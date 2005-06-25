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

#include "dialogdatetimefilter.h"

#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <ktabwidget.h>

#include <qgroupbox.h>
#include <qiconset.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>


Tracer* DialogDateTimeFilter::tracer = Tracer::getInstance("kde.kphotobook.widgets", "DialogDateTimeFilter");


DialogDateTimeFilter::DialogDateTimeFilter(QWidget* parent, const char* name, QDateTime* currentFrom, QDateTime* currentTo)
    : KDialogBase(parent, name, true, i18n("Datetime filter"), KDialogBase::Default|KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true)
    , fromDateTime(0)
    , toDateTime(0)
    , noDateSet(0)
{
    tracer->invoked(__func__);
    
    this->setButtonText(KDialogBase::Default, i18n("Clear"));
    this->setButtonTip(KDialogBase::Default, i18n("Clears the entered date"));

    QWidget* mainPanel = new QWidget(this, "dirPanel");
    (new QVBoxLayout(mainPanel, 0, 5, "mainPanelLayout"))->setAutoAdd(true);

    KTabWidget* tab = new KTabWidget(mainPanel, "tabWidget");
    tab->addTab(buildRangePanel(currentFrom, currentTo), i18n("Range"));
    tab->addTab(buildRegExpPanel(), i18n("RegExp"));

    // no date set - checkbox
    noDateSet = new QCheckBox(i18n("No date set"), mainPanel);
    connect (noDateSet, SIGNAL(toggled(bool)), this, SLOT(slotNoDateSetToggled(bool)));
    
    // spacer
    QWidget* spacer = new QWidget(mainPanel, "spacer");
    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    
    this->setMainWidget(mainPanel);
}


DialogDateTimeFilter::~DialogDateTimeFilter()
{}


QWidget* DialogDateTimeFilter::buildRangePanel(QDateTime* currentFrom, QDateTime* currentTo)
{
    QWidget* rangePanel = new QWidget(this, "rangePanel");
    QVBoxLayout* rangePanelLayout = new QVBoxLayout(rangePanel, 0, 5, "rangePanelLayout");
    rangePanelLayout->setAutoAdd(true);
    rangePanelLayout->setMargin(10);

    // from datetime
    QGroupBox* fromDateTimeGroup = new QGroupBox(i18n("From"), rangePanel, "fromDateTimeGroup");
    fromDateTimeGroup->setColumns(1);
    fromDateTime = new DateTimeWidget(fromDateTimeGroup, (currentFrom != 0 && currentFrom->isValid()) ? *currentFrom : QDateTime());
    connect(this, SIGNAL(defaultClicked()), fromDateTime, SLOT(slotClear()));
    connect(fromDateTime, SIGNAL(dateTimeValid(bool)), this, SLOT(slotValidate()));

    // to datetime
    QGroupBox* toDateTimeGroup = new QGroupBox(i18n("To"), rangePanel, "toDateTimeGroup");
    toDateTimeGroup->setColumns(1);
    toDateTime = new DateTimeWidget(toDateTimeGroup, (currentFrom != 0 && currentTo->isValid()) ? *currentTo : QDateTime());
    connect(this, SIGNAL(defaultClicked()), toDateTime, SLOT(slotClear()));
    connect(toDateTime, SIGNAL(dateTimeValid(bool)), this, SLOT(slotValidate()));

    return rangePanel;
}


QWidget* DialogDateTimeFilter::buildRegExpPanel()
{
    QWidget* regExpPanel = new QWidget(this, "regExpPanel");
    QVBoxLayout* regExpPanelLayout = new QVBoxLayout(regExpPanel, 0, 5, "regExpPanelLayout");
    regExpPanelLayout->setAutoAdd(true);

    ///@todo implement
    
    return regExpPanel;
}



void DialogDateTimeFilter::slotValidate()
{
    enableButtonOK(fromDateTime->isDateTimeValid() && toDateTime->isDateTimeValid());
}


void DialogDateTimeFilter::slotNoDateSetToggled(bool checked)
{
    fromDateTime->setEnabled(!checked);
    toDateTime->setEnabled(!checked);

    if (checked) {
        enableButtonOK(true);
    } else {
        slotValidate();
    }
}


#include "dialogdatetimefilter.moc"
