INCLUDES = $(all_includes)
METASOURCES = AUTO
libdialogs_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libdialogs.la

# command for generating the following list:
#  ls -1 *.cpp | sed -e "s/\(.*\).cpp/\1.h \1.cpp \\\\/"
libdialogs_la_SOURCES = \
	dialogaddsourcedir.h dialogaddsourcedir.cpp \
	dialogdatetimechooser.h dialogdatetimechooser.cpp \
	dialogdatetimefilter.h dialogdatetimefilter.cpp \
	dialogfilesystemscanner.h dialogfilesystemscanner.cpp \
	dialogimageviewer.h dialogimageviewer.cpp \
    dialogmanagetag.h dialogmanagetag.cpp

libdialogs_la_LIBADD = \
	$(top_builddir)/src/widgets/libwidgets.la
