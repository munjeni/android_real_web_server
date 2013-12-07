# MAKEFILE for GCC created
# glob implementation for android by munjeni @ XDA 2013
CC=/root/arm-none-linux-gnueabi/arm-2011.03/bin/arm-none-linux-gnueabi-gcc
LD=/root/arm-none-linux-gnueabi/arm-2011.03/bin/arm-none-linux-gnueabi-ld
AR=/root/arm-none-linux-gnueabi/arm-2011.03/bin/arm-none-linux-gnueabi-ar

CFLAGS = -O2 -Wall -fpic -march=armv7-a -mfloat-abi=softfp -mfpu=vfp -static

default:kernel_make kernel_dump

kernel_make:
	${CC} ${CFLAGS} kernel_make.c -o kernel_make

kernel_dump:
	${CC} ${CFLAGS} system.c kernel_dump.c -o kernel_dump

clean:
	rm -f kernel_make kernel_dump *.o

distclean:
	rm -f kernel_make kernel_dump *.o

install:
	[ ! -d /androidserver/bin ] && mkdir -p /androidserver/bin 
	cp -f kernel_make /androidserver/bin/
	cp -f kernel_dump /androidserver/bin/
	chmod 755 /androidserver/bin/kernel_make
	chmod 755 /androidserver/bin/kernel_dump
