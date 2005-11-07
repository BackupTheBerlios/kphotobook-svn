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
#include <kiconloader.h>
#include <klocale.h>
#include <klineedit.h>
#include <kpushbutton.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qtooltip.h>


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
    QGridLayout* mainLayout = new QGridLayout(this, 2, 4, 0, 5, "mainLayout");

    // clear button
    QIconSet clearIcon = KGlobal::iconLoader()->loadIconSet("locationbar_erase", KIcon::Toolbar);
    KPushButton* clearButton = new KPushButton(clearIcon, QString::null, this, "clearButton");
    clearButton->setFlat(true);
    QToolTip::add(clearButton, i18n("Clears the enterd date and time"));
    mainLayout->addWidget(clearButton, 1, 0);
    connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClear()));

    // date label & date line
    QLabel* dateLabel = new QLabel(i18n("Date"), this, "dateLabel");
    mainLayout->addWidget(dateLabel, 0, 1);

    m_dateLine = new KLineEdit("", this);
    m_dateLine->setInputMask(getDateMask(m_locale->dateFormatShort()));
    mainLayout->addWidget(m_dateLine, 1, 1);
    connect(m_dateLine, SIGNAL(textChanged(const QString&)), this, SLOT(slotDateChanged(const QString&)));
    if (dateTime.isValid()) {
        m_dateLine->setText(m_locale->formatDate(dateTime.date(), true));
    }

    // date picker button
    QIconSet dateIcon = KGlobal::iconLoader()->loadIconSet("date", KIcon::Toolbar);
    KPushButton* datePickerButton = new KPushButton(dateIcon, QString::null, this, "datePickerButton");
    datePickerButton->setFlat(true);
    QToolTip::add(datePickerButton, i18n("Opens the datepicker"));
    mainLayout->addWidget(datePickerButton, 1, 2);
    connect(datePickerButton, SIGNAL(clicked()), this, SLOT(slotPickDate()));

    // time label & time line
    QLabel* timeLabel = new QLabel(i18n("Time"), this, "timeLabel");
    mainLayout->addWidget(timeLabel, 0, 3);

    m_timeLine = new KLineEdit("", this);
    m_timeLine->setInputMask(getTimeMask(m_locale->timeFormat()));
    mainLayout->addWidget(m_timeLine, 1, 3);
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


void DateTimeWidget::slotPickerDateChanged(QDate date)
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


#include "datetimewidget.moc"
