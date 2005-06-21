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

#include <kdialogbase.h>
#include <kdatepicker.h>
#include <kglobal.h>
#include <ktoolbarbutton.h>

#include <qlayout.h>
#include <qlabel.h>


Tracer* DateTimeWidget::tracer = Tracer::getInstance("kde.kphotobook.widgets", "DateTimeWidget");


DateTimeWidget::DateTimeWidget(QWidget* parent, QDateTime dateTime)
        : QWidget(parent)
        , locale(KGlobal::locale())
        , dateLine(0)
        , timeLine(0)
        , choosenDate(QDate())
        , choosenDateValid(true)
        , choosenTime(QTime())
        , choosenTimeValid(true)
{
    initUI(dateTime);
}


DateTimeWidget::~DateTimeWidget()
{}


void DateTimeWidget::initUI(QDateTime dateTime)
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
    dateLine = new KLineEdit("", datePanel);
    dateLine->setInputMask(getDateMask(locale->dateFormatShort()));
    connect(dateLine, SIGNAL(textChanged(const QString&)), this, SLOT(slotDateChanged(const QString&)));
    if (dateTime.isValid()) {
        dateLine->setText(locale->formatDate(dateTime.date(), true));
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
    timeLine = new KLineEdit("", timePanel);
    timeLine->setInputMask(getTimeMask(locale->timeFormat()));
    connect(timeLine, SIGNAL(textChanged(const QString&)), this, SLOT(slotTimeChanged(const QString&)));
    if (dateTime.isValid() && dateTime.time().isValid()) {
        timeLine->setText(locale->formatTime(dateTime.time(), true));
    }
}


void DateTimeWidget::slotClear()
{
    dateLine->setText("");
    dateLine->setFocus();
    dateLine->setCursorPosition(0);
    timeLine->setText("");
    dateLine->setCursorPosition(0);
}


QString DateTimeWidget::getDateMask(QString dateFormat)
{
    dateFormat.replace("%d", "99");
    dateFormat.replace("%m", "99");
    dateFormat.replace("%Y", "9999");
    dateFormat.append(";_");

    return dateFormat;
}


QString DateTimeWidget::getTimeMask(QString timeFormat)
{
    timeFormat.replace("%H", "99");
    timeFormat.replace("%M", "99");
    timeFormat.replace("%S", "99");
    timeFormat.append(";_");

    return timeFormat;
}


bool DateTimeWidget::isValueEmpty(QString mask, QString value)
{
    return mask.replace("9", "") == (value + ";_");
}


void DateTimeWidget::slotPickDate()
{
    KDialogBase* dialog = new KDialogBase(this, "datePickerDialog", true, "Date picker", KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true);
    dialog->setEscapeButton(KDialogBase::Cancel);
    connect(dialog, SIGNAL(okClicked()), this, SLOT(slotPickerOk()));

    KDatePicker* datePicker = new KDatePicker(dialog, choosenDate.isNull() ? QDate::currentDate() : choosenDate, "datePicker", 0);
    connect(datePicker, SIGNAL(dateSelected(QDate)), this, SLOT(slotPickerDateChanged(QDate)));
    connect(datePicker, SIGNAL(dateEntered(QDate)), this, SLOT(slotPickerDateChanged(QDate)));

    dialog->setMainWidget(datePicker);
    dialog->show();
}


void DateTimeWidget::slotPickerDateChanged(QDate date)
{
    choosenDate = date;
    choosenDateValid = true;
}


void DateTimeWidget::slotPickerOk()
{
    dateLine->setText(locale->formatDate(choosenDate, true));
}


void DateTimeWidget::slotDateChanged(const QString& date)
{
    tracer->sinvoked(__func__) << "with date = '" <<  date << "', inputmask = '" << dateLine->inputMask() << "'" << endl;

    // test if the entered date matches the input mask
    if (isValueEmpty(dateLine->inputMask(), date)
        || dateLine->hasAcceptableInput()) {

        bool ok = false;
        choosenDate = locale->readDate(date, &ok);
        if (isValueEmpty(dateLine->inputMask(), date)
            || ok) {
            tracer->sdebug(__func__) << "accepted." << endl;
            choosenDateValid = true;
            dateLine->setBackgroundColor(QColor(255, 255, 255));

            // only emit ok signals if the time is valid too
            if (choosenTimeValid) {
                emit(dateTimeChanged(QDateTime(choosenDate, choosenTime)));
                emit(dateTimeValid(true));
            }

            return;
        }
    }

    tracer->sdebug(__func__) << "NOT accepted." << endl;
    choosenDate = QDate();
    choosenDateValid = false;
    dateLine->setBackgroundColor(QColor(255, 100, 100));

    emit(dateTimeValid(false));
}


void DateTimeWidget::slotTimeChanged(const QString& time)
{
    tracer->sinvoked(__func__) << "with time = '" <<  time << "', inputmask = '" << timeLine->inputMask() << "'" << endl;

    // test if the entered time matches the input mask
    if (isValueEmpty(timeLine->inputMask(), time)
        || timeLine->hasAcceptableInput()) {

        bool ok = false;
        choosenTime = locale->readTime(time, &ok);
        if (isValueEmpty(timeLine->inputMask(), time)
            || ok) {
            tracer->sdebug(__func__) << "accepted." << endl;
            choosenTimeValid = true;
            timeLine->setBackgroundColor(QColor(255, 255, 255));

            // only emit ok signals if the datetime would be valid
            if (isDateTimeValid()) {
                emit(dateTimeChanged(QDateTime(choosenDate, choosenTime)));
                emit(dateTimeValid(true));
            }

            return;
        }
    }

    tracer->sdebug(__func__) << "NOT accepted." << endl;
    choosenTime = QTime();
    choosenTimeValid = false;
    timeLine->setBackgroundColor(QColor(255, 100, 100));

    emit(dateTimeValid(false));
}
