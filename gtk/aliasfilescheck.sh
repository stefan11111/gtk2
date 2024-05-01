#! /bin/sh

if test "x$gtk_all_c_sources" = x; then
	echo gtk_all_c_sources variable not defined
	exit 1
fi

grep 'IN_FILE' ${srcdir-.}/gtk.symbols | sed 's/.*(//;s/).*//' | grep __ | sort -u > expected-files
{ for f in $gtk_all_c_sources; do if test -f ${f}; then grep '^ *# *define __' ${f}; else grep '^ *# *define __' "${srcdir-.}"/${f}; fi; done } | sed 's/.*define //;s/ *$//' | sort > actual-files

diff expected-files actual-files && rm -f expected-files actual-files
