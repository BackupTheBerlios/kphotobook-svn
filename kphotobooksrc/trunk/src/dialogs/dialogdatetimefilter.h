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

#include <kdialogbase.h>

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
        DialogDateTimeFilter(QWidget* parent = 0, const char* name = 0, QDateTime* currentFrom = 0, QDateTime* currentTo = 0);
        ~DialogDateTimeFilter();

        /**
         * Returns the choosen from datetime.
         * If null is returned and {@link isDateTimeFromValid} returns true, no from date is choosen.
         */
        QDateTime dateTimeFrom() {
            return fromDateTime->dateTime();
        }

        bool isDateTimeFromValid() {
            return toDateTime->isDateTimeValid();
        }

    private:
        QWidget* buildRangePanel(QDateTime* currentFrom, QDateTime* currentTo);
        QWidget* buildRegExpPanel();

    private slots:
        /**
         * Validates the entered data and enables/disables the ok button.
         */
        void slotValidate();

        /**
         * Is called everytime the 'no date set'-checkbox is clicked.
         */
        void slotNoDateSetToggled(bool checked);
    
    private:
        DateTimeWidget* fromDateTime;
        DateTimeWidget* toDateTime;
        QCheckBox* noDateSet;
};

#endif
