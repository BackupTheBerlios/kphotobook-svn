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

#include "datetimewidget.h"

#include "../tracer/tracer.h"

#include <kdialogbase.h>
#include <kdatepicker.h>
#include <ktoolbarbutton.h>
#include <klocale.h>
#include <klineedit.h>

#include <qlayout.h>
#include <qlabel.h>


Tracer* DateTimeWidget::tracer = Tracer::getInstance("kde.kphotobook.widgets", "DateTimeWidget");


DateTimeWidget::DateTimeWidget(QWidget* parent, const QDateTime& dateTime) :
    QWidget(parent),
    m_locale(KGlobal::locale()),
    m_dateLine(0),
    m_timeLine(0),
    m_chosenDate(0),
    m_chosenDateIsValid(true),
    m_chosenTime(0),
    m_chosenTimeIsValid(true)
{
    initUI(dateTime);
}


DateTimeWidget::~DateTimeWidget()
{
    delete m_chosenDate;
    delete m_chosenTime;
}


void DateTimeWidget::setDate(const QDate& date)
{
    tracer->invoked(__func__);
    if (date.isValid()) {
        m_dateLine->setText(m_locale->formatDate(date, true));
    }
}


void DateTimeWidget::setDateTime(const QDateTime& dateTime)
{
    tracer->invoked(__func__);
    if (dateTime.isValid()) {
        m_dateLine->setText(m_locale->formatDate(dateTime.date(), true));
    }
    if (dateTime.isValid() && dateTime.time().isValid()) {
        m_timeLine->setText(m_locale->formatTime(dateTime.time(), true));
    }
}


void DateTimeWidget::initUI(const QDateTime& dateTime)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this, 0, 5, "mainLayout");
    mainLayout->setAutoAdd(true);

    // init Date & Time widgets
    QWidget* dateTimePanel = new QWidget(this, "dateTimePanel");
    QHBoxLayout* dateTimePanelLayout = new QHBoxLayout(dateTimePanel, 0, 5, "dateTimePanelLayout");
    dateTimePanelLayout->setAutoAdd(true);

    // clear button
    QWidget* clearPanel = new QWidget(dateTimePanel, "clearPanel");
    QVBoxLayout* clearPanelLayout = new QVBoxLayout(clearPanel, 0, 5, "clearPanelLayout");
    clearPanelLayout->setAutoAdd(true);

    new QLabel("", clearPanel, "spacerLabel1");
    KToolBarButton* clearButton = new KToolBarButton("locationbar_erase", 1, clearPanel, "clearButton");
    clearButton->setTextLabel(i18n("Clears the enterd date and time"), true);
    connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClear()));

    // date label & date line
    QWidget* datePanel = new QWidget(dateTimePanel, "datePanel");
    QVBoxLayout* datePanelLayout = new QVBoxLayout(datePanel, 0, 5, "datePanelLayout");
    datePanelLayout->setAutoAdd(true);

    new QLabel(i18n("Date"), datePanel, "dateLabel");
    m_dateLine = new KLineEdit("", datePanel);
    m_dateLine->setInputMask(getDateMask(m_locale->dateFormatShort()));
    connect(m_dateLine, SIGNAL(textChanged(const QString&)), this, SLOT(slotDateChanged(const QString&)));
    if (dateTime.isValid()) {
        m_dateLine->setText(m_locale->formatDate(dateTime.date(), true));
    }

    // date picker button
    QWidget* datePickerPanel = new QWidget(dateTimePanel, "datePickerPanel");
    QVBoxLayout* datePickerPanelLayout = new QVBoxLayout(datePickerPanel, 0, 5, "datePickerPanelLayout");
    datePickerPanelLayout->setAutoAdd(true);

    new QLabel("", datePickerPanel, "spacerLabel2");
    KToolBarButton* datePickerButton = new KToolBarButton("date", 1, datePickerPanel, "datePickerButton");
    datePickerButton->setTextLabel(i18n("Opens the datepicker"), true);
    connect(datePickerButton, SIGNAL(clicked()), this, SLOT(slotPickDate()));

    // time label & time line
    QWidget* timePanel = new QWidget(dateTimePanel, "timePanel");
    QVBoxLayout* timePanelLayout = new QVBoxLayout(timePanel, 0, 5, "timePanelLayout");
    timePanelLayout->setAutoAdd(true);

    new QLabel(i18n("Time"), timePanel, "timeLabel");
    m_timeLine = new KLineEdit("", timePanel);
    m_timeLine->setInputMask(getTimeMask(m_locale->timeFormat()));
    connect(m_timeLine, SIGNAL(textChanged(const QString&)), this, SLOT(slotTimeChanged(const QString&)));
    if (dateTime.isValid() && dateTime.time().isValid()) {
        m_timeLine->setText(m_locale->formatTime(dateTime.time(), true));
    }
}


