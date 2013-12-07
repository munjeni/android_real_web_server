# MAKEFILE for GCC
CFLAGS = -O2 -Wall -c -I./
CC=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp -fPIC
LD=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-ld
AR=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-ar

default:libgetpass.a

libgetpass.a: getpass.o
	${AR} rs libgetpass.a getpass.o

clean:
	rm -f *.a *.o

