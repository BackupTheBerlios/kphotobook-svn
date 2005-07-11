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
         * Sets the given date. The date is set only if the specified date is valid.
         * The time is left empty.
         */
        void setDate(QDate date);
        
        /**
         * Sets the given datetime. The datetime is set only if the specified datetime is valid.
         */
        void setDateTime(QDateTime dateTime);
        
        /**
         * Returns the currently choosen date. If the date is invalid or not set, an undefined date is returned.
         */
        QDate date()
        {
            return m_choosenDate ? QDate(*m_choosenDate) : QDate();
        }
        /**
         * Returns true if no date is set. The QDate returned by @link date() is undefined if no date is set.
         */
        bool noDateSet()
        {
            return !m_choosenDate;
        }
        
        /**
         * Returns the currently choosen time. If the time is invalid or not set, an undefined time.
         */
        QTime time()
        {
            return m_choosenTime ? QTime(*m_choosenTime) : QTime();
        }
        /**
         * Returns true if no time is set. The QTime returned by @link time() is undefined if no time is set.
         */
        bool noTimeSet()
        {
            return !m_choosenTime;
        }
        
        /**
         * Returns the currently choosen date and time. If the date or the time is invalid an invalid datetime is returned.
         */
        QDateTime dateTime()
        {
            if (isDateTimeValid()) {
                if (m_choosenDate && m_choosenTime) {
                    return QDateTime(*m_choosenDate, *m_choosenTime);
                }
                if (m_choosenDate) {
                    return QDateTime(*m_choosenDate);
                }
            }
            
            return QDateTime();
        }

        /**
         * Returns true if the resulting QDateTime would be valid.
         * - Both date and time must be valid.
         * - if the time is set, the date must also be set.
         */
        bool isDateTimeValid()
        {
            return (m_choosenDateIsValid && m_choosenTimeIsValid) && !(!m_choosenDate && m_choosenTime);
        }

    signals:
        /**
         * Is always emitted, when the date or time has changed. The bool value is true only if both date and time are valid.
         */
        void dateTimeValid(bool);
    
    public slots:
        /**
         * Removes the set date and time.
         */
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
        KLocale* m_locale;
        
        KLineEdit* m_dateLine;
        KLineEdit* m_timeLine;

        /**
         * Contains the actually entered date. If no date is entered this field is 0.
         */
        QDate* m_choosenDate;
        /**
         * Contains the info if the set date is valid. A date is valid if it is not set
         * or if it is set it must be a valid date.
         */
        bool m_choosenDateIsValid;
        
        /**
         * Contains the actually entered time. If no time is entered this field is 0.
         */
        QTime* m_choosenTime;
        /**
         * Contains the info if the set time is valid. A time is valid if it is not set
         * or if it is set it must be a valid time.
         */
        bool m_choosenTimeIsValid;
};


#endif
