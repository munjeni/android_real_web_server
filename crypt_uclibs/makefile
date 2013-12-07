# MAKEFILE for GCC
CFLAGS = -O2 -Wall -c -I./
CC=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp -fPIC
LD=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-ld
AR=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-ar

default:libcrypt.a

libcrypt.a: crypt.o des.o md5.o
	${AR} rs libcrypt.a crypt.o des.o md5.o
#	${CC} -O2 -Wall -shared -o libcrypt.so -Wl,--whole-archive libcrypt.a -Wl,--no-whole-archive

clean:
	rm -f *.a *.o
#	rm -rf *.so
