all: tilepix.h marblepix.h

tilepix.h: mirror.xcf mktiles
	./mktiles
	cp $@ $(target)/

marblepix.h: marbles.blend mkmarbles
	./mkmarbles
	cp $@ $(target)/
