CXXFLAGS = `wx-config --cxxflags` -Isrc/  \
	-I. -I.. -fexceptions

# Makefile expects the cryptopp dir with libcrypto.a
# to be one level below the Hashish dir in 'crypto'

# Makefile also expects an install of wxWindows from
# the 2.3.2 source


# add -static if you are building a static bin
LBLIBS = `wx-config --libs`  -L../crypto \
              -lcryptopp
all: hashish

main.o: src/main.cpp
	g++ -c src/main.cpp $(CXXFLAGS)

hashish_wdr.o: src/hashish_wdr.cpp src/hashish_wdr.h
	g++ -c src/hashish_wdr.cpp $(CXXFLAGS)

hashish: src/main.o src/hashish_wdr.o
	g++ -o hashish $(CXXFLAGS) src/main.o src/hashish_wdr.o $(LBLIBS)

clean:
	rm -f src/*.o src/*.d hashish

