/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#include "kphotobook.h"
#include "configuration.h"

#include <qfileinfo.h>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdebug.h>

static const char description[] = I18N_NOOP(
                                      "KPhotoBook is a sophisticated and highly optimized KDE\n"
                                      "application for managing and refinding your digital photos."
                                  );

static const char version[] = "0.0.2";

static KCmdLineOptions options[] = {
                                       { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
                                       KCmdLineLastOption
                                   };

int main(int argc, char** argv) {

    KAboutData about("kphotobook", I18N_NOOP("KPhotoBook"), version, description,
                     KAboutData::License_GPL, "(C) 2003 Michael Christen",
                     0, // free text, can contain new lines
                     "http://kphotobook.sourceforge.net",
                     "starcube@users.sourceforge.net");
    about.addAuthor("Michael Christen", "The master chief developer.", "starcube@users.sourceforge.net" );
    about.addAuthor("Thomas Christen", "One of Santa's greater helpers.\nHelps me creating icons and writing the website and the documentation.", "chtom@users.sourceforge.net");
    about.addAuthor("Stefan Fink", "One of Santa's little helpers.\nAdvises in design and usability questions.");
    about.addAuthor("Daniel Gerber", "One of Santa's little helpers.\nAdvises in design and usability questions.");

    about.addCredit("George W. Bush, President of the USA", "For being a stupid little git.", "idiot@whitehouse.org" );

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    Configuration::getInstance()->load();

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    if (args->count() == 0) {

        KPhotoBook* widget = new KPhotoBook();
        widget->show();

        // try to load last opened file
        QString lastFileName = Configuration::getInstance()->lastOpenedFile();
        if (!lastFileName.isEmpty()) {
            QFileInfo lastFile(lastFileName);
            widget->load(lastFile);
        }
    } else {
        int i = 0;
        for (; i < args->count(); i++) {
            QFileInfo file(args->url(i).path());
            KPhotoBook* widget = new KPhotoBook();
            widget->show();
            widget->load(file);
        }
    }
    args->clear();

    return app.exec();
}
