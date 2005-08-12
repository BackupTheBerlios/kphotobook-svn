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
//#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qgroupbox.h>
#include <qiconset.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>


Tracer* DialogDateTimeFilter::tracer = Tracer::getInstance("kde.kphotobook.widgets", "DialogDateTimeFilter");


DialogDateTimeFilter::DialogDateTimeFilter(QWidget* parent, const char* name) :
    KDialogBase(parent, name, true, i18n("Datetime filter"), KDialogBase::Default|KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true),
    m_data(new DateTimeFilterData()),
    m_tabWidget(0),
    m_timeRuler(0),
    m_dateTable(0),
    m_fromDateTime(0),
    m_toDateTime(0)
{
    tracer->invoked(__func__, "no parameter");

    initUI();
    m_tabWidget->showPage(m_rangePanel);
}


DialogDateTimeFilter::DialogDateTimeFilter(QWidget* parent, const char* name, DateTimeFilterData* data) :
    KDialogBase(parent, name, true, i18n("Datetime filter"), KDialogBase::Default|KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true),
    m_data(data),
    m_tabWidget(0),
    m_timeRuler(0),
    m_dateTable(0),
    m_fromDateTime(0),
    m_toDateTime(0)
{
    tracer->invoked(__func__, "no date set");

    initUI();
    m_tabWidget->showPage(m_rangePanel);

//    m_noDateSet->setChecked(noDateSet);
}

/*
DialogDateTimeFilter::DialogDateTimeFilter(QWidget* parent, const char* name, QDateTime* singleDate) :
    KDialogBase(parent, name, true, i18n("Datetime filter"), KDialogBase::Default|KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true),
    m_state(INVALID),
    m_tabWidget(0),
    m_timeRuler(0),
    m_dateTable(0),
    m_fromDateTime(0),
    m_toDateTime(0),
    m_noDateSet(0)
{
    tracer->invoked(__func__, "single date");

    initUI();

    m_tabWidget->showPage(m_singlePanel);

    m_timeRuler->slotSetSelected((singleDate && singleDate->date().isValid()) ? singleDate->date() : QDate::currentDate(), true);
}


DialogDateTimeFilter::DialogDateTimeFilter(QWidget* parent, const char* name, QDateTime* fromDateTime, QDateTime* toDateTime) :
    KDialogBase(parent, name, true, i18n("Datetime filter"), KDialogBase::Default|KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true),
    m_state(INVALID),
    m_tabWidget(0),
    m_timeRuler(0),
    m_dateTable(0),
    m_fromDateTime(0),
    m_toDateTime(0),
    m_noDateSet(0)
{
    tracer->invoked(__func__, "from/to date");

    initUI();

    m_tabWidget->showPage(m_rangePanel);

    // set the from datetime
    static QTime MINIMUM_TIME = QTime(00, 00, 00);
    if (fromDateTime && fromDateTime->isValid()) {
        if (fromDateTime->time().isValid() && fromDateTime->time() != MINIMUM_TIME) {
            m_fromDateTime->setDateTime(*fromDateTime);
        } else {
            m_fromDateTime->setDate(fromDateTime->date());
        }
    }

    // set the to datetime
    static QTime MAXIMUM_TIME = QTime(23, 59, 59);
    if (toDateTime && toDateTime->isValid()) {
        if (toDateTime->time().isValid() && toDateTime->time() != MAXIMUM_TIME) {
            m_toDateTime->setDateTime(*toDateTime);
        } else {
            m_toDateTime->setDate(toDateTime->date());
        }
    }
}


DialogDateTimeFilter::DialogDateTimeFilter(QWidget* parent, const char* name, QString pattern) :
    KDialogBase(parent, name, true, i18n("Datetime filter"), KDialogBase::Default|KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true),
    m_state(INVALID),
    m_tabWidget(0),
    m_timeRuler(0),
    m_dateTable(0),
    m_fromDateTime(0),
    m_toDateTime(0),
    m_noDateSet(0)
{
    tracer->invoked(__func__, "pattern");

    initUI();

    ///@todo implement pattern thingys
}

*/

DialogDateTimeFilter::~DialogDateTimeFilter()
{
}

/*
QDateTime DialogDateTimeFilter::getDateTimeFrom()
{
    if (m_tabWidget->currentPage() == m_rangePanel) {
        return m_fromDateTime->dateTime();
    } else if (m_tabWidget->currentPage() == m_singlePanel) {
        ///@todo return the timerulers date.
    }

    return QDate();
}


QDateTime DialogDateTimeFilter::getDateTimeTo()
{
    if (m_tabWidget->currentPage() == m_rangePanel) {
        if (m_toDateTime->noTimeSet()) {
            return QDateTime(m_toDateTime->dateTime().date(), QTime(23, 59, 59));
        } else {
            return m_toDateTime->dateTime();
        }
    } else if (m_tabWidget->currentPage() == m_singlePanel){
        ///@todo return the timerulers date.
    }

    return QDate();
}


QString DialogDateTimeFilter::getPattern()
{
    ///@todo implement: return the date regular expression
    return QString::null;
}
*/


