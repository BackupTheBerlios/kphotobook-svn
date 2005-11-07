INCLUDES = $(all_includes)
METASOURCES = AUTO
libbackend_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libbackend.la

# command for generating the following list:
#  ls -1 *.cpp | sed -e "s/\(.*\).cpp/\1.h \1.cpp \\\\/"
libbackend_la_SOURCES = \
	xmlconstants.h xmlconstants.cpp \
	xmlparser.h xmlparser.cpp \
	xmlwriter.h xmlwriter.cpp
