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

#include "timeruler.h"

#include "../settings/settings.h"

#include <qpainter.h>
#include <qstyle.h>

#include <kapplication.h>

#include <list>
#include <math.h>

using namespace std;

Tracer* TimeRuler::tracer = Tracer::getInstance("kde.kphotobook.widgets", "TimeRuler");

TimeRuler::TimeRuler(QWidget* parent)
: QWidget(parent)
{
    //the basic width of a month
    m_widthMonth = 15;
    // the total height of this widgets
    m_height = 75;
    // the hight of the baseline
    m_yBase  = 40;

    // the year/month, the mouse is currently over
    m_mouseOverYear = -1;
    m_mouseOverMonth = -1;

    //the year/month, that is currently selected
    m_selectedYear = -1;
    m_selectedMonth = -1;

    initData();

    initUI();

    setMinimumWidth( 200 );

    setFixedHeight(m_height);

    m_scrollTimer = new QTimer(this);
    connect(m_scrollTimer, SIGNAL(timeout()), this, SLOT(slotScrollTimerFired()));

    setCentered(m_lstDates.lastYear(), 12);

    setMouseTracking(true);
}



TimeRuler::~TimeRuler() {
    ;
}

void TimeRuler::slotSetSelected(QDate date, bool center)
{
    setSelected(date.year(), date.month(), false);
    if (center) {
        setCentered(date.year(), date.month());
    }
}


void TimeRuler::setSelected(int year, int month, bool signalIt)
{
    if (isContained(year, month))
    {
        m_selectedYear = year;
        m_selectedMonth = month;

        if (signalIt) {
            emit selectionChanged(year, month);
        }
    }
}


void TimeRuler::setCentered(int year, int month)
{
    if (isContained(year, month)) {
        smoothShift(dateToBarRect(year, month, true).x() - (width()/2));
    }
}


bool TimeRuler::isContained(int year, int month)
{
    return (year >= m_lstDates.firstYear() && year <= m_lstDates.lastYear()
            && month > 0 && month <= 12);
}


void TimeRuler::initData()
{
    int i;
    for (i = 0; i < 60; ++i) {
        m_lstDates.addDate(QDate(2001,1,1));
    }
    for (i = 0; i < 60; ++i) {
        m_lstDates.addDate(QDate(2001,2,1));
    }
    for (i = 0; i < 30; ++i) {
        m_lstDates.addDate(QDate(2003,4,4));
    }
    for (i = 0; i < 35; ++i) {
        m_lstDates.addDate(QDate(2003,3,4));
    }

    for (i = 0; i < 60; ++i) {
        m_lstDates.addDate(QDate(2001,1,1));
    }
/*    for (i = 0; i < 55; ++i) {
        m_lstDates.addDate(QDate(2001,2,1));
    }*/
    for (i = 0; i < 50; ++i) {
        m_lstDates.addDate(QDate(2001,2,2));
    }
}


