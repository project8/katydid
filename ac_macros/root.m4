dnl -*- mode: autoconf -*- 
dnl
dnl $Id$
dnl Author: Sebastian Voecking <sebastian.voecking@uni-muenster.de>
dnl
dnl Autoconf macros to check for existence or ROOT on the system
dnl Synopsis:
dnl
dnl  ROOT_CHECK([MINIMUM-VERSION])
dnl
dnl The macro defines the following substitution variables
dnl
dnl    ROOTCINT
dnl    ROOT_CXXFLAGS
dnl    ROOT_CPPFLAGS
dnl    ROOT_LDFLAGS
dnl
dnl The macro will fail if root-config and rootcint isn't found.
dnl
dnl  ROOT_FEATURE(feature)
dnl
dnl The macros checks whether ROOT is compiled with a specific feature.
dnl

AC_DEFUN([ROOT_CHECK],
[AC_ARG_WITH(rootsys,
    [AS_HELP_STRING([--with-rootsys],[top of the ROOT installation directory])],    
    user_rootsys=$withval,
    user_rootsys="none"
)
if test ! x"$user_rootsys" = xnone ; 
then
  rootbin="$user_rootsys/bin"
elif test ! x"$ROOTSYS" = x ;
then 
  rootbin="$ROOTSYS/bin"
else 
  rootbin=$PATH
fi

AC_PATH_PROG(ROOTCONF,root-config,no,$rootbin)
AC_PATH_PROG(ROOTEXEC,root,no,$rootbin)
AC_PATH_PROG(ROOTCINT,rootcint,no,$rootbin)

if test ! x"$ROOTCONF" = "xno" && test ! x"$ROOTCINT" = "xno" ;
then

  ROOT_CXXFLAGS=`$ROOTCONF --auxcflags`
  ROOT_CPPFLAGS=`$ROOTCONF --noauxcflags --cflags`
  ROOT_LDFLAGS=`$ROOTCONF --ldflags --libs --auxlibs`

  if test $1;
  then 

    AC_MSG_CHECKING(whether ROOT version >= [$1])
    version=`$ROOTCONF --version`
    vers=`echo $version | tr './' ' ' | awk 'BEGIN { FS = " "; } { printf "%d", ($''1 * 1000 + $''2) * 1000 + $''3;}'`
    requ=`echo $1 | tr './' ' ' | awk 'BEGIN { FS = " "; } { printf "%d", ($''1 * 1000 + $''2) * 1000 + $''3;}'`
    AC_MSG_RESULT($version)

    if test $vers -lt $requ;
    then 
      AC_MSG_ERROR([ROOT installation is too old])
    fi
  fi

else
  AC_MSG_RESULT(no)
  AC_MSG_ERROR([cannot find ROOT installation])
fi

AC_SUBST(ROOTCINT)
AC_SUBST(ROOT_CXXFLAGS)
AC_SUBST(ROOT_CPPFLAGS)
AC_SUBST(ROOT_LDFLAGS)])

AC_DEFUN([ROOT_FEATURE],
[AC_MSG_CHECKING([whether ROOT is built with $1])
if test `$ROOTCONF --has-$1` = "yes";
then
  AC_MSG_RESULT(yes)
else
 AC_MSG_RESULT(no)
 AC_MSG_ERROR([ROOT must be built with $1])
fi])
