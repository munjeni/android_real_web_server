# MAKEFILE for GCC
CFLAGS = -O2 -Wall -c -I./
CC=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp -fPIC
LD=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-ld
AR=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-ar

default:libcrypt.a

libcrypt.a: base64.o rsa_sys.o rsa.o yarrow.o ctr.o cbc.o hash.o tiger.o sha1.o md5.o sha256.o serpent.o safer+.o rc6.o rc5.o blowfish.o crypt.o mpi.o prime.o
	${AR} rs libcrypt.a base64.o rsa_sys.o rsa.o yarrow.o ctr.o cbc.o hash.o tiger.o sha1.o md5.o sha256.o serpent.o safer+.o rc6.o rc5.o blowfish.o crypt.o mpi.o prime.o
#	${CC} -O2 -Wall -shared -o libcrypt.so -Wl,--whole-archive libcrypt.a -Wl,--no-whole-archive

test.exe: libcrypt.a test.o
	gcc test.o libcrypt.a -o test.exe

clean:
	rm -f *.a *.o *.exe *.log *.aux *.dvi *.toc *.idx *.ilg
#	rm -f *.so