void TimeRuler::initUI() {
    int width = m_widthMonth + (m_lstDates.numYears() * 12 ) * m_widthMonth + m_widthMonth;

    m_pixmap = QPixmap(width, m_height);
    m_pixmap.fill(backgroundColor());

    QPainter p (&m_pixmap);

    p.save();

    p.setPen(Qt::gray);
    p.setBrush(Qt::white);
    QStyle& style = KApplication::kApplication()->style();

    //draw a frame
    QColorGroup cg = colorGroup();
    cg.setColor(QColorGroup::Background, Qt::white);

    style.drawPrimitive(QStyle::PE_ButtonBevel, &p,
                        QRect(m_widthMonth, 3, width - 2 * m_widthMonth, m_yBase - 3), cg);

    p.restore();

    //first create the month counter on a temporaroy pixmap
    QPixmap tp(12 * m_widthMonth, 20);
    tp.fill(backgroundColor());
    QPainter tpp (&tp);
    tpp.translate(0,20);
    tpp.rotate(-90);

    QFont f(p.font());
    f.setPixelSize(10);
    f.setBold(false);
    tpp.setFont(f);

    for (int i = 0; i <12; ++i) {
        tpp.drawText(0, i * m_widthMonth, 20, m_widthMonth, Qt::AlignRight | Qt::AlignVCenter, QString::number(i+1));
    }
    tpp.end();


    int over = -2;
    int x = 0;

   //reset the counting mechanism...
    m_lstDates.count(-1);

    for (int y = m_lstDates.firstYear(); y <= m_lstDates.lastYear(); ++ y) {
        for (int m = 1; m <= 12 ; ++m) {

            x = m_widthMonth + ((y - m_lstDates.firstYear())*12 + m -1 ) * m_widthMonth;

            if (m == 1) {
                bitBlt(&m_pixmap, x, m_yBase + 3, &tp, 0, 0, tp.width(), tp.height());

                //draw the year numbers
                f.setPixelSize(16);
                f.setBold(true);
                p.setFont(f);
                p.drawText(x, m_yBase, 12 * m_widthMonth, m_height-m_yBase, Qt::AlignHCenter | Qt::AlignBottom, QString::number(y));

                //draw the bold year lines
                p.setPen(QPen(Qt::gray, 3));
                p.drawLine(x, m_yBase+over, x, m_yBase+22);
                p.setPen(QPen(Qt::black, 1));
                p.drawLine(x, m_yBase+over, x, m_yBase+22);
            } else if (m == 7) {
                //draw the semi big half year marks
                p.setPen(QPen(Qt::gray, 3));
                p.drawLine(x, m_yBase+over, x, m_yBase+8);
                p.setPen(QPen(Qt::black, 1));
                p.drawLine(x, m_yBase+over, x, m_yBase+8);
            } else {
                //draw the thin month marks
                p.setPen(QPen(Qt::black, 1));
                p.drawLine(x, m_yBase+over, x, m_yBase+8);
            }
            drawBeam(&p, m_lstDates.count(y,m, -1, true) * 100 / m_lstDates.maxMonth(), y, m);
        }
    }
    //draw the final year bar thats missing from the loop
    x += m_widthMonth;
    p.setPen(QPen(Qt::gray, 3));
    p.drawLine(x, m_yBase+over, x, m_yBase+22);
    p.setPen(QPen(Qt::black, 1));
    p.drawLine(x, m_yBase+over, x, m_yBase+22);


    //now draw the base line
    p.setPen(QPen(Qt::black, 1));
    p.drawLine(10, m_yBase, width - 10 , m_yBase);

    //done
    p.end();
}


void TimeRuler::drawBeam(QPainter* p, int percent, int year, int month, bool fill, QColor fillColor) {
    //     kdDebug() << "drawBeam " << year << "::" << month << endl;
    //we don't draw with 0 percent!
    if (percent == 0) {
        return;
    }

    //the '6' comes from a frame hight offset of 4 + a bit more (2)
    int h = (m_yBase-6) * percent / 100;
    if (h == 0) {
        h = 1;
    }

    p->save();
    p->setPen(QPen(Qt::black, 1));
    if (fill) {
        p->setBrush(fillColor);
    } else {
        p->setBrush( NoBrush );
    }
    p->drawRect(dateToBarRect(year, month, false, h));
    p->restore();
}


void TimeRuler::resizeEvent ( QResizeEvent * e)
{
    int delta = e->size().width() - e->oldSize().width();

    if (delta > 0) {
        //enlarge
        if (m_xVis >= delta/2  &&  m_xVis + width() < m_pixmap.width()+delta/2)
            shift(-delta/2);
        else if (m_xVis + width() >= m_pixmap.width())
            shift(-delta);

    } else {
        //shrink
        shift(-delta/2);
    }
}


void TimeRuler::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setClipRect(e->rect());

//     kdDebug() << "paintEvent()" << endl;

    painter.drawPixmap(0, 0, m_pixmap, m_xVis, 0, width(), height());

    if (m_mouseOverYear > 0 || m_selectedYear > 0) {
        painter.translate(-m_xVis,0);

        if (m_selectedYear == m_mouseOverYear
            && m_selectedMonth == m_mouseOverMonth)
        {
            drawBeam(&painter,
                     m_lstDates.count(m_selectedYear, m_selectedMonth) * 100/ m_lstDates.maxMonth(),
                     m_selectedYear, m_selectedMonth, true, Qt::gray);
        } else {

            if (m_selectedYear > 0) {
                drawBeam(&painter,
                         m_lstDates.count(m_selectedYear, m_selectedMonth) * 100/ m_lstDates.maxMonth(),
                         m_selectedYear, m_selectedMonth, true, QColor(Settings::toolsViewerOverlayColor()));
            }

            if (m_mouseOverYear > 0) {
                drawBeam(&painter,
                         m_lstDates.count(m_mouseOverYear, m_mouseOverMonth) * 100/ m_lstDates.maxMonth(),
                         m_mouseOverYear, m_mouseOverMonth, true, Qt::gray);
            }
        }
    }

    painter.end();
}


