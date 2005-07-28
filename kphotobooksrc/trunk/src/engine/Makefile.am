INCLUDES = $(all_includes)
METASOURCES = AUTO
libengine_la_LDFLAGS = $(all_libraries) 
noinst_LTLIBRARIES = libengine.la

# command for generating the following list:
#  ls -1 *.cpp | sed -e "s/\(.*\).cpp/\1.h \1.cpp \\\\/"
libengine_la_SOURCES = \
	engine.h engine.cpp \
	file.h file.cpp \
	filesystemscanner.h filesystemscanner.cpp \
	filetagnodeassoc.h filetagnodeassoc.cpp \
	filetagnodeassocboolean.h filetagnodeassocboolean.cpp \
	filetagnodeassocdatetime.h filetagnodeassocdatetime.cpp \
	filetagnodeassocradio.h filetagnodeassocradio.cpp \
	filetagnodeassocstring.h filetagnodeassocstring.cpp \
	filternode.h filternode.cpp \
	filternodeopand.h filternodeopand.cpp \
	filternodeopnegate.h filternodeopnegate.cpp \
	filternodeopor.h filternodeopor.cpp \
	filternodetagboolean.h filternodetagboolean.cpp \
	filternodetagdatetime.h filternodetagdatetime.cpp \
	filternodetagradio.h filternodetagradio.cpp \
	filternodetagstring.h filternodetagstring.cpp \
	sourcedir.h sourcedir.cpp \
	tagnode.h tagnode.cpp \
	tagnodeboolean.h tagnodeboolean.cpp \
	tagnodedatetime.h tagnodedatetime.cpp \
	tagnoderadio.h tagnoderadio.cpp \
	tagnoderadiogroup.h tagnoderadiogroup.cpp \
	tagnodestring.h tagnodestring.cpp \
	tagnodetitle.h tagnodetitle.cpp

libengine_la_LIBADD = $(top_builddir)/src/backend/libbackend.la -lexif
