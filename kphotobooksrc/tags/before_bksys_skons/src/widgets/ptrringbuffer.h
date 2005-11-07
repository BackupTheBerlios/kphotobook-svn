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
#include <qstring.h>


template <class T> class PtrRingBuffer
{
    public:
        PtrRingBuffer()
        {
            m_data = QPtrList<T>();
        };

        ~PtrRingBuffer()
        {
            m_data.clear();
        };
    
        /**
         * returns the index of the previous element in this ring.
         * As this is a RING, there always is a previous id
         * @return the index of the previous item
         */
        int prvIdx()
        {
            int i = curIdx() - 1;
            return (i < 0  ? count()-1 : i);
        }
    
        /**
         * @return the index of the current element in this ring
         *
         */
        int curIdx()
        {
            return m_data.at();
        }
    
        /**
         * returns the index of the next element in this ring.
         * if the current index is the last item in the ring, then 0 is
         * returned
         * @return the index of the next item
         */
        int nxtIdx()
        {
            int i = curIdx() + 1;
            return (i >= count() ? 0 : i);
        }
    
        /**
         * @return the count of elements in this ring
         */
        int count()
        {
            return m_data.count();
        }
    
        /**
         * Sets the internal position marker to the first element
         *
         * @return the first item in this buffer
         */
        T* first()
        {
            return m_data.first();
        };
    
        /**
         * Sets the internal position marker to the last element
         *
         * @return the last item in this buffer
         */
        T* last()
        {
            return m_data.last();
        };
    
        /**
         * Moves the internal pointer to the previous item in the buffer.
         * As this is a Ring, there is always a prev. element, as long as there
         * is an item in this list!
         *
         * @return the item before the current Item
         */
        T* prev()
        {
            return m_data.at(prvIdx());
    /*        T* prev = m_data.prev();
            if (!prev)
                prev = last();
            return prev;*/
        };
    
        /**
         * @return the current item in this buffer
         */
        T* current()
        {
            return m_data.current();
        };
    
        /**
         * Moves the internal pointer to the next item in the buffer.
         * As this is a Ring, there is always a next element, as long as there
         * is an item in this list!
         *
         * @return the item after the current Item
         */
        T* next()
        {
            return m_data.at(nxtIdx());
    /*        T* next = m_data.next();
            if (!next)
                next = first();
            return next;*/
        };
    
        /**
         * Does not change the internal pointer!
         *
         * @return the next item in this buffer
         */
        T* getNext()
        {
            int i = curIdx();
            //retrieve the next element
            T* nxt = m_data.at(nxtIdx());
            //and then return to the current one!
            m_data.at(i);
    
            return nxt;
        };
    
        /**
         * Does not change the internal pointer!
         *
         * @return the previous item in this buffer
         */
        T* getPrev()
        {
            int i = curIdx();
            //retrieve the next element
            T* prv = m_data.at(prvIdx());
            //and then return to the current one.
            m_data.at(i);
            return prv;
        };
    
        /**
         * Appends data the object to this ring.
         * @param  data the object to be added
         */
        void append(const T* data)
        {
            m_data.append(data);
        };
    
        /**
         * removes all elements from this buffer
         */
        void clear()
        {
            m_data.clear();
        };
    
        /**
         * find data in this buffer. see QPtrList::find() for details
         */
        int find(const T* data)
        {
            return m_data.find(data);
        }
    
        /**
         * find data in this buffer. see QPtrList::find() for details
         */
        int findRef(const T* data)
        {
            return m_data.findRef(data);
        }
    
        QString dump()
        {
            QString retval = "";
    
            T* current = m_data.current();
    
            QPtrListIterator<T> it(m_data);
            T* o;
            while (( o = it.current()) != 0) {
                if (o == current) {
                    retval.sprintf("%s, [%p]", retval.ascii(), o);
                } else {
                    retval.sprintf("%s, %p", retval.ascii(), o);
                }
                ++it;
            }
            return retval;
        }
    
    private:
        QPtrList<T> m_data;
};


#endif
