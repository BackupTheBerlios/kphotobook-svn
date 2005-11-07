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

#include "confighelper.h"

#include "../tracer/tracer.h"

#include <qintdict.h>
#include <qstringlist.h>


Tracer* ConfigHelper::tracer = Tracer::getInstance("kde.kphotobook.utils", "ConfigHelper");


QStringList* ConfigHelper::intDict2stringList(QIntDict<QString>* intDict)
{
    QStringList* stringList = new QStringList();

    QIntDictIterator<QString> it(*intDict);
    for (; it.current(); ++it) {
        QString entry = QString("%1:%2").arg(it.currentKey()).arg(*it.current());
        stringList->append(entry);
    }

    return stringList;
}


QIntDict<QString>* ConfigHelper::stringList2intDict(QStringList stringList)
{
    QIntDict<QString>* filterDict = new QIntDict<QString>;

    // loop over all entries in the stringlist
    for (QStringList::Iterator it = stringList.begin(); it != stringList.end(); ++it) {

        tracer->sdebug(__func__) << "Handling entry: '" << *it << "'" << endl;

        // split the current entry into key and value and put them into the intdict
        int delimitorPos = (*it).find(':');
        if (delimitorPos > 0) {
            QString keyStr = (*it).mid(0, delimitorPos);
            QString value = (*it).mid(delimitorPos + 1);

            tracer->sdebug(__func__) << "key-->value: '" << keyStr << "-->" << value << "'" << endl;

            bool ok;
            int key = keyStr.toInt(&ok);

            if (ok) {
                filterDict->insert(key, new QString(value));
            } else {
                tracer->swarning(__func__) << "Key '" << *it << "' is invalid! It is not a number. (Valid format: 'key:value')" << endl;
            }

        } else {
            tracer->swarning(__func__) << "KeyValue pair '" << *it << "' is invalid! (Valid format: 'key:value')" << endl;
        }
    }

    return filterDict;
}


