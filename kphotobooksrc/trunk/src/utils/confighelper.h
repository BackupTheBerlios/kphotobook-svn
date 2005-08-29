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

#ifndef _CONFIGHELPER_H_
#define _CONFIGHELPER_H_

class Tracer;

class QString;
class QStringList;
template<class type> class QIntDict;

/**
 * This class helps converting a dictionary to a stringlist and vice
 * versa to store the data in a config file.
 *
 * CVS-ID $Id: constants.h 435 2005-08-02 20:36:17Z starcube $
 */
class ConfigHelper
{
    private:
        static Tracer* tracer;
    
    public:
        /**
        * Builds a list of strings containing as many entries as the specified
        * int dictionary. Each entry of the dictionary is joined into one string.
        * The string is built by the int key followed by a colon and the value.
        * This helper method is used because the class KConfig or KConfigBase resp.
        * do not support writing/reading int dictionaries.
        */
        static QStringList* intDict2stringList(QIntDict<QString>* intDict);

        /**
         * Builds a int dictionary with the entries found in the specified stringlist.
         * The entries in the stringlist must begin with an int followed by a colon and
         * an alphanumeric string: <int>:<alphanumeric>.
         * This helper method is used because the class KConfig or KConfigBase resp.
         * do not support writing/reading int dictionaries.
         */
        static QIntDict<QString>* stringList2intDict(QStringList stringList);

};

#endif
