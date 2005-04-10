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

#include "tracer.h"

#include <qstringlist.h>

#include <cstdarg> // used for va_list, va_start, va_end
#include <printf.h>


// stolen from kdebug.cpp!
enum KDEDebugLevels {
     KDEBUG_INFO=0,
     KDEBUG_WARN=1,
     KDEBUG_ERROR=2,
     KDEBUG_FATAL=3
};


QChar Tracer::TRACER_NAME_SEPARATOR('.');
Tracer* Tracer::s_rootTracer = new Tracer("", "", "");

        
Tracer* Tracer::getInstance(const char* tracername, const char* classname) {

    QString strTracername(tracername);
    QString strClassname(classname);

    // if neither tracername nor classname is specified we return the root-tracer
    if (strTracername.isEmpty() && strClassname.isEmpty()) {
        return s_rootTracer;
    }

    Tracer* currentTracer = s_rootTracer;
    QString currentTracerName = QString("");
    
    // split the tracername into its parts
    QStringList parts = QStringList::split(TRACER_NAME_SEPARATOR, strTracername, false);

    // if specified the classname is handled as last part of the tracername
    if (!strClassname.isEmpty()) {
        parts.append(strClassname);
    }

    int partsCount = parts.count();

    // loop over the tracename parts and
    // try to get the tracer matching the tracername and classname
    // as soon as we don't find a matching subtracer we begin to create new tracers...
    for (int i = 0; i < partsCount; i++) {
        QString part = parts[i];
        
        // create the tracername of the tracer we would like at this point
        if (i > 0) {
            currentTracerName = currentTracerName.append(TRACER_NAME_SEPARATOR);
        }
        currentTracerName = currentTracerName.append(part);

        // try to get a matching tracer for the currentTracerName
        Tracer* matchingChild = currentTracer->getChild(part);
        if (matchingChild) {
            currentTracer = matchingChild;
        } else {
            //no matching subtracer found --> create one!

            Tracer* child;
            if (i + 1 == partsCount) {
                // this is the last tracer to instantiate --> create WITH classname
                child = new Tracer(currentTracerName, part, strClassname);
            } else {
                // this is a missing supertracer of the tracer to create --> create without classname
                child = new Tracer(currentTracerName, part, "");
            }

            currentTracer->m_children->append(child);
            child->m_parent = currentTracer;
            currentTracer = child;
        }
    }

    return currentTracer;
}


QString Tracer::level2string(TraceLevel level,  int minlength) {

    kdDebug() << "[Tracer::level2string] invoked with level " << level << endl;
    
    QString str;
    
    switch(level) {
        case LEVEL_DEBUG:
            str = "DEBUG";
            break;
        case LEVEL_INVOKED:
            str = "INVOKED";
            break;
        case LEVEL_INFO:
            str = "INFO";
            break;
        case LEVEL_WARNING:
            str = "WARNING";
            break;
        case LEVEL_ERROR:
            str = "ERROR";
            break;
        case LEVEL_FATAL:
            str = "FATAL";
            break;
    }

    // append spaces to the get the desired length
    for (int i = str.length(); i < minlength; i++) {
        str.append(" ");
    }
    
    return str;
}


Tracer::Tracer(QString tracername, QString tracerpartname, QString classname)
    : m_tracername(new QString(tracername))
    , m_tracerpartname(new QString(tracerpartname))
    , m_classname(new QString(classname))
    , m_tracelevel(LEVEL_INFO)
    , m_parent(0)
    , m_children(new QPtrList<Tracer>()) {
}


Tracer::~Tracer() {
    delete m_tracername;
    delete m_classname;
    delete m_children;
}


void Tracer::setTraceLevel(TraceLevel tracelevel, bool recursive) {

    m_tracelevel = tracelevel;

    if (recursive) {
        // set the tracelevl of all children recursively
        Tracer* child;
        for (child = m_children->first(); child; child = m_children->next()) {
            child->setTraceLevel(tracelevel, true);
        }
    }
}


void Tracer::debug(const char* method, const char* message, ...) {

    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_DEBUG) {
        return;
    }
    
    // with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    doTrace(LEVEL_DEBUG, method, str);
    */

    va_list ap;

    int maxLength = 1024;
    int result = maxLength;
    char* str;
    while (result >= maxLength) {
        va_start(ap, message);
        str = (char*)malloc(maxLength);
        result = vsnprintf(str, maxLength-1, message, ap);
        va_end(ap);

        if (result >= maxLength) {
            delete str;
            maxLength = 2 * maxLength;
            result = maxLength;
        }
    }

    doTrace(LEVEL_DEBUG, method, str);
    delete str;
}


void Tracer::invoked(const char* method) {

    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_INVOKED) {
        return;
    }

    trace(LEVEL_INVOKED, method, "invoked...");
}


void Tracer::invoked(const char* method, const char* message, ...) {

    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_INVOKED) {
        return;
    }

    // with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    doTrace(LEVEL_INVOKED, method, str);
    */

    va_list ap;

    int maxLength = 1024;
    int result = maxLength;
    char* str;
    while (result >= maxLength) {
        va_start(ap, message);
        str = (char*)malloc(maxLength);
        result = vsnprintf(str, maxLength-1, message, ap);
        va_end(ap);

        if (result >= maxLength) {
            delete str;
            maxLength = 2 * maxLength;
            result = maxLength;
        }
    }
    doTrace(LEVEL_INVOKED, method, str);
    delete str;
}


