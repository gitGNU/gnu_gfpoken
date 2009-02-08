# configure.ac for GFingerPoken
# Copyright 1999-2000  Martin Hock
# Copyright 2005-2009 Bas Wijnen <wijnen@debian.org>
#
# This file is part of Gfingerpoken.
#
# Gfingerpoken is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Gfingerpoken is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)
AC_INIT(gfpoken,0.32,[[wijnen@debian.org]])
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

dnl Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

dnl Checks for libraries.
PKG_CHECK_MODULES(GTK2, gtk+-2.0,,[AC_MSG_ERROR(Cannot build without Gtk 2)])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_CHECK_FUNCS(socket strtol)

AC_CONFIG_FILES(Makefile)

AC_OUTPUT
