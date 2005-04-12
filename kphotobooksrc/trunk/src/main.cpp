/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#include "kphotobook.h"
#include "settings/settings.h"
#include "tracer/tracer.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include <qfileinfo.h>


static Tracer* tracer = Tracer::getInstance("kde.kphotobook", "");

static const char description[] = I18N_NOOP(
  "KPhotoBook is a sophisticated and highly optimized KDE\n"
  "application for managing and refinding your digital photos."
);

static const char version[] = "0.0.7";

static KCmdLineOptions options[] = {
    { "tracelevel <level>", I18N_NOOP("The tracelevel to use."
                                      "\nPossible values: DEBUG, INVOKED, INFO, WARNING, ERROR, FATAL")
                          , "INFO" },
    { "traceconfig <file>", I18N_NOOP("The configfile must contain lines with the following format:"
                                      "\ntracername=tracelevel"
                                      "\nComment lines mut begin with a hash (#)"
                                      "\nTo set the tracelevel recursively (for the whole subtree), there must be a * before the =."
                                      "\nATTENTION: The tracernames are case sensitive!")
                          , 0 },
    { "+[URL]", I18N_NOOP("Document to open.")
              , 0 },
    KCmdLineLastOption
};


void configTracer(KCmdLineArgs *args) {

    // apply the tracelevel set by the user or the default tracelevel
    QCString tracelevel = args->getOption("tracelevel");
    if (!Tracer::getRootTracer()->setTraceLevel(tracelevel, true)) {
        tracer->sfatal("configTracer") << "Invalid tracelevel specified: '" << tracelevel << "'." << endl;
    }

    // apply the traceconfiguration from the specified traceconfig file
    if (args->isSet("traceconfig")) {
        QCString traceconfigFileName = args->getOption("traceconfig");

        QFile file(traceconfigFileName);
        if (!file.exists()) {
            tracer->sfatal("configTracer") << "The specified tracer config file does not exist: '" << traceconfigFileName << "'." << endl;
        }
        
        if ( file.open( IO_ReadOnly ) ) {
            QTextStream stream( &file );
            QString line;
            while ( !stream.atEnd() ) {
                QString line = stream.readLine();

                // remove leading and trailing spaces
                line = line.stripWhiteSpace();

                // ignore line if it seems to be a comment
                if (line.startsWith("#")) {
                    continue;
                }

                // ignore empty lines
                if (line.isEmpty()) {
                    continue;
                }

                // there must be exactly 1 equal sign (=)
                if (line.contains('=') != 1) {
                    tracer->sfatal("configTracer") << "Configline does not contain exactly 1 equal sign (=): <" << line << ">" << endl;
                }

                // split the line
                int pos = line.find('=');
                QString tracername = line.left(pos).stripWhiteSpace();
                QString tracelevel = line.mid(pos + 1).stripWhiteSpace();

                bool recursive = false;
                if (tracername.endsWith("*")) {
                    tracername = tracername.left(tracername.length() - 1).stripWhiteSpace();
                    recursive = true;
                }

                Tracer::getInstance(tracername, "")->setTraceLevel(tracelevel, recursive);
            }
            file.close();
        }
    }
}


int main(int argc, char** argv) {

    KAboutData about("kphotobook", I18N_NOOP("KPhotoBook"), version, description,
                    KAboutData::License_GPL, "(C) 2003, 2004, 2005 Michael Christen",
                    0, // free text, can contain new lines
                    "http://kphotobook.berlios.det",
                    "starcube@users.berlios.de");
    about.addAuthor("Michael Christen", I18N_NOOP("The master chief developer."), "starcube@users.berlios.de" );
    about.addAuthor("Christian Hoenig", I18N_NOOP("The loyal coder of kphotobook :-)."), "choenig@users.berlios.de" );
    about.addAuthor("Thomas Christen", I18N_NOOP("One of Santa's greater helpers.\nHelps me creating icons and writing the website and the documentation."), "chtom@users.berlios.de");
    about.addAuthor("Stefan Fink", I18N_NOOP("One of Santa's little helpers.\nAdvises in design and usability questions.\n(He is also responsible for the KPhotoBook icon...)"));
    about.addAuthor("Daniel Gerber", I18N_NOOP("One of Santa's little helpers.\nAdvises in design and usability questions."));
    
    about.addCredit("George W. Bush, President of the USA", I18N_NOOP("For being a stupid little git."), "idiot@whitehouse.org" );

    // initialize command line options
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);

    // parse command line options
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    // configure the tracer with the command line options
    configTracer(args);

    // create the application instance
    KApplication app;

    // determine the MDI Model to use (default is IDEAl)
    KMdi::MdiMode mdiMode = KMdi::IDEAlMode;
    if (Settings::generalViewMode() == Settings::EnumGeneralViewMode::TabPageMode) {
        mdiMode = KMdi::TabPageMode;
    }
    
    if (args->count() == 0) {

        KPhotoBook* widget = new KPhotoBook(mdiMode);
        widget->show();
    
        // try to load last opened file
        QString lastFileName = Settings::fileSystemLastOpenedFile();
        if (!lastFileName.isEmpty()) {
            QFileInfo lastFile(lastFileName);
            widget->load(lastFile);
        }
    } else {
        for (int i = 0; i < args->count(); i++) {
            QFileInfo file(args->url(i).path());
            KPhotoBook* widget = new KPhotoBook(mdiMode);
            widget->show();
            widget->load(file);
        }
    }
    args->clear();

    int returncode = app.exec();

    Tracer::getRootTracer()->dump();
    
    return returncode;
}