void TimeRuler::mouseMoveEvent ( QMouseEvent * e ) {
    static QPoint last = e->pos();

    if (e->state() & Qt::LeftButton) {// && e->pos().y() > m_yBase) {
        shift(last.x() - e->pos().x());
    }
    last = e->pos();


    //if we leave the active region and have a prevY drawn, remove it
    if (e->pos().y() > m_yBase && m_mouseOverYear > 0) {
        QRect r = dateToBarRect(m_mouseOverYear, m_mouseOverMonth, true);

        m_mouseOverYear = -1;
        repaint(r);

    } else {

        //store the position of the prev. selection
        int prevY = m_mouseOverYear;
        int prevM = m_mouseOverMonth;

        mousePosToOffset(e->pos(), &m_mouseOverYear, &m_mouseOverMonth);

        //remove the prev. paint only, when it is not the current
        if (prevY > 0 && !(prevY == m_mouseOverYear && prevM == m_mouseOverMonth)) {
            update(dateToBarRect(prevY, prevM, true));
        }
        if (m_mouseOverYear > 0) {
            //remove the previous paint
            repaint(dateToBarRect(m_mouseOverYear, m_mouseOverMonth, true));
        }
    }
}


void TimeRuler::mousePressEvent(QMouseEvent* e) {
    //store the press position to find out, if a click was intended or not
    m_mousePressPosition = e->pos();
}


void TimeRuler::mouseReleaseEvent(QMouseEvent* e) {
    int delta = 3;
    const QPoint* p = &(e->pos());
    if (m_mousePressPosition.x() + delta > p->x() &&
            m_mousePressPosition.x() - delta < p->x() &&
            m_mousePressPosition.y() + delta > p->y() &&
            m_mousePressPosition.y() - delta < p->y())
    {
        //store the previous selection
        int prevY = m_selectedYear;
        int prevM = m_selectedMonth;

        //calculate the current selection
        mousePosToOffset(m_mousePressPosition, &m_selectedYear, &m_selectedMonth);

        //if we had a prev selection, 'undraw' it
        if (prevY > 0) {
            update(dateToBarRect(prevY, prevM, true));
        }

        if (m_selectedYear > 0) {
            //now repaint the current selection
            repaint(dateToBarRect(m_selectedYear, m_selectedMonth, true));

            emit selectionChanged(m_selectedYear, m_selectedMonth);
        }
    }

    m_mousePressPosition = QPoint(0,0);
}


void TimeRuler::wheelEvent ( QWheelEvent * e ) {
    shift(20 * (e->delta() / 120));
}


void TimeRuler::mousePosToOffset(QPoint pos, int* year, int* month) {
    if (pos.y() > m_yBase) {
        *year = -1;
        return;
    }

    int m = (pos.x() + m_xVis) / m_widthMonth - 1;

    *month = m % 12 + 1;
    if (*month > 0) {
        *year = m / 12 + m_lstDates.firstYear();
    } else {
        *year = -1;
    }
}



QRect TimeRuler::dateToBarRect(int year, int month, bool translateX, int height ) {
    //if height is 0, we return the bunding rect for the whole month
    if (height < 0) {
        height = m_yBase;
    }

    QRect retval(((year - m_lstDates.firstYear()) * 12 + month ) * m_widthMonth + 3,
                 m_yBase - height - 1,
                 m_widthMonth - 5,
                 height);

    if (translateX) {
        retval.moveBy(-m_xVis, 0);
    }

    return retval;
}


void TimeRuler::smoothShift(int x)
{
    m_scrollLeft = x;
    m_scrollTimer->start(25);
}


bool TimeRuler::shift(int x) {
    bool retval = true;
    if (x > 0) {
        if (m_xVis + width() + x < m_pixmap.width()) {
            m_xVis += x;
        } else {
            retval = false;
            m_xVis = m_pixmap.width() - width() ;
        }
    } else {
        if (m_xVis > -x) {
            m_xVis += x;
        } else {
            retval = false;
            m_xVis = 0;
        }
    }
    update();
    return retval;
}



