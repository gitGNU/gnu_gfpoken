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
# Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

bin_PROGRAMS = gfpoken

AM_CFLAGS = @GTK_CFLAGS@ `imlib-config --cflags-gdk`

LDADD = @GTK_LIBS@

AM_CPPFLAGS = -DDATADIR=\"$(pkgdatadir)\"

gfpoken_LDFLAGS = `imlib-config --libs-gdk`

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
	png/32_move.png

marbles = png/marbles.png

$(tiles) png/gfpoken.png: art/mirror.xcf art/mktiles
	art/mktiles

$(marbles): art/marbles.blend art/mkmarbles
	art/mkmarbles

appicondir = $(datadir)/pixmaps
gamesdir = $(datadir)/gnome/apps/Games
pngsdir = $(pkgdatadir)/png

appicon_DATA = png/gfpoken.png

games_DATA = gfpoken.desktop

pngs_DATA = $(tiles) $(marbles) $(appicon_DATA) \
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

EXTRA_DIST = \
	AUTHORS \
	COPYING \
	INSTALL \
	NEWS \
	README \
	ChangeLog \
	art/mirror.xcf \
	art/marbles.blend \
	art/mktiles \
	art/mkmarbles \
	art/README \
	$(appicon_DATA) \
	$(games_DATA) \
	$(pngs_DATA) \
	$(appicon_DATA)

CLEANFILES = $(tiles) $(marbles) $(appicon_DATA)
