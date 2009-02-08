# Makefile.am for GFingerPoken
# Copyright 2005-2009  Bas Wijnen <wijnen@debian.org>
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

pkgdatadir=$(datadir)/games/$(PACKAGE_TARNAME)

AUTOJUNK = INSTALL Makefile.in aclocal.m4 compile configure depcomp \
	install-sh missing

bin_PROGRAMS = gfpoken

gfpoken_CPPFLAGS = -DDATADIR=\"$(pkgdatadir)\"
gfpoken_CFLAGS = $(GTK2_CFLAGS)
# Don't use autoconf substitution here, because it would include
# implicit dependencies.
gfpoken_LDFLAGS = -lgtk-x11-2.0

gfpoken_SOURCES = \
	convert.c \
	dialog.c \
	history.c \
	interface.c \
	logic.c \
	main.c \
	multi.c \
	common.h \
	gfp.h \
	graphics.h \
	initial.h

man_MANS = gfpoken.6

tiles = png/00_bg.png \
	png/01_normal.png \
	png/02_normal.png \
	png/03_flip2.png \
	png/04_flip2.png \
	png/05_flip4.png \
	png/06_flip4.png \
	png/07_flip4.png \
	png/08_flip4.png \
	png/09_block.png \
	png/10_sink.png \
	png/11_axial.png \
	png/12_axial.png \
	png/13_axial2.png \
	png/14_axial2.png \
	png/15_rotator.png \
	png/16_rotator.png \
	png/17_rotator2.png \
	png/18_rotator2.png \
	png/19_half.png \
	png/20_half.png \
	png/21_half.png \
	png/22_half.png \
	png/23_half4.png \
	png/24_half4.png \
	png/25_half4.png \
	png/26_half4.png \
	png/27_half4.png \
	png/28_half4.png \
	png/29_half4.png \
	png/30_half4.png \
	png/31_move.png \
	png/32_move.png \
	png/gfpoken.png

marbles = png/marbles.png

$(tiles): tiles-stamp

tiles-stamp: art/mirror.xcf art/mktiles
	$(top_srcdir)/art/mktiles
	touch $@

$(marbles): marbles-stamp

marbles-stamp: art/marbles.blend art/mkmarbles
	$(top_srcdir)/art/mkmarbles || (cd png && uudecode < marbles.png.uu)
	touch $@

desktopdir = $(datadir)/applications
pngsdir = $(pkgdatadir)/png

desktop_DATA = gfpoken.desktop

install-data-local:
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	test -e $(DESTDIR)$(pngsdir)/gfpoken.png || ln -s $(pngsdir)/gfpoken.png $(DESTDIR)$(datadir)/pixmaps/

pngsources = \
	png/picBorder.png \
	png/picDDown.png \
	png/picDDownW.png \
	png/picDLeft.png \
	png/picDLeftW.png \
	png/picDRight.png \
	png/picDRightW.png \
	png/picDUp.png \
	png/picDUpW.png \
	png/picIAbout.png \
	png/picICheck.png \
	png/picIFlipBack.png \
	png/picIFlipBoards.png \
	png/picINet.png \
	png/picINetTurn.png \
	png/picINew.png \
	png/picIOpen.png \
	png/picISave.png \
	png/picIStop.png \
	png/picMkCircle.png \
	png/picMkSquare.png \
	png/picMkTriangle.png

pngs_DATA = \
	$(tiles) \
	$(marbles) \
	$(pngsources)

EXTRA_DIST = \
	autogen.sh \
	art/mirror.xcf \
	art/marbles.blend \
	art/mktiles \
	art/mkmarbles \
	art/README \
	png/README \
	png/marbles.png.uu \
	$(man_MANS) \
	$(pngsources) \
	$(desktop_DATA)

CLEANFILES = $(tiles) tiles-stamp $(marbles) marbles-stamp

autoclean: maintainer-clean
 ifneq (,$(wildcard $(AUTOJUNK)))
	rm $(wildcard $(AUTOJUNK))
 endif
