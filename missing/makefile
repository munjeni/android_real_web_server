#
CFLAGS = -O2 -Wall
CC=/compilationmoj/mipsel-unknown-linux-gnu/bin/mipsel-unknown-linux-gnu-gcc -O2 -Wall -fpic -lc
LD=/compilationmoj/mipsel-unknown-linux-gnu/bin/mipsel-unknown-linux-gnu-ld
AR=/compilationmoj/mipsel-unknown-linux-gnu/bin/mipsel-unknown-linux-gnu-ar

CFLAGS += -fpic

default:libmissing.a

libmissing.a: strlcat.o strlcpy.o clock_gettime.o fgetln.o
	${AR} rs libmissing.a strlcat.o strlcpy.o clock_gettime.o fgetln.o
#	${CC} -O2 -Wall -shared -o libgmissing.so -Wl,--whole-archive libmissing.a -Wl,--no-whole-archive

clean:
	rm -f *.a *.o
	rm -f *.so

distclean:
	rm -f *.a *.o
	rm -f *.so

install:
	if [ ! -d /hdd/server/include ]; then mkdir -p /hdd/server/include; fi
	if [ ! -d /hdd/server/lib ]; then mkdir -p /hdd/server/lib; fi
	cp -f *.h /hdd/server/include/
	cp *.a /hdd/server/lib/
#	cp *.so /hdd/server/lib/