void DialogDateTimeFilter::initUI()
{
    tracer->invoked(__func__);

    this->setButtonText(KDialogBase::Default, i18n("Clear"));
    this->setButtonTip(KDialogBase::Default, i18n("Removes the filter."));
    connect(this, SIGNAL(defaultClicked()), this, SLOT(slotClear()));

    QWidget* mainPanel = new QWidget(this, "mainPanel");
    (new QVBoxLayout(mainPanel, 0, 5, "mainPanelLayout"))->setAutoAdd(true);

    m_tabWidget = new KTabWidget(mainPanel, "tabWidget");
    m_tabWidget->addTab(buildSinglePanel(), i18n("Single"));
    m_tabWidget->addTab(buildRangePanel(), i18n("Range"));
    m_tabWidget->addTab(buildPatternPanel(), i18n("Pattern"));
    connect(m_tabWidget, SIGNAL(currentChanged(QWidget*)), this, SLOT(slotValidate()));

    // no date set - checkbox
    m_noDateSet = new QCheckBox(i18n("No date set"), mainPanel);
    connect (m_noDateSet, SIGNAL(toggled(bool)), this, SLOT(slotNoDateSetToggled(bool)));

    // spacer
    QWidget* spacer = new QWidget(mainPanel, "spacer");
    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    ///@todo remove disabling singleDate and pattern as soon as implemented
    m_singlePanel->setEnabled(false);
    m_patternPanel->setEnabled(false);

    this->setMainWidget(mainPanel);
}


QWidget* DialogDateTimeFilter::buildSinglePanel()
{
    m_singlePanel = new QWidget(this, "singlePanel");
    QVBoxLayout* singlePanelLayout = new QVBoxLayout(m_singlePanel, 5, 5, "singlePanelLayout");
    singlePanelLayout->setAutoAdd(true);

    m_timeRuler = new TimeRuler(m_singlePanel);
    connect(m_timeRuler, SIGNAL(selectionChanged(int, int)), this, SLOT(slotDateSelectionChanged(int, int)));
    m_dateTable = new KDatePicker(m_singlePanel);
    connect(m_dateTable, SIGNAL(dateChanged(QDate)), m_timeRuler, SLOT(slotSetSelected(const QDate&, bool)));

    return m_singlePanel;
}


QWidget* DialogDateTimeFilter::buildRangePanel()
{
    m_rangePanel = new QWidget(this, "rangePanel");
    QVBoxLayout* rangePanelLayout = new QVBoxLayout(m_rangePanel, 0, 5, "rangePanelLayout");
    rangePanelLayout->setAutoAdd(true);
    rangePanelLayout->setMargin(10);

    // from datetime
    QGroupBox* m_fromDateTimeGroup = new QGroupBox(i18n("From"), m_rangePanel, "m_fromDateTimeGroup");
    m_fromDateTimeGroup->setColumns(1);
    m_fromDateTime = new DateTimeWidget(m_fromDateTimeGroup);
    connect(m_fromDateTime, SIGNAL(dateTimeValid(bool)), this, SLOT(slotValidate()));

    // to datetime
    QGroupBox* m_toDateTimeGroup = new QGroupBox(i18n("To"), m_rangePanel, "m_toDateTimeGroup");
    m_toDateTimeGroup->setColumns(1);
    m_toDateTime = new DateTimeWidget(m_toDateTimeGroup);
    connect(m_toDateTime, SIGNAL(dateTimeValid(bool)), this, SLOT(slotValidate()));

    return m_rangePanel;
}


QWidget* DialogDateTimeFilter::buildPatternPanel()
{
    m_patternPanel = new QWidget(this, "patternPanel");
    QVBoxLayout* patternPanelLayout = new QVBoxLayout(m_patternPanel, 0, 5, "patternPanelLayout");
    patternPanelLayout->setAutoAdd(true);

    ///@todo implement

    return m_patternPanel;
}


void DialogDateTimeFilter::slotClear()
{
    m_data->m_state = DateTimeFilterData::NO_FILTER_SET;
    slotOk();
}


void DialogDateTimeFilter::slotValidate()
{
    if (m_tabWidget->currentPage() == m_rangePanel) {
        if (m_fromDateTime->isDateTimeValid() && m_toDateTime->isDateTimeValid()) {
            // date time are valid --> determine state
            enableButtonOK(true);
            if (!m_fromDateTime->noDateSet() && !m_toDateTime->noDateSet()) {
                // both from and to date are set
                m_data->m_state = DateTimeFilterData::FROM_TO_DATE_SET;
            } else if (!m_fromDateTime->noDateSet()) {
                // from date is set only
                m_data->m_state = DateTimeFilterData::FROM_DATE_SET;
            } else if (!m_toDateTime->noDateSet()) {
                // from date is set only
                m_data->m_state = DateTimeFilterData::TO_DATE_SET;
            } else {
                // neither from nor to date is set --> no filter set
                m_data->m_state = DateTimeFilterData::NO_FILTER_SET;
            }
        } else {
            // datetime are not valid
            enableButtonOK(false);
            m_data->m_state = DateTimeFilterData::INVALID;
        }
    } else if (m_tabWidget->currentPage() == m_patternPanel) {
        enableButtonOK(true);
        m_data->m_state = DateTimeFilterData::PATTERN_DATE_SET;
    } else if (m_tabWidget->currentPage() == m_singlePanel){
        enableButtonOK(true);
        m_data->m_state = DateTimeFilterData::SINGLE_DATE_SET;
    }
}


void DialogDateTimeFilter::slotNoDateSetToggled(bool checked)
{
    if (checked) {
        m_tabWidget->setEnabled(false);
        enableButtonOK(true);
        m_data->m_state = DateTimeFilterData::NO_DATE_SET;
    } else {
        m_tabWidget->setEnabled(true);
        slotValidate();
    }
}


void DialogDateTimeFilter::slotDateSelectionChanged(int year, int month)
{
    m_dateTable->setDate(QDate(year, month, 1));
}

#include "dialogdatetimefilter.moc"
