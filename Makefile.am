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
