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

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <qstring.h>

/**
 * The EngineException is the superclass of all exceptions thrown by the engine.
 *
 * CVS-ID $Id$
 */
class EngineException {

public:
    EngineException(QString message)
        : m_message(message)
        , m_detailMessage(QString::null) {
    }

    EngineException(QString message, QString detailMessage)
        : m_message(message)
        , m_detailMessage(detailMessage) {
    }

    virtual ~EngineException() {
        delete m_message;
        delete m_detailMessage;
    }

    QString message() {
        if (m_message.isNull()) {
            return QString("");
        }
        return m_message;
    }

    QString detailMessage() {
        if (m_detailMessage.isNull()) {
            return QString("");
        }
        return m_detailMessage;
    }
    void appendDetailMessage(QString& detailMessage) {
        if (m_detailMessage.isEmpty()) {
            m_detailMessage = QString("").append(detailMessage);
        } else {
            m_detailMessage.append("\n").append(detailMessage);
        }
    }

    QString toString() {
        if (m_detailMessage.isEmpty()) {
            return m_message;
        } else {
            return QString(m_message).append("\n").append(m_detailMessage);
        }
    }

private:
    QString m_message;
    QString m_detailMessage;
};

/**
 * The PersistingException is thrown when a problem during persisting
 * the xml file occurs.
 *
 * CVS-ID $Id$
 */
class PersistingException : public EngineException {

public:
    PersistingException(QString message)
        : EngineException(message) {
    }

    PersistingException(QString message, QString detailMessage)
        : EngineException(message, detailMessage) {
    }

    ~PersistingException() {
    }

private:
};

#endif
