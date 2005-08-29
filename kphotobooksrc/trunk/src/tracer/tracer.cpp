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


// stolen from kdebug.cpp...
enum KDEDebugLevels {
     KDEBUG_INFO=0,
     KDEBUG_WARN=1,
     KDEBUG_ERROR=2,
     KDEBUG_FATAL=3
};


//
// initialize static members
//
int Tracer::KDEBUG_TRACE_LEVEL_LOOKUP_TABLE[6] = {KDEBUG_INFO, KDEBUG_INFO, KDEBUG_INFO, KDEBUG_WARN, KDEBUG_ERROR, KDEBUG_FATAL};
QString Tracer::TRACE_LEVEL_NAME[6] = {"DEBUG", "INVOKED", "INFO", "WARNING", "ERROR", "FATAL"};
QString Tracer::TRACE_LEVEL_NAME_FIXED_LENGTH[6] = {"DEBUG  ", "INVOKED", "INFO   ", "WARNING", "ERROR  ", "FATAL  "};
QChar Tracer::TRACER_NAME_SEPARATOR('.');

Tracer* Tracer::s_rootTracer = new Tracer("", "", "");


//
// public static methods
//
Tracer* Tracer::getInstance(const char* tracername, const char* classname)
{
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

            // link the new tracer into the tracer tree
            currentTracer->m_children->append(child);
            child->m_parent = currentTracer;

            // tracers inherit the tracelevel of the parent
            child->m_tracelevel = currentTracer->m_tracelevel;
            
            currentTracer = child;
        }
    }

    // if a classname is specified but the resulting tracer has no one set
    // the tracer was already instantiated without a classname set
    // this most likely occurs when the tracer was configured by a traceconfigfile
    // --> force the classname
    if (!strClassname.isEmpty() && currentTracer->m_classname->isEmpty()) {
        currentTracer->m_classname = new QString(strClassname);
    }
    

    return currentTracer;
}


//
// private constructor
//
Tracer::Tracer(const QString& tracername, const QString& tracerpartname, const QString& classname) :
    m_tracername(new QString(tracername)),
    m_tracerpartname(new QString(tracerpartname)),
    m_classname(new QString(classname)),
    m_tracelevel(LEVEL_DEBUG),
    m_parent(0),
    m_children(new QPtrList<Tracer>())
{
}


//
// public methods
//
Tracer::~Tracer()
{
    delete m_tracername;
    delete m_tracerpartname;
    delete m_classname;
    delete m_children;
}


void Tracer::setTraceLevel(TraceLevel tracelevel, bool recursive)
{
    m_tracelevel = tracelevel;

    if (recursive) {
        // set the tracelevl of all children recursively
        Tracer* child;
        for (child = m_children->first(); child; child = m_children->next()) {
            child->setTraceLevel(tracelevel, true);
        }
    }
}


bool Tracer::setTraceLevel(const QString& tracelevel, bool recursive)
{
    // uppercase the tracelevel to set
    QString preparedTracelevel = tracelevel.upper().stripWhiteSpace();

    // determine the id of the tracelevel to set
    for (int level = 0; level <= 6; level++) {
        if (preparedTracelevel == TRACE_LEVEL_NAME[level]) {
            setTraceLevel((TraceLevel)level, recursive);
            return true;
        }        
    }

    return false;
}


void Tracer::debug(const char* method, const char* message, ...) const
{
    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_DEBUG) {
        return;
    }
    
    ///@todo with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    sdebug(method) << str << endl;
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

    sdebug(method) << str << endl;
    delete str;
}


void Tracer::invoked(const char* method) const
{
    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_INVOKED) {
        return;
    }

    sinvoked(method) << "invoked..." << endl;
}


void Tracer::invoked(const char* method, const char* message, ...) const
{
    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_INVOKED) {
        return;
    }

    ///@todo with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    sinvoked(method) << str << endl;
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

    sinvoked(method) << str << endl;
    delete str;
}


void Tracer::info(const char* method, const char* message, ...) const
{
    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_INFO) {
        return;
    }

    ///@todo with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    sinfo(method) << str << endl;
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

    sinfo(method) << str << endl;
    delete str;
}


void Tracer::warning(const char* method, const char* message, ...) const
{
    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_WARNING) {
        return;
    }

    ///@todo with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    swarning(method) << str << endl;
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

    swarning(method) << str << endl;
    delete str;
}


