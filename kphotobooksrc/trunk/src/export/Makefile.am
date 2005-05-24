INCLUDES = $(all_includes)
METASOURCES = AUTO
libexport_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libexport.la

# command for generating the following list:
#  ls -1 *.cpp | sed -e "s/\(.*\).cpp/\1.h \1.cpp \\\\/"
libexport_la_SOURCES = \
	exportsymlinks.h exportsymlinks.cpp
