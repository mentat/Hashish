INCLUDES = $(WXFLAGS) -Icrypto -I.

EXTRA_DIST = hash.ico Hashish-1.1a-1.spec Makefile.rpm gnu_small_trans.xpm gnu_small_trans.bmp

bin_PROGRAMS = hashish
hashish_SOURCES = \
main.cpp \
hashish_wdr.cpp \
hashish_wdr.h\
crypto/base64.cpp\
crypto/crc.cpp\
crypto/cryptlib.cpp\
crypto/files.cpp\
crypto/haval.cpp\
crypto/hex.cpp\
crypto/md2.cpp\
crypto/md5.cpp\
crypto/misc.cpp\
crypto/panama.cpp\
crypto/ripemd.cpp\
crypto/sha.cpp\
crypto/tiger.cpp

hashish_LDLIBS = $(WXLIBS)
