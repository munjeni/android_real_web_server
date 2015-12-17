#
CFLAGS = -O2 -Wall
CC=/compilationmoj/mipsel-unknown-linux-gnu/bin/mipsel-unknown-linux-gnu-gcc -O2 -Wall -fpic
LD=/compilationmoj/mipsel-unknown-linux-gnu/bin/mipsel-unknown-linux-gnu-ld
AR=/compilationmoj/mipsel-unknown-linux-gnu/bin/mipsel-unknown-linux-gnu-ar

CFLAGS += -fpic

default:libglob.a

libglob.a: glob.o
	${AR} rs libglob.a glob.o
	#${CC} -O2 -Wall -shared -o libglob.so -Wl,--whole-archive libglob.a -Wl,--no-whole-archive

clean:
	rm -f *.a *.o
	rm -f *.so

distclean:
	rm -f *.a *.o
	rm -f *.so

install:
	if [ ! -d "/hdd/server/include" ]; then mkdir -p /hdd/server/include; fi
	if [ ! -d /hdd/server/lib ]; then mkdir -p /hdd/server/lib; fi
	cp -f *.h /hdd/server/include/
	cp *.a /hdd/server/lib/
	#cp *.so /hdd/server/lib/

