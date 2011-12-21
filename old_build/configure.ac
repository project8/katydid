#$Id$
#  File: configure.ac
#  Author: N. Oblath
#

#Autoconf version check.
#Required for AC_LANG.
#Tested at v. 2.64. Anything below may or may not work.
AC_PREREQ([2.60])


AC_INIT([Katydid], [0.50], [https://github.com/project8/katydid])

#automake version check.
#this is real. 1.11 is required for AM_SILENT_RULES
#no guarantee that it'll work though.

AM_INIT_AUTOMAKE([foreign 1.10 tar-ustar])
 
AC_CONFIG_MACRO_DIR([ac_macros])
AC_CONFIG_HEADERS([KatydidConfig.h])

#Make use of the AM_SILENT_RULES functionality in automake 1.11
#Defaults to quiet output. Verbose output can be enabled with
#"./configure --disable-silent-rules" or "make V=1"
#m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])], [AC_MSG_WARN([  "macro SILENT_RULES not found: upgrade autoconf to enjoy a less verbose build output!"])  ])

#check for libtool version. again, 2.0 is a random version. tested is 2.2.6b
m4_ifdef( [LT_PREREQ], [LT_PREREQ(2.0)])
m4_ifdef( [LT_INIT], [LT_INIT([shared])],  [AC_PROG_LIBTOOL] )

#Setup tools that will be used
#AC_PROG_LIBTOOL
AC_LANG(C++)
AC_PROG_CXX
AC_CANONICAL_HOST
AM_PROG_CC_C_O

AC_PROG_SED
AC_PROG_LN_S

PKG_PROG_PKG_CONFIG

#check for root and required features
ROOT_CHECK([5.24])
#ROOT_FEATURE([fftw3])
# Need to add -lFFTW because for some reason it doesn't get included from root-config
ROOT_LDFLAGS="$ROOT_LDFLAGS -lFFTW"

# Include for Mini-XML
MXML_LDFLAGS="-lmxml"

#set default prefix to build area
AC_PREFIX_DEFAULT([`pwd`])

# Define compiler and linker flags for automake
AM_CPPFLAGS="$DEFS $DEBUG_FLAGS $ROOT_CPPFLAGS" 
AM_CXXFLAGS="-ggdb -W -Wall $ROOT_CXXFLAGS"
AM_LDFLAGS="-ggdb -rdynamic -fPIC $ROOT_LDFLAGS $MXML_LDFLAGS"

AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_CXXFLAGS)
AC_SUBST(AM_LDFLAGS)


AC_CONFIG_FILES(
        [Makefile]
		[katydid.pc]
        [Source/Makefile]
        [Source/Egg/Makefile]
        [Source/FFT/Makefile]
        [Source/Main/Makefile]
        [Source/build/Makefile]
        [Scripts/Makefile]
		)

AC_OUTPUT
