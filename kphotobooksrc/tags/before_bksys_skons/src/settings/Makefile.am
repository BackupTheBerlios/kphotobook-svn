INCLUDES = $(all_includes)
METASOURCES = AUTO
libsettings_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libsettings.la

# command for generating the following list:
#  ls -1 *.cpp | sed -e "s/\(.*\).cpp/\1.h \1.cpp \\\\/"
#  ls -1 *.ui* | sed -e "s/\(.*\)/\1 \\\\/"
libsettings_la_SOURCES = settings.h settings.cpp 	settingsfilehandling.ui \
		settingsfilehandling.ui.h 	settingsgeneral.ui 	settingsimagepreview.ui 	settingsimagepreview.ui.h \
		settingssourcedirtree.ui 	settingstagtree.ui 	settingstools.ui settingsimageviewer.ui
