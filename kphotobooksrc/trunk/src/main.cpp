/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#include "kphotobook.h"
#include "settings/settings.h"

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

static const char version[] = "0.0.6";

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
  about.addAuthor("Michael Christen", I18N_NOOP("The master chief developer."), "starcube@users.sourceforge.net" );
  about.addAuthor("Thomas Christen", I18N_NOOP("One of Santa's greater helpers.\nHelps me creating icons and writing the website and the documentation."), "chtom@users.sourceforge.net");
  about.addAuthor("Stefan Fink", I18N_NOOP("One of Santa's little helpers.\nAdvises in design and usability questions.\n(He is also responsible for the KPhotoBook icon...)"));
  about.addAuthor("Daniel Gerber", I18N_NOOP("One of Santa's little helpers.\nAdvises in design and usability questions."));

  about.addCredit("George W. Bush, President of the USA", I18N_NOOP("For being a stupid little git."), "idiot@whitehouse.org" );

  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions(options);

  KApplication app;

  KMdi::MdiMode mdiMode = KMdi::IDEAlMode;
  if (Settings::generalViewMode() == Settings::EnumGeneralViewMode::TabPageMode) {
      mdiMode = KMdi::TabPageMode;
  }

  KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
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
    int i = 0;
    for (; i < args->count(); i++) {
      QFileInfo file(args->url(i).path());
      KPhotoBook* widget = new KPhotoBook(mdiMode);
      widget->show();
      widget->load(file);
    }
  }
  args->clear();

  return app.exec();
}
