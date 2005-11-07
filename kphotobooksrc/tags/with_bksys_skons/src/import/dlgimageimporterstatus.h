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

#ifndef DLGIMAGEIMPORTERSTATUS_H
#define DLGIMAGEIMPORTERSTATUS_H

#include "dialogimageimporterstatus.h"

#include <qapplication.h>

class DlgImageImporterStatus : public DialogImageImporterStatusUI
{
  Q_OBJECT

public:
    /** the current mode of the import operation */
    enum Mode {ModeNone, ModeImport, ModeArchive, ModeDestination, ModeAbort, ModeFinished};

public:
    /**
     * standardc'tor
     */
    DlgImageImporterStatus(QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );

    virtual ~DlgImageImporterStatus();

    /**
     * used to enable or disable the Archive Slot
     */
    void enableImageArchive(bool b);

    /**
     * sets the currently active Mode to m
     */
    void setCurrentMode(Mode m, const QString& msg = QString());

    /**
     * adds a Folder to the internal counter
     */
    void addFolder() { ++m_iFolderCount;      slotUpdateGUI();}

    /**
     * adds a file to the internal counter (matched or not!)
     */
    void addFile()  { ++m_iFileCountTotal;    slotUpdateGUI();}

    /**
     * adds a matched file to the internal counter
     */
    void addMatch() { ++m_iFileCountMatched;  slotUpdateGUI();}

    /**
     * increases the progressBar of the current Mode by one
     */
    void incProgress();

    /**
     * adds an listItem to the statusTexts in color 'color'
     */
    void appendStatusMessage(const QString& msg, const QString& color = "black");

    /**
     * indicates, that an error has occured
     */
    void appendErrorMessage(const QString& errorMsg);

    /**
     * @returns true, if the user pressec abort
     */
    bool wasCanceled() {return m_wasCanceled; }

public slots:
    void slotUpdateGUI();
    void slotAbortClicked();

private:
    Mode m_currentMode;

    int m_iFolderCount;
    int m_iFileCountTotal;
    int m_iFileCountMatched;

    //current count used for Archive AND Dest progress bar
    int m_iCurrentProgress;

    bool m_wasCanceled;

    //status messages are cached here
    QString m_strStatusMessages;
};

#endif

