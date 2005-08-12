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
#include "../utils/datetimefilterdata.h"
#include "../widgets/datetimewidget.h"
#include "../widgets/timeruler.h"

#include <kdatepicker.h>
#include <kdialogbase.h>
#include <ktabwidget.h>

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

    private:
        static Tracer* tracer;

    public:
        DialogDateTimeFilter(QWidget* parent, const char* name);
        DialogDateTimeFilter(QWidget* parent, const char* name, DateTimeFilterData* data);
        /*
        DialogDateTimeFilter(QWidget* parent, const char* name, QDateTime* singleDate);
        DialogDateTimeFilter(QWidget* parent, const char* name, QDateTime* fromDateTime, QDateTime* toDateTime);
        DialogDateTimeFilter(QWidget* parent, const char* name, QString pattern);
        */
        ~DialogDateTimeFilter();

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
        DateTimeFilterData* m_data;

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
