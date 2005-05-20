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

#ifndef PTRRINGBUFFER_H
#define PTRRINGBUFFER_H


#include <qptrlist.h>



template <class T>
class PtrRingBuffer
{

public:
    PtrRingBuffer()    { m_data = QPtrList<T>(); };
    ~PtrRingBuffer()   { m_data.clear(); };


    /**
     * Sets the internal position marker to the first element
     *
     * @return the first item in this buffer
     */
    T*   first()       {
        return m_data.first();
    };


    /**
     * Sets the internal position marker to the last element
     *
     * @return the last item in this buffer
     */
    T*   last() {
        return m_data.last();
    };



    /**
     * Moves the internal pointer to the previous item in the buffer.
     * As this is a Ring, there is always a prev. element, as long as there
     * is an item in this list!
     *
     * @return the item before the current Item
     */
    T*   prev() {
        T* prev = m_data.prev();
        if (!prev)
            prev = last();
        return prev;
    };


    /**
     * @return the current item in this buffer
     */
    T*   current()     { return m_data.current(); };


    /**
     * Moves the internal pointer to the next item in the buffer.
     * As this is a Ring, there is always a next element, as long as there
     * is an item in this list!
     *
     * @return the item after the current Item
     */
    T*   next(){
        T* next = m_data.next();
        if (!next)
            next = first();
        return next;
    };


    /**
     * Does not change the internal pointer!
     *
     * @return the next item in this buffer
     */
    T*   getNext()  {
        //retrieve the next element
        T* nxt = next();
        //and then return to the current one!
        prev();
        return nxt;
    };

    /**
     * Does not change the internal pointer!
     *
     * @return the previous item in this buffer
     */
    T*   getPrev()  {
        //retrieve the next element
        T* prv = prev();
        //and then return to the current one.
        next();
        return prv;
    };

    /**
     * Appends data the object to this ring.
     * @param  data the object to be added
     */
    void append(const T* data) {
        m_data.append(data);
    };



    /**
     * removes all elements from this buffer
     */
    void clear() {
        m_data.clear();
    };

    /**
     * find data in this buffer. see QPtrList::find() for details
     */
    int find(const T* data) {
        return m_data.find(data);
    }

/**
     * find data in this buffer. see QPtrList::find() for details
     */
    int findRef(const T* data) {
        return m_data.findRef(data);
    }


    QString dump() {

        QString retval = "";
        T* data;
        for ( data = m_data.first(); data; data = m_data.next() ) {
            retval.sprintf("%s, %p", retval.ascii(), data);
        }
        return retval;
    }


private:
    QPtrList<T> m_data;

};




#endif
