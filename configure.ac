# configure.ac for GFingerPoken
# Copyright 1999-2000  Martin Hock
# Copyright 2005 Bas Wijnen <shevek@fmf.nl>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)
AC_INIT(gfpoken,0.28,[[shevek@fmf.nl]])
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

dnl Checks for programs.
AC_PROG_CC

dnl Checks for libraries.
AM_PATH_GTK(1.2.0,,[AC_MSG_ERROR(Cannot build without Gtk)])
AM_PATH_GDK_IMLIB(,,[AC_MSG_ERROR(Cannot build without Gdk-Imlib)])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_CHECK_FUNCS(socket strtol)

AC_CONFIG_FILES(Makefile)

AC_OUTPUT
