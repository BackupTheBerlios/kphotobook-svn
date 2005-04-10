INCLUDES = $(all_includes)
METASOURCES = AUTO
libengine_la_LDFLAGS = $(all_libraries) 
noinst_LTLIBRARIES = libengine.la
libengine_la_SOURCES = engine.cpp engine.h file.cpp file.h \
			filetagnodeassoc.cpp	filetagnodeassoc.h filetagnodeassocboolean.cpp filetagnodeassocboolean.h	sourcedir.cpp \
			sourcedir.h tagnode.cpp tagnode.h tagnodeboolean.cpp	tagnodeboolean.h \
			tagnodetitle.cpp tagnodetitle.h tagnodestring.cpp tagnodestring.h \
			filetagnodeassocstring.cpp filetagnodeassocstring.h tagnoderadiogroup.cpp tagnoderadiogroup.h \
	tagnoderadio.cpp tagnoderadio.h filetagnodeassocradio.cpp filetagnodeassocradio.h
libengine_la_LIBADD = $(top_builddir)/src/backend/libbackend.la
