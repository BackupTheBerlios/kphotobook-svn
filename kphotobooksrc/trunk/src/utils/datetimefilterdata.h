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

#ifndef _DATETIMEFILTERDATA_H_
#define _DATETIMEFILTERDATA_H_

#include <qdatetime.h>
#include <qobject.h>
#include <qstring.h>


/**
 * This class holds the data used for filtering datetimes.
 *
 * CVS-ID $Id: constants.h 435 2005-08-02 20:36:17Z starcube $
 */
class DateTimeFilterData : public QObject
{
    Q_OBJECT

    friend class DialogDateTimeFilter;

    public:
        /**
         * The state of the datetime filter dialog. This state is returned by the method @link getState().
         */
        enum STATE {
            /**
             * If the dialog is in a invalid state.
             */
            INVALID = -1,
            /**
             * No filter is set. The engine must not filter by this date.
             */
            NO_FILTER_SET = 0,
            /**
             * Only the from date is set. The todate is not invalid but not set.
             * This means the engine should not restrict the todate. Use @link getValidFrom().
             */
            FROM_DATE_SET = 1,
            /** Only the to date is set. The fromdate is not invalid but not set.
             * This means the engine should not restrict the fromdate. Use @link getValidTo().
             */
            TO_DATE_SET = 2,
            /**
             * The from and to date are set.
             * Use @link getValidFrom() AND @link getValidTo().
             */
            FROM_TO_DATE_SET = 3,
            /**
             * A regular expression for matching dates is set.
             * Use @link getPattern();
             */
            PATTERN_DATE_SET = 4,
            /**
             * Only a single date is chosen. That means the engine must show images with the date matching this date.
             * ValidFrom and ValidTo are equal. Use @link getValidFrom() OR @link getValidTo().
             */
            SINGLE_DATE_SET = 8,
            /**
             * No date is set. The engine must show all images with this date not set.
             */
            NO_DATE_SET = 128
        };

    public:
        /**
         * Instantiates a new INVALID DateTimeFilterData.
         */
        DateTimeFilterData();
        /**
         * Instantiates this DateTimeFilterData with the given filterstring.
         * The datetimes in the string must be in ISO format.
         * @see marshal()
         */
        DateTimeFilterData(const QString& filterstring);
        ~DateTimeFilterData();

        /**
         * Sets the data stored according the given filterstring.
         * Emits a dataChanged signal.
         */
        void setFilterString(const QString& filterstring);

        /**
         * Returns the state of the datetime filter dialog.
         * @see STATE for details.
         */
        const STATE getState()
        {
            return m_state;
        }

        /**
         * Returns the chosen from datetime.
         * If the state (@see getState() and @see STATE) does not say the fromdate is set, it is not defined what is returned.
         */
        const QDateTime getDateTimeFrom()
        {
            return m_fromDateTime;
        }

        /**
         * Returns the chosen to datetime.
         * If the state (@see getState() and @see STATE) does not say the todate is set, it is not defined what is returned.
         */
        const QDateTime getDateTimeTo()
        {
            return m_toDateTime;
        }

        /**
         * Returns the regular expression a date must match.
         * If the state (@see getState() and @see STATE) does not say the pattern is set, it is not defined what is returned.
         */
        const QString getPattern()
        {
            return m_pattern;
        }

        /**
         * Returns a string representing this filter formatted to be shown to the user.
         * I.e. the dates are formatted for the current locale.
         */
        const QString toString();

        /**
         * Returns a string representing the current filter.
         * This string can be used to instantiate a new instance of this class.
         */
        const QString marshal();

    signals:
        void dataChanged();

    private:
        STATE m_state;

        QDateTime m_fromDateTime;
        QDateTime m_toDateTime;
        QString m_pattern;
};

#endif
