INCLUDES = -Icrypto/ -I.
CXXFLAGS = $(WX_CXXFLAGS)

EXTRA_DIST = hash.ico Hashish-1.1a-1.spec Makefile.rpm gnu_small_trans.xpm gnu_small_trans.bmp

bin_PROGRAMS = hashish
hashish_SOURCES = \
main.cpp \
hashish_wdr.cpp \
hashish_wdr.h\
crypto/algparam.cpp \
crypto/base64.cpp \
crypto/basecode.cpp \
crypto/crc.cpp \
crypto/cryptlib.cpp \
crypto/files.cpp \
crypto/filters.cpp \
crypto/haval.cpp \
crypto/hex.cpp \
crypto/iterhash.cpp \
crypto/md2.cpp \
crypto/md5.cpp \
crypto/misc.cpp \
crypto/mqueue.cpp \
crypto/panama.cpp \
crypto/pch.cpp \
crypto/ripemd.cpp \
crypto/sha.cpp \
crypto/strciphr.cpp \
crypto/tiger.cpp \
crypto/simple.cpp \
crypto/queue.cpp \
crypto/fips140.cpp \
crypto/trdlocal.cpp \
crypto/tigertab.cpp


hashish_LDADD = $(WX_LIBS)
