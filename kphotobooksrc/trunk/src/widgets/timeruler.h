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

class Tracer;

#include <qpixmap.h>
#include <qwidget.h>

#include <qdatetime.h>
class QTimer;

#include <list>
#include <vector>


using namespace std;

/** this class represents an advanced list of dates */
class ListOfDates
{
    public:
        ListOfDates();
        ~ListOfDates();
    
        /**
        * adds the QDate d to this list of dates
        */
        void addDate(const QDate& d);
    
        /**
        * @returns the number of images which mach the given year, year/month, year/month/day combo
        * @param contd if true, the iterator will not be reset to the beginning. This
        * is usefull for continued counting of dates within a loop or so. Be careful, as
        * the data is sorted from low to high, dates could be missed, if they're lower then
        * the dates counted before. calling count(-1) only sets the iterator to the beginning
        * and does nothing else
        */
        int count (int year, int month = -1, int day = -1, bool cntd = false);
    
        /**
        * @returns the minimal year
        */
        int firstYear();
        /**
        * @returns the maximal year in this list
        */
        int lastYear();
        /**
        * @returns the number of years covered by this list
        * (that is lastYear() - firstYear() + 1
        */
        int numYears();
    
	/** @returns the year of the latest date in this container */
        int maxYear();
	/** @returns the month of the latest date in this container */
        int maxMonth();
	/** @returns the day of the latest date in this container */
        int maxDay();
    
    private:
        list<QDate> m_lstData;
    
        int m_maxYear;
        int m_maxMonth;
        int m_maxDay;
};


class TimeRuler : public QWidget
{
    Q_OBJECT
    
    private:
        static Tracer* tracer;
    
    public:
        TimeRuler(QWidget* parent = 0);
        virtual ~TimeRuler();
    
    public slots:
        void slotSetSelected(const QDate& date, bool center = true);
    
        /**
        * sets the selected date to year/month.
        * if signalIt is true, a selectionChanged signal is emitted
        */
        void setSelected(int year, int month, bool signalIt = false);
    
        /**
        * if year/month isContained, it is centerd in the widget
        */
        void setCentered(int year, int month);
    
        /**
        * @returns true, if year/month is available in this widget
        */
        bool isContained(int year, int month);
    
    signals:
        void selectionChanged(int year, int month);
    
    protected:
        virtual void resizeEvent ( QResizeEvent * );
        virtual void paintEvent ( QPaintEvent * );
        virtual void mouseMoveEvent ( QMouseEvent * e );
        virtual void mousePressEvent ( QMouseEvent * e );
        virtual void mouseReleaseEvent ( QMouseEvent * e );
        virtual void wheelEvent ( QWheelEvent * e );
    
    private slots:
        void slotScrollTimerFired();
    
    private:
        /**
        * initializes the userinterface
        */
        void initUI();
    
        /**
        * initializes the data to be shown in this widget
        */
        void initData();
    
        /**
        * sets year/month to the date referenced at the position pos
        */
        void mousePosToOffset(const QPoint& pos, int* year, int* month);
    
        /**
        * @returns the rectangle to draw the image count bar.
        * @param translateX if true, the rectangle is translated to the current
        * screen coordinates depending on m_xVis
        * @param height this is the height of the returned rectangle. if < 0, a rectangle from
        * y_0=0 to y_1=m_yBase is returned. this is usefull for redrawing calculations
        */
        QRect dateToBarRect(int year, int month, bool translateX, int height = -1);
    
    
        void smoothShift(int x);
        /**
        * shifts the TimeRuler by x pixels to the right, if x>0 and left, if x<0
        */
        bool shift(int x);
    
        /**
        * draws a single beam
        * @param p to this painter
        * @param percent this is the size of the beam in percent
        * @param year the year to paint
        * @param month the month do draw
        * @param fill if true, the beam will be filled with color fillColor
        * @param fillColor the color to fill the beam if desired
        */
        void drawBeam(QPainter* p, int percent, int year, int month, bool fill = false, QColor fillColor = Qt::black);
    
        /// the width of a single month
        int m_widthMonth;
        /// the height of this widget
        int m_height;
        /// the baseline of this widget. Bars are drawn above, text below.
        int m_yBase;
    
        /// this is the amount, the TimeRuler pixmap is offset
        int m_xVis;
        /// the year, the mouse is currently over
        int m_mouseOverYear;
        /// the month, the mouse is currently over
        int m_mouseOverMonth;
    
        /// the year, that is currently selected
        int m_selectedYear;
        /// the month, that is currently selected
        int m_selectedMonth;
    
        /// the location, the mousepress has happened at. tis is used to distinguish a click from a move
        QPoint m_mousePressPosition;
    
        /// the list of dates available in this album
        ListOfDates  m_lstDates;
    
        /// the basic pixmap the timeruler is made of
        QPixmap m_pixmap;
    
        ///timer used for smoothScrolling
        QTimer* m_scrollTimer;
        /// scroll amount, that is left in smoothScrolling
        int m_scrollLeft;
};











#endif //TIMERULER_H