void TimeRuler::slotScrollTimerFired()
{
    int diff = m_scrollLeft/7;
    m_scrollLeft -= diff;

    if (!shift(diff) || diff == 0) {
        m_scrollTimer->stop();
    }
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


DateBinder::DateBinder() {
    m_maxYear  = -1;
    m_maxMonth = -1;
    m_maxDay   = -1;
}

DateBinder::~DateBinder() {}

void DateBinder::addDate(QDate d) {
    list<QDate>::iterator it;
    for (it= m_lstData.begin(); it != m_lstData.end(); ++it) {
        if ((*it) > d) {
            m_lstData.insert(it, d);
            break;
        }
    }
    // this means, we came to the end and did not add the item yet
    if (it == m_lstData.end()) {
        m_lstData.push_back(d);
    }

    m_maxYear  = -1;
    m_maxMonth = -1;
    m_maxDay   = -1;
}


int DateBinder::count (int year, int month, int day, bool cntd)
{
    int count = 0;
    static list<QDate>::iterator it = m_lstData.begin();

    //we only initialize the iterator, when not cntd. This means, we don't
    // search the list from the beginning again, but from the point, we left it before.
    if (!cntd) {
        it = m_lstData.begin();
    }

    //if no year is given, we don't have to look for anything. But we reset the *it* for the next run.
    if (year <= 0) {
        return 0;
    }

    //first find the first matching item;
    if (year > 0) {
        while (it != m_lstData.end() && it->year() < year) {
            ++it;
        }
        if (month > 0) {
            while (it != m_lstData.end() && it->year() == year && it->month() < month) {
                ++it;
            }
            if (day > 0) {
                while (it != m_lstData.end() && it->year() == year && it->month() == month && it->day() < day) {
                    ++it;
                }
            }
        }
    }

    //then do the counting;
    if (day > 0) {
        while (it != m_lstData.end() && it->year() == year && it->month() == month && it->day() == day) {
            ++it;
            ++count;
        }
    } else if (month > 0) {
        while (it != m_lstData.end() && it->year() == year && it->month() == month) {
            ++it;
            ++count;
        }
    } else if (year > 0) {
        while (it != m_lstData.end() && it->year() == year) {
            ++it;
            ++count;
        }
    }






    return count;
}


int DateBinder::firstYear() {
    if (m_lstData.size()) {
        return m_lstData.front().year();
    }
    return -1;
}


int DateBinder::lastYear() {
    if (m_lstData.size()) {
        return m_lstData.back().year();
    }
    return -1;
}

int DateBinder::numYears() {
    return lastYear() - firstYear() + 1;
}



int DateBinder::maxYear() {
    if (m_maxYear <= 0) {
        //calculate the max;
        list<QDate>::iterator it = m_lstData.begin();
        int cur = 0;
        m_maxYear = 0;
        QDate curdate = *it;
        for (; it != m_lstData.end(); ++it) {

            if (curdate.year() == it->year()) {
                    ++cur;
                } else {

                    if (cur > m_maxYear) {
                        m_maxYear = cur;
                    }
                    cur     = 1;
                    curdate = (*it);
                }
        }

        if (cur > m_maxYear) {
            m_maxYear = cur;
        }
    }
    return m_maxYear ;
}


int DateBinder::maxMonth() {
    if (m_maxMonth <= 0) {
        //calculate the max;
        list<QDate>::iterator it = m_lstData.begin();
        int cur = 0;
        m_maxMonth = 0;
        QDate curdate = *it;
        for (; it != m_lstData.end(); ++it) {

            if (curdate.year() == it->year()
                && curdate.month() == it->month()) {
                ++cur;
            } else {
                if (cur > m_maxMonth) {
                    m_maxMonth = cur;
                }
                cur     = 1;
                curdate = (*it);
            }
        }

        if (cur > m_maxMonth) {
            m_maxMonth = cur;
        }
    }
    return m_maxMonth;
}

int DateBinder::maxDay() {
    if (m_maxDay <= 0) {
        //calculate the max;
        list<QDate>::iterator it = m_lstData.begin();
        int cur = 0;
        m_maxDay = 0;
        QDate curdate = *it;
        for (; it != m_lstData.end(); ++it) {

            if (curdate == *it) {
                ++cur;
            } else {
                if (cur > m_maxDay) {
                    m_maxDay = cur;
                }
                cur     = 1;
                curdate = (*it);
            }
        }

        if (cur > m_maxDay) {
            m_maxDay = cur;
        }
    }
    return m_maxDay;
}










