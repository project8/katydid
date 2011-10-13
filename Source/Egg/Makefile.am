# $Id$
# Katydid/Source/Egg Makefile definition
# Author: Noah Oblath

EGG_INCLUDES =  \
    -I@top_srcdir@/Source/Egg

EGG_DICT_INCLUDES=\
	$(ROOT_CXXFLAGS)\
    -I@top_srcdir@/Source/Egg

# headers for classes that should go into the root dictionary
EGG_DICT_HEADERFILES = \
    KTArrayUC.hh\
    KTEgg.hh\
    KTEvent.hh

# headers for classes that should not go into the root dictionary (e.g. if a class uses boost)
EGG_NODICT_HEADERFILES = 

EGG_HEADERFILES = \
    $(EGG_DICT_HEADERFILES)\
    $(EGG_NODICT_HEADERFILES)

EGG_SOURCEFILES = \
    KTArrayUC.cc\
    KTEgg.cc\
    KTEvent.cc


EGG_LINKDEF_HEADERFILES = \
    LinkDef/EggLinkDef.hh

include_HEADERS = \
    $(EGG_HEADERFILES)

noinst_HEADERS = $(EGG_LINKDEF_HEADERFILES)

###########################################

noinst_LTLIBRARIES = libEgg.la

libEgg_la_CPPFLAGS = $(EGG_INCLUDES) $(AM_CPPFLAGS)

libEgg_la_SOURCES = \
    $(EGG_SOURCEFILES)

nodist_libEgg_la_SOURCES = \
    EggDict.cxx

EggDict.cxx : $(EGG_DICT_HEADERFILES)   $(EGG_LINKDEF_HEADERFILES)
	$(ROOTCINT) -f $@ -c $(AM_CXXFLAGS) $(EGG_DICT_INCLUDES) $+

###########################################

CLEANFILES = \
   EggDict.cxx\
   EggDict.h

MAINTAINERCLEANFILES = Makefile.in
