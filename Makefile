# shared lib c++ flags
CXXFLAGS = `wx-config --cxxflags` -Isrc/  \
	-I. -I.. -fexceptions

# static lib c++ flags
WXWIN = /usr/home/mentat/gm/wxWindows
CXXFLAGS_S = -I$(WXWIN)/src/png -I$(WXWIN)/src/jpeg -I$(WXWIN)/src/tiff \
	-I/usr/X11R6/include/gtk12 -I/usr/local/include/glib12 -I/usr/local/include \
	-I/usr/X11R6/include `wx-config --cxxflags` -Isrc/ -I. -I.. -fexceptions

# Makefile expects the cryptopp dir with libcrypto.a
# to be one level below the Hashish dir in 'crypto'

# Makefile also expects an install of wxWindows from
# the 2.3.2 source

LIBS_STATIC = `wx-config --libs --static` -L../crypto -lcryptopp \
 	-L/usr/X11R6/lib -L/usr/X11R6/lib -L/usr/lib -lgtk12  \
	-lgdk12 -L/usr/local/lib -Wl,-E -lgmodule12 -lgthread12 -lglib12 -pthread \
	-lintl -lxpg4 -lXext -lX11 -lm -lz -lc_r -lm -lcryptopp -lstdc++ -static


# add -static if you are building a static bin
LBLIBS = `wx-config --libs`  -L../crypto \
              -lcryptopp
all: hashish

static: src/main.o src/hashish_wdr.o
	g++ -o hashish $(CXXFLAGS_S) src/main.o src/hashish_wdr.o $(LIBS_STATIC)

main.o: src/main.cpp
	g++ -c src/main.cpp $(CXXFLAGS)

hashish_wdr.o: src/hashish_wdr.cpp src/hashish_wdr.h
	g++ -c src/hashish_wdr.cpp $(CXXFLAGS)

hashish: src/main.o src/hashish_wdr.o
	g++ -o hashish $(CXXFLAGS) src/main.o src/hashish_wdr.o $(LBLIBS)

clean:
	rm -f src/*.o src/*.d hashish

