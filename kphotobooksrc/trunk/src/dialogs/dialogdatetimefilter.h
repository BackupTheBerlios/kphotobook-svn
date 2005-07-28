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

#ifndef DIALOGDATETIMEFILTER_H
#define DIALOGDATETIMEFILTER_H

#include "../tracer/tracer.h"
#include "../widgets/datetimewidget.h"
#include "../widgets/timeruler.h"

#include <kdialogbase.h>
#include <ktabwidget.h>
#include <kdatepicker.h>

#include <qcheckbox.h>
#include <qdatetime.h>


/**
 * Dialog for setting the datetime filter.
 *
 * CVS-ID $Id: dialogaddsourcedir.h 274 2005-03-25 08:52:15Z choenig $
 */
class DialogDateTimeFilter : public KDialogBase
{
    Q_OBJECT

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
            NO_DATE_SET = 256
        };

    private:
        static Tracer* tracer;

    public:
        DialogDateTimeFilter(QWidget* parent, const char* name);
        DialogDateTimeFilter(QWidget* parent, const char* name, bool noDateSet);
        DialogDateTimeFilter(QWidget* parent, const char* name, QDateTime* singleDate);
        DialogDateTimeFilter(QWidget* parent, const char* name, QDateTime* fromDateTime, QDateTime* toDateTime);
        DialogDateTimeFilter(QWidget* parent, const char* name, QString pattern);
        ~DialogDateTimeFilter();

        /**
         * Returns the state of the datetime filter dialog.
         * @see STATE for details.
         */
        STATE getState()
        {
            return m_state;
        }
        
        /**
         * Returns the chosen from datetime.
         * If the state (@see getState() and @see STATE) does not say the fromdate is set, it is not defined what is returned.
         */
        QDateTime getDateTimeFrom();
        
        /**
         * Returns the chosen to datetime.
         * If the state (@see getState() and @see STATE) does not say the todate is set, it is not defined what is returned.
         */
        QDateTime getDateTimeTo();

        /**
         * Returns the regular expression a date must match.
         * If the state (@see getState() and @see STATE) does not say the pattern is set, it is not defined what is returned.
         */
        QString getPattern();


    private:
        void initUI();
        QWidget* buildSinglePanel();
        QWidget* buildRangePanel();
        QWidget* buildPatternPanel();

    private slots:
        /**
         * Is called when the clear button is pressed.
         * Clears all input on all tabs and sets the state to NO_FILTER_SET.
         */
        void slotClear();

        /**
         * Validates the entered data and enables/disables the ok button.
         */
        void slotValidate();

        /**
         * Is called everytime the 'no date set'-checkbox is clicked.
         */
        void slotNoDateSetToggled(bool checked);

        /**
         * Is called, when the selection of the timeruler is changed
         */
        void slotDateSelectionChanged(int, int);

    private:
        STATE m_state;
        
        KTabWidget* m_tabWidget;

        QWidget* m_rangePanel;
        QWidget* m_patternPanel;
        QWidget* m_singlePanel;

        TimeRuler* m_timeRuler;
        KDatePicker* m_dateTable;

        DateTimeWidget* m_fromDateTime;
        DateTimeWidget* m_toDateTime;

        QCheckBox* m_noDateSet;
};

#endif
