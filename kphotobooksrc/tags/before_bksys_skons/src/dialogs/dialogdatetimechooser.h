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

#ifndef DIALOGDATETIMECHOOSER_H
#define DIALOGDATETIMECHOOSER_H

class DateTimeWidget;
class Tracer;

#include <kdialogbase.h>

#include <qdatetime.h>


/**
 * Dialog for choosing a datetime.
 *
 * CVS-ID $Id: dialogaddsourcedir.h 274 2005-03-25 08:52:15Z choenig $
 */
class DialogDateTimeChooser : public KDialogBase
{
    Q_OBJECT
    
    private:
        static Tracer* tracer;
    
    public:
        DialogDateTimeChooser(QWidget* parent = 0, const char* name = 0, QDateTime dateTime = QDateTime());
        ~DialogDateTimeChooser();

        /**
         * Returns the chosen datetime.
         * If null is returned and {@link isDateTimeValid} returns true, no date is chosen.
         */
        QDateTime dateTime() const;

        bool noDateTimeSet() const;
    
    private:
        DateTimeWidget* dateTimeWidget;  
};

#endif
