## Makefile.am for kphotobook

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kphotobook

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# the library search path.
kphotobook_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
kphotobook_LDADD   = $(LIB_KFILE) $(LIB_KDEPRINT)

# which sources should be compiled for kphotobook
kphotobook_SOURCES = main.cpp kphotobook.cpp kphotobookview.cpp pref.cpp engine.cpp tagtree.cpp tagtreenode.cpp exception.cpp configuration.cpp xmlwriter.cpp xmlparser.cpp dialogcreatetag.cpp dialogaddsourcedir.cpp constants.cpp xmlconstants.cpp dialogedittag.cpp sourcedir.cpp file.cpp tagnode.cpp tagnodetitle.cpp tagnodeboolean.cpp filetagnodeassoc.cpp filetagnodeassocboolean.cpp tagtreenodetitle.cpp tagtreenodesourcedir.cpp tagtreenodeboolean.cpp sourcedirtree.cpp

# these are the headers for your project
noinst_HEADERS = kphotobook.h kphotobookview.h pref.h engine.h tagtree.h tagtreenode.h constants.h exception.h configuration.h xmlwriter.h xmlparser.h dialogcreatetag.h dialogaddsourcedir.h xmlconstants.h dialogedittag.h sourcedir.h file.h tagnode.h tagnodetitle.h tagnodeboolean.h filetagnodeassoc.h filetagnodeassocboolean.h tagtreenodetitle.h tagtreenodesourcedir.h tagtreenodeboolean.h sourcedirtree.h

# client stuff


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kphotobook.pot

# this is where the kdelnk file will go
kdelnkdir = $(kde_appsdir)/Graphics
kdelnk_DATA = kphotobook.desktop

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/kphotobook
rc_DATA = kphotobookui.rc

SUBDIRS =  icons
