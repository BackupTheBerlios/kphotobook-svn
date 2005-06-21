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

#ifndef DATETIMEWIDGET_H
#define DATETIMEWIDGET_H

#include "../tracer/tracer.h"

#include <klineedit.h>
#include <klocale.h>

#include <qcolor.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qwidget.h>


class DateTimeWidget : public QWidget
{
    Q_OBJECT

    private:
        static Tracer* tracer;
    
    public:
        DateTimeWidget(QWidget* parent, QDateTime dateTime = QDateTime());
        virtual ~DateTimeWidget();

        /**
         * Returns the currently choosen date. If the date is invalid or not set, a null date is returned.
         */
        QDate date()
        {
            return choosenDate;
        }
        /**
         * Returns true if the choosenDate is valid. The QDate returned by {@link date()} can be null if no date is set.
         */
        bool isDateValid()
        {
            return choosenDateValid;
        }
        
        /**
         * Returns the currently choosen time. If the time is invalid or not set, a null time is returned.
         */
        QTime time()
        {
            return choosenTime;
        }
        /**
         * Returns true if the choosenTime is valid. The QTime returned by {@link time()} can be null if no time is set.
         */
        bool isTimeValid()
        {
            return choosenTimeValid;
        }
        
        /**
         * Returns the currently choosen date and time. If the date or the time is invalid a null datetime is returned.
         */
        QDateTime dateTime()
        {
            if (isDateTimeValid()) {
                if (!choosenDate.isNull() && !choosenTime.isNull()) {
                    return QDateTime(choosenDate, choosenTime);
                }
                if (!choosenDate.isNull()) {
                    return QDateTime(choosenDate);
                }
            }
            
            return QDateTime();
        }
        /**
         * Returns true if the resulting QDateTime would be valid:
         *   - both date and time must be valid
         *   - if time is not null date mustn't be null
         *   - time may be null if date is set
         *   - both date and time may be null
         */
        bool isDateTimeValid()
        {
            if (!choosenDateValid || !choosenTimeValid) {
                return false;
            }
            if (!choosenTime.isNull() && choosenDate.isNull()) {
                return false;
            }
            return true;
        }

    signals:
        /**
         * Is always emitted when the date or time has changed and both values are valid.
         */
        void dateTimeChanged(const QDateTime&);
        /**
         * Is always emitted, when the date or time has changed. The bool value is true only if both date and time are valid.
         */
        void dateTimeValid(bool);
    
    public slots:
        void slotClear();
        
    private:
        void initUI(QDateTime dateTime);
        /**
         * Returns the inputMask for the given dateFormat.
         * (replaces '%d' and '%m' with '99' and '%Y' with '9999').
         */
        QString getDateMask(QString dateFormat);
        /**
         * Returns the inputMask for the given timeFormat.
         * (replaces '%H', '%M' and '%S' with '99').
         */
        QString getTimeMask(QString timeFormat);
        /**
         * Returns true if the given date is empty. This is true for example
         * if the mask is '99.99.9999;_' and the string is '..'.
         */
        bool isValueEmpty(QString mask, QString value);

    private slots:
        void slotPickDate();
        void slotPickerDateChanged(QDate);
        void slotPickerOk();
        
        void slotDateChanged(const QString&);
        void slotTimeChanged(const QString&);
    
    private:
        KLocale* locale;
        
        KLineEdit* dateLine;
        KLineEdit* timeLine;

        QDate choosenDate;
        bool choosenDateValid;
        QTime choosenTime;
        bool choosenTimeValid;
};


#endif
