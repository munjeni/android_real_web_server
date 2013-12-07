# MAKEFILE for GCC created
# glob implementation for android by munjeni @ XDA 2013
CFLAGS = -O2 -Wall -c -I./
CC=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-gcc
LD=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-ld
AR=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-ar

CFLAGS += -fpic -march=armv7-a -mfloat-abi=softfp -mfpu=vfp

default:libglob.a

libglob.a: glob.o
	${AR} rs libglob.a glob.o
	${CC} -O2 -Wall -shared -o libglob.so -Wl,--whole-archive libglob.a -Wl,--no-whole-archive

clean:
	rm -f *.a *.o
	rm -f *.so

distclean:
	rm -f *.a *.o
	rm -f *.so

install:
	[ ! -d /androidserver/include ] && mkdir -p /androidserver/include
	[ ! -d /androidserver/lib ] && mkdir -p /androidserver/lib 
	cp -f *.h /androidserver/include/
	cp *.a /androidserver/lib/
