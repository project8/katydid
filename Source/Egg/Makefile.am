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
    KTComplexVector.hh\
    KTEgg.hh\
    KTEvent.hh\
    KTEventWindowFunction.hh\
    KTFFT.hh\
    KTHammingWindow.hh\
    KTHannWindow.hh\
    KTPowerSpectrum.hh\
    KTRectangularWindow.hh\
    KTSimpleFFT.hh\
    KTSlidingWindowFFT.hh\
    KTWindowFunction.hh

# headers for classes that should not go into the root dictionary (e.g. if a class uses boost)
EGG_NODICT_HEADERFILES = 

EGG_HEADERFILES = \
    $(EGG_DICT_HEADERFILES)\
    $(EGG_NODICT_HEADERFILES)

EGG_SOURCEFILES = \
    KTArrayUC.cc\
    KTComplexVector.cc\
    KTEgg.cc\
    KTEvent.cc\
    KTEventWindowFunction.cc\
    KTFFT.cc\
    KTHammingWindow.cc\
    KTHannWindow.cc\
    KTPowerSpectrum.cc\
    KTRectangularWindow.cc\
    KTSimpleFFT.cc\
    KTSlidingWindowFFT.cc\
    KTWindowFunction.cc


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
