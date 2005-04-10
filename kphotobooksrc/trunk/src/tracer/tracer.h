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

#ifndef TRACER_H
#define TRACER_H

#include <kdebug.h> // we use the kdbgstream

#include <qstring.h>
#include <qdatetime.h>
#include <qptrlist.h>


/**
 * A simple and powerful enough tracer.
 *
 * ATTENTION: a trace with tracelevel 'FATAL' will abort the application!
 *
 * CVS-ID $Id: tagnode.h 299 2005-04-08 22:16:51Z starcube $
 */
class Tracer {

public:
    /**
     * The valid tracelevels.
     */
    enum TraceLevel {LEVEL_DEBUG = 0, LEVEL_INVOKED, LEVEL_INFO, LEVEL_WARNING, LEVEL_ERROR, LEVEL_FATAL};

    /**
     * The character used to separate the parts of the tracername.
     */
    static QChar TRACER_NAME_SEPARATOR;

    
public:
    /**
     * Returns the tracer instance for the specified tracername.
     * If no parameter or two empty strings are specified, the root-tracer is returned.
     * The specified classname is automatically appended as last part to the tracername!
     *
     * @param tracername the name of the tracerinstance to return. (default is empty string)
     * @param classname the classname of the class this tracer is used in. (default is empty string)
     */
    static Tracer* getInstance(const char* tracername = "\0", const char* classname = "\0'");

    /**
     * Returns the root tracer. This is the toplevel tracer instance.
     */
    static Tracer* getRootTracer() {

        return s_rootTracer;
    }

    /**
     * Returns a string representing the specified trace level.
     * The string contains at least 'minlength' characters.
     *
     * @param level the tracelevel to name.
     * @param minlength the minimum number of characters in the returned string.
     * @return the name of the specified tracelevel.
     */
    static QString level2string(TraceLevel level, int minlength = 0);


private:
    Tracer(QString tracername, QString tracerpartname, QString classname);

    
public:
    ~Tracer();
    

    /**
     * Sets the tracelevel of this tracer instance.
     * If recursive is set to true, the tracelevel of all children is adjusted too.
     * Only messages traced with a level higher or equal to this tracelevel are traced!
     * The default tracelevel is LEVEL_INFO.
     *
     * @param tracelevel the new tracelevel for this tracer instance.
     * @param recursive defines if the tracelevel is applied recusrively to all children (default is false).
     */
    void setTraceLevel(TraceLevel tracelevel, bool recursive = false);
    
    /**
     * Returns the currently set tracelevel of this tracer instance.
     * Only messages traced with a level higher or equal to this tracelevel are traced!
     *
     * @return the currently set tracelevel.
     */
    TraceLevel getTraceLevel() {
        
        return m_tracelevel;
    }
    
    void debug(const char* method, const char* message, ...);    
    void invoked(const char* method);
    void invoked(const char* method, const char* message, ...);    
    void info(const char* method, const char* message, ...);    
    void warning(const char* method, const char* message, ...);    
    void error(const char* method, const char* message, ...);    
    void fatal(const char* method, const char* message, ...);    
    void trace(TraceLevel level, const char* method, const char* message, ...);
    
    kdbgstream sdebug(const char* method);
    kdbgstream sinvoked(const char* method);
    kdbgstream sinfo(const char* method);
    kdbgstream swarning(const char* method);
    kdbgstream serror(const char* method);
    kdbgstream sfatal(const char* method);
    kdbgstream strace(TraceLevel level, const char* method);
    
private:
    /**
     * This method does the actual printing of the trace message.
     */
    void doTrace(TraceLevel level, const char* method, const char* message) {

        kdDebug() << "[Tracer::doTrace] invoked... with message: " << message << endl;
        strace(level, method) << message << endl;
        strace(level, method).flush();
    }

    /**
     * Prepares the header for each trace-line:
     * timestamp levelname [class::method] '
     */
    QString prepareMessageHeader(TraceLevel level, const char* method) {

        QString msg;
        
        msg.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
        msg.append(" ");
        msg.append(level2string(level, 7));
        msg.append(" [");
        msg.append(*m_classname);
        msg.append("::");
        msg.append(method);
        msg.append("] ");

        return msg;
    }
    
    /**
     * Returns the child tracer with the given tracerpartname or 0 if no such
     * child was found or the given tracerpartname is null or empty.
     *
     * @param tracerpartname the last part of the child tracer to return.
     * @return the appropriate child tracer or 0.
     */
    Tracer* getChild(QString tracerpartname) {

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
    

private:
    /**
     * The root tracer.
     */
    static Tracer* s_rootTracer;

    
private:
    /**
     * The full name of this tracer instance.
     */
    QString* m_tracername;
    
    /**
     * The last part of the name of this tracer instance.
     */
    QString* m_tracerpartname;
    
    /**
     * The name of the class this tracer is used in.
     * This is just used to be printed in the trace message
     */
    QString* m_classname;

    /**
     * The currently set tracelevel of this tracer instance.
     * Only messages traced with a level higher or equal to this tracelevel are traced!
     */
    TraceLevel m_tracelevel;

    /**
     * The parent tracer of this tracer.
     * If the parent is null, thsi is the root tracer.
     */
    Tracer* m_parent;
    
    /**
     * All direct children of this tracer.
     */
    QPtrList<Tracer>* m_children;
};

#endif
