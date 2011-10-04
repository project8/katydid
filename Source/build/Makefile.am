# $Id$
# Katydid/Source/build Makefile definition
# Author: Noah Oblath

KATYDID_INCLUDES = \
    -I@top_srcdir@\
	-I@top_srcdir@/Source/Egg


include_HEADERS = 


###########################################


lib_LTLIBRARIES = libKatydid.la

libKatydid_la_CPPFLAGS = $(KATYDID_INCLUDES) $(AM_CPPFLAGS)

libKatydid_la_LIBADD = \
    ../Egg/libEgg.la


libKatydid_la_SOURCES = 
# Dummy source to cause linking
nodist_EXTRA_libKatydid_la_SOURCES = dummy.cxx


###########################################

CLEANFILES = 

MAINTAINERCLEANFILES = Makefile.in
