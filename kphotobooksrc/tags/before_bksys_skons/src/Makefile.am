## Makefile.am for kphotobook

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kphotobook

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# the library search path.
kphotobook_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
# ATTENTION: libtracer must be the last entry in this list!
kphotobook_LDADD = -lkmdi $(LIB_KFILE) $(LIB_KDEPRINT) backend/libbackend.la \
    dialogs/libdialogs.la engine/libengine.la export/libexport.la \
    settings/libsettings.la widgets/libwidgets.la \
    uitrees/libuitrees.la utils/libutils.la import/libimport.la \
    tracer/libtracer.la 

# which sources should be compiled for kphotobook
kphotobook_SOURCES = \
	constants.cpp \
	exception.cpp \
	main.cpp \
	kphotobook.cpp \
	kphotobookview.cpp

# these are the headers for your project
noinst_HEADERS = \
	constants.h \
	exception.h \
	kphotobook.h \
	kphotobookview.h

# client stuff


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kphotobook.pot

# this is where the kdelnk file will go
kdelnkdir = $(kde_appsdir)/Graphics
kdelnk_DATA = kphotobook.desktop

# install a mimelnk
mimedir = $(kde_mimedir)/application
mime_DATA = x-kphotobook.desktop
EXTRA_DIST = $(mime_DATA)

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/kphotobook
rc_DATA = kphotobookui.rc tips

# this is where the KConfigXT file goes
#kde_kcfgdir = $(kde_datadir)/kphotobook
#kde_kcfg_DATA = settings.kcfg

SUBDIRS = icons images backend widgets dialogs engine export settings uitrees utils tracer import
