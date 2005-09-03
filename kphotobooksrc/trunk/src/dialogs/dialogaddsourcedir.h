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

#ifndef DIALOGADDSOURCEDIR_H
#define DIALOGADDSOURCEDIR_H

#include <kdialogbase.h>
class KLineEdit;

class QCheckBox;
class QDir;


/**
 * Dialog for adding a source directory.
 *
 * CVS-ID $Id: dialogaddsourcedir.h 435 2005-08-02 20:36:17Z starcube $
 */
class DialogAddSourceDir : public KDialogBase
{
    Q_OBJECT
    
    public:
        DialogAddSourceDir(QWidget* parent = 0, const char* name = 0);
    
        ~DialogAddSourceDir();
    
        QDir* directory() const;
    
        bool recursive() const;
    
    private slots:
        void slotDirectoryButtonClicked();
        void slotTextChanged(const QString& text);
    
    private:
        KLineEdit* m_currentDirectoryLineEdit;
        QCheckBox* m_recursiveCheckBox;
};

#endif
