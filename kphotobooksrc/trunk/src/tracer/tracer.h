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

#include <qdatetime.h>
#include <qptrlist.h>
#include <qstring.h>


/**
 * A simple and powerful enough tracer.
 * We just implemented the functions we are currently using. It is very likely that
 * we forgot a lot of useful methods...
 *
 * ATTENTION: a trace with tracelevel 'FATAL' will abort the application!
 * 
 * A message is traced only if its trace level is greater or equal to the tracelevel
 * of the used tracer instance.
 *
 * In order to use a Tracer in a class you have to get an instance of it first.
 * You get this instance by calling the static method getInstance(...).
 * We recommend to use tracernames reflecting the directory or namespace the tracer
 * is used in. The parts of the tracername must be separated with a dot (.).
 * 
 * <pre>
 *   static Tracer* tracer = Tracer::getInstance( "kde.kphotobook", "main" );
 * </pre>
 * If you use the same name twice or more you always get the same instance of the tracer.
 *
 * If you instantiate Tracers with the following parameters
 * <ul>
 *   <li>"kde.kphotobook.engine", "Engine"</li>
 *   <li>"kde.kphotobook.uitrees", "TagTree"</li>
 *   <li>"kde.kphotobook.uitrees", "TreeHelper"</li>
 * </ul>
 * a tree of Tracers will be created as shown below.
 * </p>
 * <pre>
 *                                  [root-Tracer]
 *                                       |
 *                                       |
 *                                     [kde]
 *                                       |
 *                                       |
 *                                   [kphotobook]
 *                                       /\
 *                                      /  \
 *                                     /    \
 *                                    /      \
 *                                   /        \
 *                               [engine]   [uitrees]
 *                                  |           /\
 *                                  |          /  \
 *                             [Engine]       /    \
 *                                           /      \
 *                                          /        \
 *                                      [TagTree] [TreeHelper]
 * </pre>
 * The big advantage of this tracer framework is that you can specify
 * very detailled which trace messages you want to see...
 * 
 * Initially all new Tracers in the tree inherit the configuration of their
 * parent. If you change the configuration of a Tracer, you can specify
 * that only this Tracer should be adjusted or the whole subtree below this
 * Tracer as well (recursive).
 *
 * There are two different ways to trace messages:
 * - via the vararg methods:
 *     tracer->debug(__func__, "The value %d is invalid!", myvalue)
 * - using streams
 *     tracer->sdebug(__func__) << "The value " << myvalue << " is invalid!" << endl;
 * We don't prefere one of these ways to trace. Both methods have advantages and
 * disadvantages. E.g. with the vararg method you cannot trace QStrings directly,
 * you must use the method ascii() method on the string first:
 *     tracer->debug(__func__, "The string %s is invalid!", mystring.ascii())
 *
 * There also exists an isXxxEnabled() method for each tracelevel. These methods can
 * be useful for improving performance! If you are tracing a debug message in a
 * method which is executed very often, this can be very time consuming even
 * the message is not traced at all. To avoid the execution of the code totally
 * you should execute the trace statement only if it would be traced:
 * if (tracer->isDebugEnabled()) {
 *     tracer->debug(__func__, "mymessage", myparameter)
 * }
 *
 * BTW: '__func__' is a preprocessor macro which will be replaced automatically
 * with the name of the actual function the macro.
 *
 * CVS-ID $Id: tagnode.h 299 2005-04-08 22:16:51Z starcube $
 */
class Tracer
{
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
        static Tracer* getRootTracer()
        {
            return s_rootTracer;
        }
    
    private:
        /**
        * Private constructor. Use getInstance(...) to get a tracer instance!
        */
        Tracer(const QString& tracername, const QString& tracerpartname, const QString& classname);
    
        
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
        bool setTraceLevel(const QString& tracelevel, bool recursive = false);
        
        /**
        * Returns the currently set tracelevel of this tracer instance.
        * Only messages traced with a level higher or equal to this tracelevel are traced!
        *
        * @return the currently set tracelevel.
        */
        TraceLevel getTraceLevel() const
        {
            return m_tracelevel;
        }
    
        bool isDebugEnabled() const
        {
            return LEVEL_DEBUG >= m_tracelevel;
        }
        bool isInvokedEnabled() const
        {
            return LEVEL_INVOKED >= m_tracelevel;
        }
        bool isInfoEnabled() const
        {
            return LEVEL_INFO >= m_tracelevel;
        }
        bool isWarningEnabled() const
        {
            return LEVEL_WARNING >= m_tracelevel;
        }
        bool isErrorEnabled() const
        {
            return LEVEL_ERROR >= m_tracelevel;
        }
        bool isFatalEnabled() const
        {
            return true;
        }
        bool isTraceEnabled(TraceLevel level) const
        {
            return level >= m_tracelevel;
        }
        
        void debug(const char* method, const char* message, ...) const;    
        void invoked(const char* method) const;
        void invoked(const char* method, const char* message, ...) const;    
        void info(const char* method, const char* message, ...) const;    
        void warning(const char* method, const char* message, ...) const;    
        void error(const char* method, const char* message, ...) const;    
        void fatal(const char* method, const char* message, ...) const;    
        void trace(TraceLevel level, const char* method, const char* message, ...) const;
    
        kdbgstream sdebug(const char* method) const;
        kdbgstream sinvoked(const char* method) const;
        kdbgstream sinfo(const char* method) const;
        kdbgstream swarning(const char* method) const;
        kdbgstream serror(const char* method) const;
        kdbgstream sfatal(const char* method) const;
        kdbgstream strace(TraceLevel level, const char* method) const;
    
        /**
        * Prints the configuration of this tracer and calls this method on each child
        * with an increased indention. This leads to a tree representing the tracers.
        * The method is intended for debugging the tracer itself.
        * 
        * @indention the indention to put before the text.
        */
        void dump(QString indention = QString("")) const;
        
        
    private:
        /**
        * Prepares the header for each trace-line: timestamp levelname [class::method] '
        */
        inline QString prepareMessageHeader(TraceLevel level, const char* method) const;
        
        /**
        * Returns the child tracer with the given tracerpartname or 0 if no such
        * child was found or the given tracerpartname is null or empty.
        *
        * @param tracerpartname the last part of the child tracer to return.
        * @return the appropriate child tracer or 0.
        */
        inline Tracer* getChild(const QString& tracerpartname) const;
    
    
    private:
        /**
        * The root tracer is the topmost tracer in the hierarchy of tracer instances.
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