void Tracer::error(const char* method, const char* message, ...) const
{
    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_ERROR) {
        return;
    }

    ///@todo with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    serror(method) << str << endl;
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

    serror(method) << str << endl;
    delete str;
}


void Tracer::fatal(const char* method, const char* message, ...) const
{
    // fatal traces cannot be suppressed!

    ///@todo with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    sfatal(method) << str << endl;
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

    sfatal(method) << str << endl;
    delete str;
}


void Tracer::trace(TraceLevel level, const char* method, const char* message, ...) const
{
    // immediately return if the current set tracelevel is higher
    if (m_tracelevel > level) {
        return;
    }

    ///@todo with qt4 the follwing code can be used:
    /*
    va_list ap;
    va_start(ap, message);
    QString str;
    str.vsprintf(message, ap);
    va_end(ap);

    strace(level, method) << str << endl;
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

    strace(level, method) << str << endl;
    delete str;
}


kdbgstream Tracer::sdebug(const char* method) const
{
    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_DEBUG) {
        return kdbgstream("", 0, KDEBUG_INFO, false);
    }
    
    return kdbgstream(prepareMessageHeader(LEVEL_DEBUG, method), 0, KDEBUG_INFO, true);
}


kdbgstream Tracer::sinvoked(const char* method) const
{
    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_INVOKED) {
        return kdbgstream("", 0, KDEBUG_INFO, false);
    }
    
    return kdbgstream(prepareMessageHeader(LEVEL_INVOKED, method), 0, KDEBUG_INFO, true);
}


kdbgstream Tracer::sinfo(const char* method) const
{
    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_INFO) {
        return kdbgstream("", 0, KDEBUG_INFO, false);
    }
    
    return kdbgstream(prepareMessageHeader(LEVEL_INFO, method), 0, KDEBUG_INFO, true);
}


kdbgstream Tracer::swarning(const char* method) const
{
    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_WARNING) {
        return kdbgstream("", 0, KDEBUG_WARN, false);
    }
    
    return kdbgstream(prepareMessageHeader(LEVEL_WARNING, method), 0, KDEBUG_WARN, true);
}


kdbgstream Tracer::serror(const char* method) const
{
    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > LEVEL_ERROR) {
        return kdbgstream("", 0, KDEBUG_ERROR, false);
    }
    
    return kdbgstream(prepareMessageHeader(LEVEL_ERROR, method), 0, KDEBUG_ERROR, true);
}


kdbgstream Tracer::sfatal(const char* method) const
{
    // fatal traces cannot be suppressed!
    
    return kdbgstream(prepareMessageHeader(LEVEL_FATAL, method), 0, KDEBUG_FATAL, true);
}


kdbgstream Tracer::strace(TraceLevel level, const char* method) const
{
    // return a NULL stream if the current set tracelevel is higher
    if (m_tracelevel > level) {
        return kdbgstream("", 0, KDEBUG_TRACE_LEVEL_LOOKUP_TABLE[level], false);
    }

    return kdbgstream(prepareMessageHeader(level, method), 0, KDEBUG_TRACE_LEVEL_LOOKUP_TABLE[level], true);
}


void Tracer::dump(QString indention) const
{
    // print some useful infos about this tracer
    kdbgstream(indention, 0, 999, true)
            << "- "
            << *m_tracerpartname
            << " [" << TRACE_LEVEL_NAME[m_tracelevel] << "] "
            << "(classname: " << *m_classname << "), "
            << "fulltracername: " << *m_tracername
            << endl;

    // call dump on each child
    indention.append("  ");
    Tracer* child;
    for (child = m_children->first(); child; child = m_children->next()) {
        child->dump(indention);
    }
}


//
// private methods
//
QString Tracer::prepareMessageHeader(TraceLevel level, const char* method) const
{
    QString msg;

    msg.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
    msg.append(" ");
    msg.append(TRACE_LEVEL_NAME_FIXED_LENGTH[level]);
    msg.append(" [");
    msg.append(*m_classname);
    msg.append("::");
    msg.append(method);
    msg.append("] ");

    return msg;
}


Tracer* Tracer::getChild(const QString& tracerpartname) const
{
    if (tracerpartname.isEmpty()) {
        return 0;
    }

    Tracer* child;
    for (child = m_children->first(); child; child = m_children->next()) {
        if (*(child->m_tracerpartname) == tracerpartname) {
            return child;
        }
    }

    return 0;
}
