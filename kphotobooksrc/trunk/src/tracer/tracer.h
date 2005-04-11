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

#include <kdebug.h> // we need the kdbgstream

#include <qstring.h>
#include <qdatetime.h>
#include <qptrlist.h>


/**
 * A simple and powerful enough tracer.
 * We just implemented the functions we are currently using. It is very likely that
 * we forgot a lot of useful methods...
 *
 * ATTENTION: a trace with tracelevel 'FATAL' will abort the application!
 *
 * 
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
     * Contains the kdebuglevels to use.
     * index             --> content
     * ----------------------------------
     * 0 [LEVEL_DEBUG]   --> KDEBUG_INFO
     * 1 [LEVEL_INVOKED] --> KDEBUG_INFO
     * 2 [LEVEL_INFO]    --> KDEBUG_INFO
     * 3 [LEVEL_WARNING] --> KDEBUG_WARN
     * 4 [LEVEL_ERROR]   --> KDEBUG_ERROR
     * 5 [LEVEL_FATAL]   --> KDEBUG_FATAL
     */
    static int KDEBUG_TRACE_LEVEL_LOOKUP_TABLE[];

    /**
     * Contains the names of the tracelevels.
     */
    static QString TRACE_LEVEL_NAME[];
    
    /**
     * Contains the names of the tracelevels with tailing spaces to ensure that every
     * tracelevel has exactly the same number of characters.
     */
    static QString TRACE_LEVEL_NAME_FIXED_LENGTH[];

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

private:
    /**
     * Private constructor. Use getInstance(...) to get a tracer instance!
     */
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
     * Sets the tracelevel of this tracer instance.
     * If recursive is set to true, the tracelevel of all children is adjusted too.
     * Only messages traced with a level higher or equal to this tracelevel are traced!
     * The default tracelevel is LEVEL_INFO.
     *
     * @param tracelevel the new tracelevel for this tracer instance.
     * @param recursive defines if the tracelevel is applied recusrively to all children (default is false).
     * @return true is the specified tracelevl was valid else false is returned.
     */
    bool setTraceLevel(QString tracelevel, bool recursive = false);
    
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

    /**
     * Prints the configuration of this tracer and calls this method on each child
     * with an increased indention. This leads to a tree representing the tracers.
     * The method is intended for debugging the tracer itself.
     * 
     * @indention the indention to put before the text.
     */
    void dump(QString indention = QString(""));
    
    
private:
    /**
     * Prepares the header for each trace-line: timestamp levelname [class::method] '
     */
    inline QString prepareMessageHeader(TraceLevel level, const char* method);
    
    /**
     * Returns the child tracer with the given tracerpartname or 0 if no such
     * child was found or the given tracerpartname is null or empty.
     *
     * @param tracerpartname the last part of the child tracer to return.
     * @return the appropriate child tracer or 0.
     */
    inline Tracer* getChild(QString tracerpartname);


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