void Tracer::info(const char* method, const char* message, ...) {

    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_INFO) {
        return;
    }

    // with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    doTrace(LEVEL_INFO, method, str);
    */

    va_list ap;

    int maxLength = 1024;
    int result = maxLength;
    char* str;
    while (result >= maxLength) {
        va_start(ap, message);
        str = (char*)malloc(maxLength);
        result = vsnprintf(str, maxLength-1, message, ap);
        va_end(ap);

        if (result >= maxLength) {
            delete str;
            maxLength = 2 * maxLength;
            result = maxLength;
        }
    }

    doTrace(LEVEL_INFO, method, str);
    delete str;
}


void Tracer::warning(const char* method, const char* message, ...) {

    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_WARNING) {
        return;
    }

    // with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    doTrace(LEVEL_WARNING, method, str);
    */

    va_list ap;

    int maxLength = 1024;
    int result = maxLength;
    char* str;
    while (result >= maxLength) {
        va_start(ap, message);
        str = (char*)malloc(maxLength);
        result = vsnprintf(str, maxLength-1, message, ap);
        va_end(ap);

        if (result >= maxLength) {
            delete str;
            maxLength = 2 * maxLength;
            result = maxLength;
        }
    }

    doTrace(LEVEL_WARNING, method, str);
    delete str;
}


void Tracer::error(const char* method, const char* message, ...) {

    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_ERROR) {
        return;
    }

    // with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    doTrace(LEVEL_ERROR, method, str);
    */

    va_list ap;

    int maxLength = 1024;
    int result = maxLength;
    char* str;
    while (result >= maxLength) {
        va_start(ap, message);
        str = (char*)malloc(maxLength);
        result = vsnprintf(str, maxLength-1, message, ap);
        va_end(ap);

        if (result >= maxLength) {
            delete str;
            maxLength = 2 * maxLength;
            result = maxLength;
        }
    }

    doTrace(LEVEL_ERROR, method, str);
    delete str;
}


void Tracer::fatal(const char* method, const char* message, ...) {

    // fatal traces cannot be suppressed!

    // with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    doTrace(LEVEL_FATAL, method, str);
    */

    va_list ap;

    int maxLength = 1024;
    int result = maxLength;
    char* str;
    while (result >= maxLength) {
        va_start(ap, message);
        str = (char*)malloc(maxLength);
        result = vsnprintf(str, maxLength-1, message, ap);
        va_end(ap);

        if (result >= maxLength) {
            delete str;
            maxLength = 2 * maxLength;
            result = maxLength;
        }
    }

    doTrace(LEVEL_FATAL, method, str);
    delete str;
}


void Tracer::trace(TraceLevel level, const char* method, const char* message, ...) {

    kdDebug() << "[Tracer::trace] invoked..." << endl;
    
    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > level) {
        return;
    }

    // with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    doTrace(level, method, str);
    */

    va_list ap;

    int maxLength = 1024;
    int result = maxLength;
    char* str;
    while (result >= maxLength) {
        va_start(ap, message);
        str = (char*)malloc(maxLength);
        result = vsnprintf(str, maxLength-1, message, ap);
        va_end(ap);

        if (result >= maxLength) {
            delete str;
            maxLength = 2 * maxLength;
            result = maxLength;
        }
    }

    doTrace(level, method, str);
    delete str;
}


kdbgstream Tracer::sdebug(const char* method) {

    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_DEBUG) {
        return kdbgstream("", 0, KDEBUG_INFO, false);
    }
    
    return kdbgstream(prepareMessageHeader(LEVEL_DEBUG, method), 0, KDEBUG_INFO, true);
}


kdbgstream Tracer::sinvoked(const char* method) {

    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_INVOKED) {
        return kdbgstream("", 0, KDEBUG_INFO, false);
    }
    
    return kdbgstream(prepareMessageHeader(LEVEL_INVOKED, method), 0, KDEBUG_INFO, true);
}


kdbgstream Tracer::sinfo(const char* method) {

    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_INFO) {
        return kdbgstream("", 0, KDEBUG_INFO, false);
    }
    
    return kdbgstream(prepareMessageHeader(LEVEL_INFO, method), 0, KDEBUG_INFO, true);
}


kdbgstream Tracer::swarning(const char* method) {

    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_WARNING) {
        return kdbgstream("", 0, KDEBUG_WARN, false);
    }
    
    return kdbgstream(prepareMessageHeader(LEVEL_WARNING, method), 0, KDEBUG_WARN, true);
}


kdbgstream Tracer::serror(const char* method) {

    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_ERROR) {
        return kdbgstream("", 0, KDEBUG_ERROR, false);
    }
    
    return kdbgstream(prepareMessageHeader(LEVEL_ERROR, method), 0, KDEBUG_ERROR, true);
}


kdbgstream Tracer::sfatal(const char* method) {

    // fatal traces cannot be suppressed!
    
    return kdbgstream(prepareMessageHeader(LEVEL_FATAL, method), 0, KDEBUG_FATAL, true);
}


kdbgstream Tracer::strace(TraceLevel level, const char* method) {

    kdDebug() << "[Tracer::strace] invoked..." << endl;
    
    int kDebugLevel = KDEBUG_INFO;
    switch (level) {
        case LEVEL_WARNING:
            kDebugLevel = KDEBUG_WARN;
            break;
        case LEVEL_ERROR:
            kDebugLevel = KDEBUG_ERROR;
            break;
        case LEVEL_FATAL:
            kDebugLevel = KDEBUG_FATAL;
            break;
        default:
            break;
    }
    
    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > level) {
        return kdbgstream("", 0, kDebugLevel, false);
    }

    return kdbgstream(prepareMessageHeader(level, method), 0, kDebugLevel, true);
}