QDateTime DateTimeWidget::dateTime() const
{
    if (isDateTimeValid()) {
        if (m_chosenDate && m_chosenTime) {
            return QDateTime(*m_chosenDate, *m_chosenTime);
        }
        if (m_chosenDate) {
            return QDateTime(*m_chosenDate);
        }
    }

    return QDateTime();
}


void DateTimeWidget::slotClear()
{
    tracer->sinvoked(__func__);
    m_dateLine->setText("");
    m_dateLine->setFocus();
    m_dateLine->setCursorPosition(0);
    tracer->debug(__func__, "middle");
    m_timeLine->setText("");
    m_dateLine->setCursorPosition(0);
    tracer->debug(__func__, "ended");
}


QString DateTimeWidget::getDateMask(const QString& dateFormat)
{
    QString s = dateFormat;
    s.replace("%d", "99");
    s.replace("%m", "99");
    s.replace("%Y", "9999");
    s.append(";_");

    return s;
}


QString DateTimeWidget::getTimeMask(const QString& timeFormat)
{
    QString s = timeFormat;
    s.replace("%H", "99");
    s.replace("%M", "99");
    s.replace("%S", "99");
    s.append(";_");

    return s;
}


bool DateTimeWidget::isValueEmpty(const QString& mask, const QString& value)
{
    QString s = mask;
    return s.replace("9", "") == (value + ";_");
}


void DateTimeWidget::slotPickDate()
{
    KDialogBase* dialog = new KDialogBase(this, "datePickerDialog", true, "Date picker", KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true);
    dialog->setEscapeButton(KDialogBase::Cancel);
    connect(dialog, SIGNAL(okClicked()), this, SLOT(slotPickerOk()));

    KDatePicker* datePicker = new KDatePicker(dialog, (m_chosenDate && !m_chosenDate->isNull()) ? *m_chosenDate : QDate::currentDate(), "datePicker", 0);
    connect(datePicker, SIGNAL(dateSelected(QDate)), this, SLOT(slotPickerDateChanged(QDate)));
    connect(datePicker, SIGNAL(dateEntered(QDate)), this, SLOT(slotPickerDateChanged(QDate)));

    dialog->setMainWidget(datePicker);
    dialog->show();
}


void DateTimeWidget::slotPickerDateChanged(const QDate& date)
{
    delete m_chosenDate;
    m_chosenDate = new QDate(date);
    m_chosenDateIsValid = true;
}


void DateTimeWidget::slotPickerOk()
{
    m_dateLine->setText(m_locale->formatDate(*m_chosenDate, true));
}


void DateTimeWidget::slotDateChanged(const QString& date)
{
    tracer->sinvoked(__func__) << "with date = '" <<  date << "', inputmask = '" << m_dateLine->inputMask() << "'" << endl;

    // test if the entered date matches the input mask
    if (isValueEmpty(m_dateLine->inputMask(), date)
        || m_dateLine->hasAcceptableInput()) {

        bool ok = false;
        QDate newDate = m_locale->readDate(date, &ok);
        if (isValueEmpty(m_dateLine->inputMask(), date)
            || ok) {
            tracer->sdebug(__func__) << "accepted." << endl;
            delete m_chosenDate;
            m_chosenDate = ok ? new QDate(newDate) : 0;
            m_chosenDateIsValid = true;
            m_dateLine->setBackgroundColor(QColor(255, 255, 255));

            emit(dateTimeValid(isDateTimeValid()));

            return;
        }
    }

    tracer->sdebug(__func__) << "NOT accepted." << endl;
    // the date is invalid
    m_chosenDateIsValid = false;
    m_dateLine->setBackgroundColor(QColor(255, 100, 100));

    emit(dateTimeValid(false));
}


void DateTimeWidget::slotTimeChanged(const QString& time)
{
    tracer->sinvoked(__func__) << "with time = '" <<  time << "', inputmask = '" << m_timeLine->inputMask() << "'" << endl;

    // test if the entered time matches the input mask
    if (isValueEmpty(m_timeLine->inputMask(), time)
        || m_timeLine->hasAcceptableInput()) {

        bool ok = false;
        QTime newTime = m_locale->readTime(time, &ok);
        if (isValueEmpty(m_timeLine->inputMask(), time)
            || ok) {
            tracer->sdebug(__func__) << "accepted." << endl;
            delete m_chosenTime;
            m_chosenTime = ok ? new QTime(newTime) : 0;
            m_chosenTimeIsValid = true;
            m_timeLine->setBackgroundColor(QColor(255, 255, 255));

            emit(dateTimeValid(isDateTimeValid()));

            return;
        }
    }

    tracer->sdebug(__func__) << "NOT accepted." << endl;
    // the time is invalid
    m_chosenTimeIsValid = false;
    m_timeLine->setBackgroundColor(QColor(255, 100, 100));

    emit(dateTimeValid(false));
}
