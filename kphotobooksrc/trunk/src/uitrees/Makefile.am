INCLUDES = $(all_includes)
METASOURCES = AUTO
libuitrees_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libuitrees.la

# command for generating the following list:
#  ls -1 *.cpp | sed -e "s/\(.*\).cpp/\1.h \1.cpp \\\\/"
libuitrees_la_SOURCES = \
	sourcedirtree.h sourcedirtree.cpp \
	sourcedirtreenode.h sourcedirtreenode.cpp \
	tagtree.h tagtree.cpp \
	tagtreenode.h tagtreenode.cpp \
	tagtreenodeboolean.h tagtreenodeboolean.cpp \
	tagtreenodedatetime.h tagtreenodedatetime.cpp \
	tagtreenoderadio.h tagtreenoderadio.cpp \
	tagtreenoderadiogroup.h tagtreenoderadiogroup.cpp \
	tagtreenodestring.h tagtreenodestring.cpp \
	tagtreenodetitle.h tagtreenodetitle.cpp \
	treehelper.h treehelper.cpp

libuitrees_la_LIBADD = $(top_builddir)/src/engine/libengine.la

