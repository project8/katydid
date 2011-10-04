#!/bin/bash

# link_dylibs.sh
# Author: N. Oblath
#
# If shared libraries with .dylib extensions are created, creates symbolic links with .so extensions
#
# Expects one argument:
#  1 - The directory in which the libraries are located

cd $1

for file_dylib in $( ls *.dylib ); do
    basename=`basename "$file_dylib" .dylib`
    file_so="${basename}.so"
#    if [ ! -e $file_so ]
#    then
	echo "Creating a symbolic link: ${file_so} --> ${file_so}"
	ln -sf $file_dylib $file_so
#    fi
done
