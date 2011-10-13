# $Id$
# Katydid/Source/Main makefile definition
# Author: N. Oblath

MAIN_CPPFLAGS = $(AM_CPPFLAGS) \
	-I@top_srcdir@/Source/Egg\
	-I@top_srcdir@/Source/FFT

#---------------------------

bin_PROGRAMS = ExtractEvents
ExtractEvents_SOURCES = ExtractEvents.cc
ExtractEvents_CPPFLAGS = $(MAIN_CPPFLAGS)

ExtractEvents_LDADD = \
    @top_builddir@/Source/build/libKatydid.la

ExtractEvents_LDFLAGS = $(AM_LDFLAGS)

#---------------------------

bin_PROGRAMS += SimpleElectronHunt
SimpleElectronHunt_SOURCES = SimpleElectronHunt.cc
SimpleElectronHunt_CPPFLAGS = $(MAIN_CPPFLAGS)

SimpleElectronHunt_LDADD = \
	@top_builddir@/Source/build/libKatydid.la
	
SimpleElectronHunt_LDFLAGS = $(AM_LDFLAGS)

#---------------------------

# example for adding another executable
#bin_PROGRAMS += FileReader
#FileReader_SOURCES = FileReader.cc
#FileReader_CPPFLAGS = $(MAIN_CPPFLAGS)

#FileReader_LDADD = \
#	@top_builddir@/Source/build/libKatydid.la

#FileReader_LDFLAGS = $(AM_LDFLAGS)

#---------------------------

MAINTAINERCLEANFILES = Makefile.in
