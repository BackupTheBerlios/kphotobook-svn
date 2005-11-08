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

#include "dlgimageimporterstatus.h"

#include "../tracer/tracer.h"

#include <qtextedit.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <kprogress.h>
#include <qapplication.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>

DlgImageImporterStatus::DlgImageImporterStatus(QWidget* parent, const char* name, bool modal, WFlags fl)
: DialogImageImporterStatusUI(parent,name, modal, fl),
m_currentMode(DlgImageImporterStatus::ModeNone),
m_iFolderCount(0), m_iFileCountTotal(0), m_iFileCountMatched(0), m_wasCanceled(false)
{
    m_teStatusMessages->setVScrollBarMode(QScrollView::AlwaysOn);
    m_icoLblSearch->setPixmap(  KGlobal::iconLoader()->loadIcon( "viewmag", KIcon::Toolbar));
    m_icoLblArchive->setPixmap( KGlobal::iconLoader()->loadIcon( "viewmag", KIcon::Toolbar));
    m_icoLblDest->setPixmap(    KGlobal::iconLoader()->loadIcon( "viewmag", KIcon::Toolbar));

    connect(m_btnAbort, SIGNAL(clicked()), SLOT(slotAbortClicked()));
    connect(m_btnClose, SIGNAL(clicked()), SLOT(close()));
}



DlgImageImporterStatus::~DlgImageImporterStatus()
{
}

void DlgImageImporterStatus::setCurrentMode(Mode m, const QString& msg)
{
    if (!msg.isEmpty()) {
        appendStatusMessage( msg );
    }

    Mode prevMode = m_currentMode;

    m_currentMode = m;

    //setup progressBar stuff
    //set the maxcount
    if (m == DlgImageImporterStatus::ModeArchive) {
        m_progressArchive->setTotalSteps(m_iFileCountMatched);
    } else if (m == DlgImageImporterStatus::ModeDestination) {
        m_progressDest->setTotalSteps(m_iFileCountMatched);
    }
    //reset progress counter
    m_iCurrentProgress = 0;


    //setup icon stuff
    switch (m) {
    case DlgImageImporterStatus::ModeImport:
        m_icoLblSearch->setPixmap(  KGlobal::iconLoader()->loadIcon( "forward", KIcon::Toolbar));
        break;
    case DlgImageImporterStatus::ModeArchive:
        m_icoLblArchive->setPixmap(  KGlobal::iconLoader()->loadIcon( "forward", KIcon::Toolbar));

        m_icoLblSearch->setPixmap(  KGlobal::iconLoader()->loadIcon( "ok", KIcon::Toolbar));
        break;
    case DlgImageImporterStatus::ModeDestination:
        m_icoLblDest->setPixmap(  KGlobal::iconLoader()->loadIcon( "forward", KIcon::Toolbar));

        if (prevMode == DlgImageImporterStatus::ModeImport) {
            m_icoLblSearch->setPixmap(  KGlobal::iconLoader()->loadIcon( "ok", KIcon::Toolbar));
        } else if (prevMode == DlgImageImporterStatus::ModeArchive) {
            m_icoLblArchive->setPixmap(  KGlobal::iconLoader()->loadIcon( "ok", KIcon::Toolbar));
        }
        break;
    case DlgImageImporterStatus::ModeAbort:
        switch (prevMode) {
        case DlgImageImporterStatus::ModeImport:
            m_icoLblSearch->setPixmap(  KGlobal::iconLoader()->loadIcon( "cancel", KIcon::Toolbar));
        case DlgImageImporterStatus::ModeArchive:
            m_icoLblArchive->setPixmap(  KGlobal::iconLoader()->loadIcon( "cancel", KIcon::Toolbar));
        case DlgImageImporterStatus::ModeDestination:
            m_icoLblDest->setPixmap(  KGlobal::iconLoader()->loadIcon( "cancel", KIcon::Toolbar));
            break;
        default:
            break;
        }
        break;
    case DlgImageImporterStatus::ModeFinished:
        m_icoLblSearch->setPixmap(  KGlobal::iconLoader()->loadIcon( "ok", KIcon::Toolbar));
        m_icoLblArchive->setPixmap( KGlobal::iconLoader()->loadIcon( "ok", KIcon::Toolbar));
        m_icoLblDest->setPixmap(    KGlobal::iconLoader()->loadIcon( "ok", KIcon::Toolbar));
        break;
    }

    //handle DONE
    if (m == DlgImageImporterStatus::ModeFinished || m == DlgImageImporterStatus::ModeAbort) {
        m_btnAbort->setEnabled(false);
        m_btnClose->setEnabled(true);
    }

    slotUpdateGUI();
}

void DlgImageImporterStatus::enableImageArchive(bool b)
{
    m_icoLblArchive->setEnabled(b);
    m_lblArchive->setEnabled(b);
    m_progressArchive->setEnabled(b);
}




void DlgImageImporterStatus::incProgress()
{
    if (m_currentMode == DlgImageImporterStatus::ModeArchive) {
        m_progressArchive->setProgress(++m_iCurrentProgress);
    } else if (m_currentMode == DlgImageImporterStatus::ModeDestination) {
        m_progressDest->setProgress(++m_iCurrentProgress);
    }

    qApp->processEvents();
}

void DlgImageImporterStatus::appendStatusMessage(const QString& msg, const QString& color)
{
    // %1 = color
    // %2 = messageText
    QString line = "<li><font color=\"%1\">%1</font><li>";

    m_strStatusMessages += line.arg(color).arg(msg);

    m_teStatusMessages->setText(QString("<ul>%1</ul>").arg(m_strStatusMessages));

    ///@todo implement autoscrolling if the scrollbar is at the bottom
}


void DlgImageImporterStatus::appendErrorMessage(const QString& errorMsg )
{
    appendStatusMessage(errorMsg, "red");
}


void DlgImageImporterStatus::slotUpdateGUI()
{
    m_lblFolderCount->setText(QString("<b>%1</b>").arg(m_iFolderCount));
    m_lblFileCount->setText(QString("<b>%1</b> / <b>%2</b>").arg(m_iFileCountMatched).arg(m_iFileCountTotal));

    //redraw every 125th call only
    static int nth = 0;
    if (++nth > 125) {
        nth = 0;
        m_lblFolderCount->update();
        m_lblFileCount->update();
        //this is the way QProgressDialog works, too
        qApp->processEvents();
    }
}

void DlgImageImporterStatus::slotAbortClicked()
{
    m_wasCanceled = true;
    setCurrentMode(ModeAbort, i18n("Aborted by user interaction"));
}


#include "dlgimageimporterstatus.moc"

