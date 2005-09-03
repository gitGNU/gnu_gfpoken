# Makefile.am for GFingerPoken
# Copyright 2005  Bas Wijnen <shevek@fmf.nl>
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
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

bin_PROGRAMS = gfpoken

AM_CFLAGS = @GTK_CFLAGS@

LDADD = @GTK_LIBS@

gfpoken_SOURCES = \
	convert.c \
	dialog.c \
	history.c \
	interface.c \
	logic.c \
	main.c \
	multi.c \
	arrowpix.h \
	barpix.h \
	borderpix.h \
	common.h \
	gfp.h \
	graphics.h \
	iconpix.h \
	initial.h \
	markpix.h

main.c: tilepix.h marblepix.h

tilepix.h: art/mirror.xcf art/mktiles
	art/mktiles

marblepix.h: art/marbles.blend art/mkmarbles
	art/mkmarbles

EXTRA_DIST = 
