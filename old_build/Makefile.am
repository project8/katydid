#$Id$
#  File: Katydid Makefile.am
#  Author: N. Oblath

SUBDIRS = Source Scripts

# The Scripts directory shouldn't be added here as a whole directory, 
# per the recommendation of the automake manual.  This should be changed in the future.
EXTRA_DIST = autogen.sh  README

ACLOCAL_AMFLAGS = -I ac_macros

# Install directory for kassiopeia.pc:
pkgconfigdir = @libdir@/pkgconfig
nodist_pkgconfig_DATA = katydid.pc

# Rootmap, so ROOT knows about the Katydid classes
rootmapdir = @libdir@
dist_rootmap_DATA = libkatydid.rootmap

# If .dylib shared-object libraries are created, make .so links to them because ROOT doesn't like .dylib
install-exec-hook:
	@top_srcdir@/Scripts/link_dylibs.sh $(libdir)


MAINTAINERCLEANFILES = Makefile.in \
	aclocal.m4 config.guess config.h.in config.sub configure \
	depcomp install-sh ltmain.sh missing compile *~


