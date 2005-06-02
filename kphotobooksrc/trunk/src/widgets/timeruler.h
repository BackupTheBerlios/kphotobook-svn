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

#ifndef TIMERULER_H
#define TIMERULER_H

#include "../tracer/tracer.h"

#include <qpixmap.h>
#include <qwidget.h>

#include <vector>
#include <list>

using namespace std;



class DateBinder {

public:
    DateBinder();
    ~DateBinder();

    void addDate(QDate d);

    int count (int year = -1, int month = -1, int day = -1);

    int minYear();
    int maxYear();
    int numYears();

private:
    list<QDate> m_lstData;
};



/*

class YearMonth {
public:
    YearMonth(int year, int month) {
        m_year = year;
        m_month = month;
    };

    YearMonth(int sum) {
        if (sum >= 0) {
            m_year  = sum / 12;
            m_month = sum % 12;
        } else {
            m_year  = -1;
            m_month = -1;
        }
    }

    int year()  { return m_year;  };
    int month() { return m_month; };

    int sum()   { return m_year * 12 + m_month; };

    bool isValid() {
        return (m_year > 0 && m_month > 0);
    }

private:
    int m_year;
    int m_month;

};*/






class TimeRuler : public QWidget
{
    Q_OBJECT

private:
    static Tracer* tracer;


public:
    TimeRuler(QWidget* parent);
    virtual ~TimeRuler();


protected:
    virtual void resizeEvent ( QResizeEvent * );
    virtual void paintEvent ( QPaintEvent * );
    virtual void mouseMoveEvent ( QMouseEvent * e );
    virtual void wheelEvent ( QWheelEvent * e );

private:
    void initUI();
    void initData();

    void mousePosToOffset(QPoint pos, int* year, int* month);
    QRect dateToBoundingRect(int year, int month, int size = -1);

    void shift(int x);

    void drawBeam(QPainter* p, int percent, int year, int month, bool fill = false, QColor fillColor = Qt::black);

    int m_mWidth;
    int m_height;
    int m_yBase;

    int m_xVis;
    int m_mouseOverYear;
    int m_mouseOverMonth;

    DateBinder  m_lstDates;

    QPixmap m_pixmap;
};











#endif //TIMERULER_H
