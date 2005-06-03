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

#include <kapplication.h>

#include <qpainter.h>

#include <list>

using namespace std;

Tracer* TimeRuler::tracer = Tracer::getInstance("kde.kphotobook.dialogs", "TimeRuler");

TimeRuler::TimeRuler(QWidget* parent)
: QWidget(parent)
{
    initData();

    m_mWidth = 15;
    m_height = 75;
    m_yBase  = 40;

    m_mouseOverYear = -1;
    m_mouseOverMonth = -1;

    initUI();

    resize(450,m_height);

    setMouseTracking(true);
    m_xVis = 0;
}



TimeRuler::~TimeRuler()
{
    ;
}




void TimeRuler::initData()
{
    m_lstDates.addDate(QDate(1995,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2001,1,1));
    m_lstDates.addDate(QDate(2004,4,4));
    m_lstDates.addDate(QDate(2003,3,3));
    m_lstDates.addDate(QDate(2005,5,5));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,3,4));
    m_lstDates.addDate(QDate(2003,6,6));
    m_lstDates.addDate(QDate(2002,2,2));
}

void TimeRuler::initUI()
{
    int width = (m_lstDates.numYears() * 12 + 1 ) * m_mWidth + 20;

    m_pixmap = QPixmap(width, m_height);
    m_pixmap.fill(backgroundColor());

    QPainter p (&m_pixmap);

    //first create the month counter on a temporaroy pixmap
    QPixmap tp(12 * m_mWidth, 20);
    tp.fill(backgroundColor());
    QPainter tpp (&tp);
    tpp.translate(0,20);
    tpp.rotate(-90);

    QFont f(p.font());
    f.setPixelSize(10);
    f.setBold(false);
    tpp.setFont(f);

    for (int i = 0; i <12; ++i) {
        tpp.drawText(0, i * m_mWidth, 20, m_mWidth, Qt::AlignRight | Qt::AlignVCenter, QString::number(i+1));
    }
    tpp.end();


    int over = -2;
    int x = 0;
    for (int y = m_lstDates.minYear(); y <= m_lstDates.maxYear(); ++ y) {
        for (int m = 1; m <= 12 ; ++m) {

            x = m_mWidth + ((y - m_lstDates.minYear())*12 + m -1 ) * m_mWidth;

            if (m == 1) {
                bitBlt(&m_pixmap, x, m_yBase + 3, &tp, 0, 0, tp.width(), tp.height());

                //draw the year numbers
                f.setPixelSize(16);
                f.setBold(true);
                p.setFont(f);
                p.drawText(x, m_yBase, 12 * m_mWidth, m_height-m_yBase, Qt::AlignHCenter | Qt::AlignBottom, QString::number(y));

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
            ///@todo here the max should be checked for percentage calculation
            drawBeam(&p, m_lstDates.count(y,m), y, m);
        }
    }
    //draw the final year bar thats missing from the loop
    x += m_mWidth;
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


void TimeRuler::drawBeam(QPainter* p, int percent, int year, int month, bool fill, QColor fillColor)
{
//     kdDebug() << "drawBeam " << year << "::" << month << endl;
    //we don't draw with 0 percent!
    if (percent == 0) {
        return;
    }

    int h = (m_yBase-2) * percent / 100;
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
    p->drawRect(dateToBoundingRect(year, month, h));
    p->restore();
}


void TimeRuler::resizeEvent ( QResizeEvent * e)
{
    int delta = e->oldSize().width() - e->size().width();

    if (delta < 0 && m_xVis >= 0
        || delta > 0 && m_xVis <= m_pixmap.width() - width() ) {
        m_xVis += delta/2;
    }
}


void TimeRuler::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.setClipRect(e->rect());

    painter.drawPixmap(0, 0, m_pixmap, m_xVis, 0, width(), height());

    if (m_mouseOverYear > 0) {
        painter.translate(-m_xVis,0);
        drawBeam(&painter, m_lstDates.count(m_mouseOverYear, m_mouseOverMonth),
                 m_mouseOverYear, m_mouseOverMonth, true, Qt::gray);
    }

    painter.end();
}


void TimeRuler::mouseMoveEvent ( QMouseEvent * e )
{
    static QPoint last = e->pos();

    if (e->state() & Qt::LeftButton) {
        shift(last.x() - e->pos().x());
    }
    last = e->pos();


    //if we leave the active region and have a prevY drawn, remove it
    if (e->pos().y() > m_yBase && m_mouseOverYear > 0) {
        QRect r = dateToBoundingRect(m_mouseOverYear, m_mouseOverMonth);

        m_mouseOverYear = -1;

        r.moveBy(-m_xVis,0);
        repaint(r);

    } else {

        //store the position of the prev. selection
        int prevY = m_mouseOverYear;
        int prevM = m_mouseOverMonth;

        mousePosToOffset(e->pos(), &m_mouseOverYear, &m_mouseOverMonth);
        if (m_mouseOverYear > 0) {
        //remove the previous paint
            if (prevY > 0 ) {//&& prevY != m_mouseOverYear && prevM != m_mouseOverMonth) {
                QRect r = dateToBoundingRect(prevY, prevM);
                r.moveBy(-m_xVis,0);
                update(r);
            }
            QRect r = dateToBoundingRect(m_mouseOverYear, m_mouseOverMonth);
            r.moveBy(-m_xVis,0);
            repaint(r);
        }
    }
}


void TimeRuler::wheelEvent ( QWheelEvent * e )
{
    shift(20 * (e->delta() / 120));
}


void TimeRuler::mousePosToOffset(QPoint pos, int* year, int* month)
{
    if (pos.y() > m_yBase) {
        *year = -1;
        return;
    }

    int m = (pos.x() + m_xVis) / m_mWidth - 1;

    *year = m / 12 + m_lstDates.minYear();
    *month = m % 12 + 1;
}



QRect TimeRuler::dateToBoundingRect(int year, int month, int height)
{
    //if height is 0, we return the bunding rect for the whole month
    if (height < 0) {
        height = m_yBase;
    }

    return QRect(((year - m_lstDates.minYear()) * 12 + month ) * m_mWidth + 3,
                 m_yBase - height - 1,
                 m_mWidth - 5,
                 height);
}



void TimeRuler::shift(int x)
{
    if (x > 0) {
        if (m_xVis + width() + x < m_pixmap.width()) {
            m_xVis += x;
        } else {
            m_xVis = m_pixmap.width() - width() ;
        }
    } else {
        if (m_xVis > -x) {
            m_xVis += x;
        } else {
            m_xVis = 0;
        }
    }
    update();
}



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


DateBinder::DateBinder()
{
}

DateBinder::~DateBinder()
{
}

void DateBinder::addDate(QDate d)
{
    list<QDate>::iterator it;
    for (it= m_lstData.begin(); it != m_lstData.end(); ++it) {
        if ((*it) > d) {
            m_lstData.insert(it, d);
            break;
        }
    }
    if (it == m_lstData.end()) {
        m_lstData.push_back(d);
    }
}


int DateBinder::count (int year, int month, int day)
{
    if (year <= 0 || month <= 0) {
        return 0;
    }

    int count = 0;
    list<QDate>::iterator it = m_lstData.begin();

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


int DateBinder::minYear()
{
    if (m_lstData.size()) {
        return m_lstData.front().year();
    }
    return -1;
}


int DateBinder::maxYear()
{
    if (m_lstData.size()) {
        return m_lstData.back().year();
    }
    return -1;
}

int DateBinder::numYears()
{
    return maxYear() - minYear() + 1;
}














