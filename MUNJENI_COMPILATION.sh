#!/bin/bash

red='\e[0;31m'
RED='\e[1;31m'
blue='\e[0;34m'
BLUE='\e[1;34m'
cyan='\e[0;36m'
CYAN='\e[1;36m'
NC='\e[0m' # No Color

OLDPATH=$PATH

TOOLCHAIN=/compilation/mipsel-unknown-linux-gnu
PLATFORM=$TOOLCHAIN/mipsel-unknown-linux-gnu/sys-root
export PATH=$PATH:$TOOLCHAIN/bin

# just to save this function :)
#for TT in `ls /hdd/server/bin`; do mv /hdd/server/bin/$TT /hdd/server/bin/`echo $TT | awk '{gsub("arm-", ""); print $1 }'`; done
#$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-objcopy --localize-symbol=__dtoa /compilation/backup/libcrystax.a
#
#6) cd lib/bind
#7) gcc-3.4 -pthread -shared -Wl,-soname,libresolv.so.2 */*.o -o libresolv.so

rm -rf /hdd
echo "" > CONFIGURE.log

#####################################################################

mkdir -p /hdd/server/lib
cd /hdd/server/lib
#tar xzf /compilation/lib.tar.gz
cd /compilation

#####################################################################
echo ""
echo -e "${RED}COMPILING LIBMISSING${NC}"
echo ""
sleep 3
cd missing
for TT in `find . -type f -name *.o`; do rm $TT; done
echo -e "libmissing\n\n" >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
make install
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING GLOB${NC}"
echo ""
sleep 3
cd glob
for TT in `find . -type f -name *.o`; do rm $TT; done
echo -e "glob\n\n" >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
make install
make distclean
cd ..
################################################################
echo ""
echo -e "${RED}COMPILING PERL-5.16.3${NC}"
echo ""
sleep 3
cd perl-5.16.3
for TT in `find . -type f -name *.o`; do rm $TT; done
rm -rf cpan/Time-HiRes/xdefine
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc" RANLIB="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib" AR="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar" LD="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld" STRIP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip" CXX="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++" ASCPP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as" OBJDUMP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-objdump" ./configure --target=mipsel-unknown-linux-gnu --sysroot=$PLATFORM -d -Dprefix=/hdd/server -A ccflags=-DUSE_HASH_SEED_EXPLICIT ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "perl\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
cd ..
# after installing, perl is broken on my host machine, so fix for them is re-linking old perl again
rm -rf /usr/bin/perl
ln -s /usr/bin/perl5.20.2 /usr/bin/perl
#####################################################################
echo ""
echo -e "${RED}COMPILING NCURSES-5.7${NC}"
echo ""
sleep 3
cd ncurses-5.7
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib" AR="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar" LD="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld" STRIP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip" CXX="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++" ASCPP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as" ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --without-cxx-binding --disable-big-core --disable-big-strings --disable-leaks --enable-expanded --disable-largefile CFLAGS="--sysroot=$PLATFORM" program_prefix="" ac_cv_func_getttynam=no ac_cv_lib_util_openpty=no cf_cv_func_openpty=no ac_cv_func_sigvec=no cf_cv_dcl_errno=yes cf_cv_have_errno=yes cf_cv_need_libm=yes ac_cv_header_locale_h=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "ncurses\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
ln -s /hdd/server/lib/libncurses.a /hdd/server/lib/libcurses.a
#####################################################################
echo ""
echo -e "${RED}COMPILING ZLIB${NC}"
echo ""
sleep 3
cd zlib
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -static -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as ./configure --prefix=/hdd/server --static ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "zlib\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING BZIP2-1.0.6${NC}"
echo ""
sleep 3
cd bzip2-1.0.6
for TT in `find . -type f -name *.o`; do rm $TT; done
echo -e "bzip2\n\n" >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install PREFIX=/hdd/server ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
################################################################
echo ""
echo -e "${RED}COMPILING TERMCAP${NC}"
echo ""
sleep 3
cd termcap-1.3.1
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as ./configure --prefix=/hdd/server --target=mipsel --host=mipsel program_prefix="" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "termcap\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#################################################################
echo ""
echo -e "${RED}COMPILING PCRE${NC}"
echo ""
sleep 3
cd pcre-8.33
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as ./configure --prefix=/hdd/server --enable-static --disable-shared --target=mipsel-unknown-linux --host=mipsel-unknown-linux --enable-utf8 --enable-unicode-properties --enable-pcregrep-libz --enable-rebuild-chartables program_prefix="" --disable-pcretest-libreadline --enable-pcregrep-libbz2 LDFLAGS="-L/hdd/server/lib -lz -lbz2" CFLAGS="-I/hdd/server/include" --with-sysroot="/compilation/mipsel-unknown-linux-gnu/mipsel-unknown-linux-gnu/sys-root" program_prefix="" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "pcre\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#################################################################
echo ""
echo -e "${RED}COMPILING MHASH${NC}"
echo ""
sleep 3
cd mhash-0.9.9.9
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as CFLAGS="--sysroot=$PLATFORM" ./configure --prefix=/hdd/server --enable-static --disable-shared --target=mipsel-unknown-linux --host=mipsel-unknown-linux --with-CC=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc program_prefix="" ac_cv_func_signal=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "mhash\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
##################################################################
echo ""
echo -e "${RED}COMPILING LIBMCRYPT${NC}"
echo ""
sleep 3
cd libmcrypt-2.5.8
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as CFLAGS="-I/hdd/server/include" ./configure --prefix=/hdd/server --enable-static --disable-shared --target=mipsel-unknown-linux --host=mipsel-unknown-linux program_prefix="" ac_cv_func_shl_load=no ac_cv_lib_dld_shl_load=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "libmcrypt\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
####################################################################
echo ""
echo -e "${RED}COMPILING ICONV${NC}"
echo ""
sleep 3
cd libiconv-1.14
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" \
RANLIB="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib" \
AR="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar" \
LD="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld" \
STRIP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip" \
CXX="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++" \
ASCPP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as" \
./configure --prefix=/hdd/server \
--enable-extra-encodings \
--enable-static \
--disable-shared \
--target=mipsel \
--host=mipsel \
gl_cv_header_working_stdint_h=yes \
program_prefix="" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "iconv\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING EXPAT${NC}"
echo ""
sleep 3
cd expat-2.1.0
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as ./configure --prefix=/hdd/server --enable-static --disable-shared --target=mipsel-unknown-linux --host=mipsel-unknown-linux program_prefix="" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "expat\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
##################################################################
echo ""
echo -e "${RED}COMPILING LIBMBFL${NC}"
echo ""
sleep 3
cd libmbfl-1.2.0
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as ./configure --prefix=/hdd/server --enable-static --disable-shared --target=mipsel-unknown-linux --host=mipsel-unknown-linux program_prefix="" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "libmbfl\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING OPENSSL${NC}"
echo ""
sleep 3
cd openssl
for TT in `find . -type f -name *.o`; do rm $TT; done
#compiling static openssl
#export PATH=$PATH:/root/mipsel-unknown-linux-gnueabi/arm-2011.03/bin
#./Configure linux-generic32 -DL_ENDIAN --prefix=/hdd/server no-shared -static -fPIC
#make CC="mipsel-unknown-linux-gnueabi-gcc" RANLIB=mipsel-unknown-linux-gnueabi-ranlib
#configuring imap
#make slx CC=/root/mipsel-unknown-linux-gnueabi/arm-2011.03/bin/mipsel-unknown-linux-gnueabi-gcc RANLIB=/root/mipsel-unknown-linux-gnueabi/arm-2011.03/bin/mipsel-unknown-linux-gnueabi-ranlib AR=/root/mipsel-unknown-linux-gnueabi/arm-2011.03/bin/mipsel-unknown-linux-gnueabi-ar LD=/root/mipsel-unknown-linux-gnueabi/arm-2011.03/bin/mipsel-unknown-linux-gnueabi-ld STRIP=/root/mipsel-unknown-linux-gnueabi/arm-2011.03/bin/mipsel-unknown-linux-gnueabi-strip CXX=/root/mipsel-unknown-linux-gnueabi/arm-2011.03/bin/mipsel-unknown-linux-gnueabi-c++ ASCPP=/root/mipsel-unknown-linux-gnueabi/arm-2011.03/bin/mipsel-unknown-linux-gnueabi-as EXTRACFLAGS="-fPIC -I/hdd/server/include/openssl -I/hdd/server/include" SSLDIR=/hdd/server/ssl SSLLIB="/hdd/server/lib" EXTRALDFLAGS="-lssl -lcrypto"
#cp c-client/*.a /hdd/server/lib/
#cp c-client/*.h /hdd/server/include/
if ! ./Configure linux-generic32 --prefix=/hdd/server no-shared -fPIC ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "openssl\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fPIC" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib ; then
    echo "Failure!!!"
    exit 1
fi
if ! make CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib install_sw ; then
    echo "Failure installing!!!"
    exit 1
fi
make clean
cd ..
###################################################################
echo ""
echo -e "${RED}COMPILING CRYPT${NC}"
echo ""
sleep 3
cd crypt_0_03
for TT in `find . -type f -name *.o`; do rm $TT; done
make clean
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
cp libcrypt.a /hdd/server/lib/
cp -fr *.h /hdd/server/include/
make clean
echo -e "crypt\n\n" >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
cd ..
# next step was:
### commented base64_encode and base64_decode in /hdd/server/include/crypt.h !!! ### 
### commented (#define  MP_NEG  1) and (#define  NEG     MP_NEG) in /hdd/server/include/mpi.h ###
cp -fr mpi.h /hdd/server/include/
cp -fr crypt.h /hdd/server/include/
####################################################################
echo ""
echo -e "${RED}COMPILING OSCAM${NC}"
echo ""
cd oscam-svn
for TT in `find . -type f -name *.o`; do rm $TT; done
sleep 3
if ! make CROSS=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu- USE_LIBCRYPTO=1 LIBCRYPTO_LIB="/hdd/server/lib/libcrypto.a" USE_SSL=1 SSL_LIB="/hdd/server/lib/libssl.a" EXTRA_CFLAGS="-I/hdd/server/include" ; then
    echo "Failure!!!"
    exit 1
fi
cp -fr Distribution/`ls Distribution | grep "oscam" | sed 's/.debug//g' | head -1` /hdd/server/bin/oscam
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING LIBXML2${NC}"
echo ""
sleep 3
cd libxml2
for TT in `find . -type f -name *.o`; do rm $TT; done
./autogen.sh
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as ./configure --target=mipsel-unknown-linux --host=mipsel-unknown-linux --prefix=/hdd/server --without-iconv --without-python --enable-static --disable-shared --without-readline --with-zlib=/hdd/server/lib program_prefix="" CFLAGS="-I/hdd/server/iinclude" LDFLAGS="-L/hdd/server/lib" LIBS="-lz -liconv" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "libxml\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
###################################################################
echo ""
echo -e "${RED}COMPILING BIND${NC}"
echo ""
sleep 3
cd bind-9.4.1
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as BUILD_CC=gcc LDFLAGS="-L/hdd/server/lib" LIBS="-lmissing" ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --enable-static --disable-shared --with-openssl=/hdd/server --with-randomdev=/dev/urandom --with-libiconv=/hdd/server --enable-libbind ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "bind\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
mkdir -p /hdd/server/bind/include/sys
echo "" >/hdd/server/bind/include/sys/bitypes.h
ln -s /hdd/server/lib/libbind.a /hdd/server/lib/libresolv.a
#/compilation/mipsel-unknown-linux-gnu/bin/mipsel-unknown-linux-gnu-objcopy --localize-symbol=MD5_Init /hdd/server/lib/libresolv.a
#/compilation/mipsel-unknown-linux-gnu/bin/mipsel-unknown-linux-gnu-objcopy --localize-symbol=MD5_Final /hdd/server/lib/libresolv.a
#/compilation/mipsel-unknown-linux-gnu/bin/mipsel-unknown-linux-gnu-objcopy --localize-symbol=MD5_Update /hdd/server/lib/libresolv.a
#/compilation/mipsel-unknown-linux-gnu/bin/mipsel-unknown-linux-gnu-objcopy --localize-symbol=MD5_version /hdd/server/lib/libresolv.a
##########################################################
echo ""
echo -e "${RED}COMPILING MYSQL${NC}"
echo ""
sleep 3
cd mysql-5.1.51
for TT in `find . -type f -name *.o`; do rm $TT; done
for TT in `find . -type f -name *.lo`; do rm $TT; done
for TT in `find . -type d -name *.libs`; do rm -rf $TT; done
for TT in `find . -type d -name *.deps`; do rm -rf $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc" RANLIB="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib" AR="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar" LD="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld" STRIP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip" CXX="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++" ASCPP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as" CC_FOR_BUILD="gcc" CFLAGS="-I/hdd/server/include -O3 -fPIC -D_GNU_SOURCE" CXXFLAGS="-I/hdd/server/include -O3 -fPIC -D_GNU_SOURCE" LDFLAGS="-L/hdd/server/lib" LIBS="-lncurses -lz -lssl -lcrypto -lmissing" ./configure --prefix=/hdd/server --target=mipsel --host=mipsel --with-lib-ccflags=-fPIC --disable-shared --enable-static --with-ssl --without-docs --without-man --with-readline --enable-community-features --enable-local-infile --with-mysqld-user=root --with-big-tables --with-plugins=partition,blackhole,federated,heap,innodb_plugin --with-named-curses-libs="/hdd/server/lib/libncurses.a" ac_cv_sys_restartable_syscalls=y with_named_curses=y mysql_cv_termcap_lib=y ac_cv_search_crypt='-lcrypt -lcrypto' ac_cv_c_bigendian=no ac_cv_func_gethostbyaddr_r=no program_prefix="" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "mysql\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
##################################################################
echo ""
echo -e "${RED}COMPILING APR${NC}"
echo ""
sleep 3
cd apr
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as CC_FOR_BUILD=gcc LDFLAGS="-L/hdd/server/lib" LIBS="-lcrypt -lcrypto" CFLAGS="-I/hdd/server/include" ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --enable-static --disable-shared --with-crypto program_prefix="" ac_cv_file__dev_zero=yes ac_cv_func_setpgrp_void=yes apr_cv_process_shared_works=yes apr_cv_mutex_robust_shared=yes apr_cv_tcp_nodelay_with_cork=yes ac_cv_search_crypt="-lcrypt -lcrypto" ac_cv_search_modf="-lm" ac_cv_func_gethostbyaddr_r=no ac_cv_func_kqueue=no ac_cv_func_port_create=no ac_cv_func_getpwnam_r=no ac_cv_func_getpwuid_r=no ac_cv_func_getgrnam_r=no ac_cv_func_getgrgid_r=no ac_cv_func_shm_open=no ac_cv_search_shm_open=no ac_cv_func_getservbyname_r=no ac_cv_func_shm_unlink=no ac_cv_func_shmget=no ac_cv_func_shmat=no ac_cv_func_shmdt=no ac_cv_func_shmctl=no ac_cv_func_create_area=no ac_cv_func_isinf=no ac_cv_func_getifaddrs=no ac_cv_func_sendfilev=no ac_cv_lib_sendfile_sendfilev=no ac_cv_func__getch=no ac_cv_func_getpass=no ac_cv_func_getpassphrase=no ac_cv_func_strnicmp=no ac_cv_func_stricmp=no ac_cv_func_semget=no ac_cv_func_semctl=no ac_cv_func_create_sem=no ac_cv_func_uuid_create=no ac_cv_search_uuid_create=no ac_cv_func_uuid_generate=no ac_cv_search_uuid_generate=no ac_cv_func_send_file=no ac_cv_func_set_h_errno=no ac_cv_sizeof_pid_t=4 ac_cv_func_nl_langinfo=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "apr\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
###################################################################
echo ""
echo -e "${RED}COMPILING APR-UTIL${NC}"
echo ""
sleep 3
mkdir -p /root/x-tools
ln -s /compilation/mipsel-unknown-linux-gnu /root/x-tools/mipsel-unknown-linux-gnu
cd apr-util-1.5.2
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as CC_FOR_BUILD=gcc ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --without-iconv --with-expat=/hdd/server --with-mysql=/hdd/server --with-openssl="/hdd/server" --with-crypto="/hdd/server" --with-apr=/hdd/server/bin/apr-1-config CFLAGS="-I/hdd/server/include" LDFLAGS="-L/hdd/server/lib" LIBS="-lssl -lcrypto -liconv -lcrypt" LIBTOOL="/hdd/server/build-1/libtool" program_prefix="" ac_cv_search_crypt="-lcrypt -lcrypto" ac_cv_func_nl_langinfo=no ac_cv_header_langinfo_h=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "apr-util\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
mipsel-unknown-linux-gnu-gcc -O2 -Wall -shared -o /hdd/server/lib/apr-util-1/apr_crypto_openssl.so -Wl,--whole-archive /hdd/server/lib/apr-util-1/apr_crypto_openssl.a -Wl,--no-whole-archive
mipsel-unknown-linux-gnu-gcc -O2 -Wall -shared -o /hdd/server/lib/apr-util-1/apr_dbd_mysql.so -Wl,--whole-archive /hdd/server/lib/apr-util-1/apr_dbd_mysql.a -Wl,--no-whole-archive
ln -s /hdd/server/lib/apr-util-1/apr_crypto_openssl.so /hdd/server/lib/apr-util-1/apr_crypto_openssl-1.so
ln -s /hdd/server/lib/apr-util-1/apr_dbd_mysql.so /hdd/server/lib/apr-util-1/apr_dbd_mysql-1.so
rm -rf /root/x-tools
###################################################################
echo ""
echo -e "${RED}COMPILING LIBJPEG-8d${NC}"
echo ""
sleep 3
cd jpeg-8d
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --disable-shared --enable-static --enable-maxmem=20 program_prefix="" LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "libjpeg\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING FREETYPE-2.5.0.1${NC}"
echo ""
sleep 3
cd freetype-2.5.0.1
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as \
./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --disable-shared --enable-static --without-zlib --without-bzip2 --without-png LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "freetype\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING FONTCONFIG-2.10.2${NC}"
echo ""
sleep 3
cd fontconfig-2.10.2
cp -fr ../system/fonts /hdd/server/fonts
for TT in `find . -type f -name *.o`; do rm $TT; done
cp /compilation/ft2build.h /hdd/server/include/
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --disable-shared --enable-static --enable-iconv --disable-libxml2 --disable-docs --with-arch=mipsel --with-libiconv=/hdd/server/bin --with-libiconv-includes=/hdd/server/include --with-libiconv-lib=/hdd/server/lib --with-expat --with-expat-includes=/hdd/server/include --with-expat-lib=/hdd/server/lib --with-default-fonts=/hdd/server/fonts --with-add-fonts=/hdd/server/fonts FREETYPE_CFLAGS="-I/hdd/server/include/freetype2" FREETYPE_LIBS="-L/hdd/server/lib -lfreetype" CFLAGS="-I/hdd/server/include" LIBTOOL=/hdd/server/build-1/libtool ac_cv_func__doprnt=no ac_cv_func_getpagesize=no ac_cv_func_chsize=no ac_cv_func_fstatvfs=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "fontconfig\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
/compilation/qemu-mipsel -L /compilation/mipsel-unknown-linux-gnu/mipsel-unknown-linux-gnu/sys-root /hdd/server/bin/fc-cache
#####################################################################
echo ""
echo -e "${RED}COMPILING LIBPNG-1.6.3${NC}"
echo ""
sleep 3
cd libpng-1.6.3
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CPPFLAGS="-I/hdd/server/include" LDFLAGS="-L/hdd/server/lib -lm" CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --disable-shared --enable-static --with-zlib-prefix="/hdd/server" LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "libpng\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING TIFF-4.0.3${NC}"
echo ""
sleep 3
cd tiff-4.0.3
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as CFLAGS="-I/hdd/server/include" LIBTOOL=/hdd/server/build-1/libtool ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --disable-shared --enable-static --without-x --disable-jbig --disable-lzma --with-zlib-include-dir=/hdd/server/include --with-zlib-lib-dir=/hdd/server/lib --with-jpeg-lib-dir=/hdd/server/lib ac_cv_func_setmode=no ac_cv_func_lfind=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "tiff\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING LIBGD-2.1.0${NC}"
echo ""
sleep 3
cd libgd-2.1.0
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as LDFLAGS="-L/hdd/server/lib" LIBS="-liconv" LIBTOOL=/hdd/server/build-1/libtool ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --disable-shared --enable-static --without-x --without-libiconv-prefix --with-zlib=/hdd/server/lib --with-jpeg=/hdd/server/lib --without-xpm --without-vpx CFLAGS="-I/hdd/server/include" LIBPNG_LIBS="-L/hdd/server/lib -lpng16" LIBPNG_CFLAGS="-I/hdd/server/include/libpng16" LIBFONTCONFIG_LIBS="-L/hdd/server/lib -lfontconfig" LIBFONTCONFIG_CFLAGS="-I/hdd/server/include/fontconfig" LIBTIFF_LIBS="-L/hdd/server/lib -ltiff" LIBTIFF_CFLAGS="-I/hdd/server/include" ac_cv_func_sin=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "libgd\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING GETTEXT-0.18.3${NC}"
echo ""
sleep 3
cd gettext-0.18.3
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as CFLAGS="-I/hdd/server/include" LDFLAGS="-L/hdd/server/lib" LIBTOOL=/hdd/server/build-1/libtool ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --enable-static --disable-shared --with-libpth-prefix --disable-java --disable-native-java --disable-threads --disable-c++ --enable-relocatable --disable-curses --disable-acl --disable-openmp --without-bzip2 --without-xz --with-libiconv-prefix=/hdd/server ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "gettext\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING GNU MP 5.1.2${NC}"
echo ""
sleep 3
cd gmp-5.1.2
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc -fpic" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as CFLAGS="--sysroot=$PLATFORM" LIBTOOL=/hdd/server/build-1/libtool CFLAGS="-I/hdd/server/include" LDFLAGS="-L/hdd/server/lib" LIBS="-lmissing" ./configure --prefix=/hdd/server --target=mipsel-unknown-linux --host=mipsel-unknown-linux --enable-static --disable-shared --without-readline --disable-cxx ac_cv_func_attr_get=no ac_cv_func_cputime=no ac_cv_func_getpagesize=no ac_cv_func_getsysinfo=no ac_cv_func_localeconv=no ac_cv_func_nl_langinfo=no ac_cv_func_obstack_vprintf=no ac_cv_func_processor_info=no ac_cv_func_pstat_getprocessor=no ac_cv_func_sigstack=no ac_cv_func_read_real_time=no ac_cv_func_syssgi=no ac_cv_header_sys_syssgi_h=no ac_cv_func_sysctl=no ac_cv_func_sysctlbyname=no ac_cv_header_sys_sysctl_h=no gmp_cv_m4_m4wrap_spurious=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "gmp\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
###################################################################
echo ""
echo -e "${RED}COMPILING APACHE2${NC}"
echo ""
sleep 3
cd httpd-2.4.17
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc" \
RANLIB="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib" \
AR="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar" \
LD="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld" \
STRIP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip" \
CXX="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++" \
ASCPP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as" \
CFLAGS="-D_GNU_SOURCE -DBIG_SECURITY_HOLE" \
CXXFLAGS="-D_GNU_SOURCE -DBIG_SECURITY_HOLE" \
CC_FOR_BUILD=gcc \
./configure --prefix=/hdd/server \
--target=mipsel-unknown-linux \
--host=mipsel-unknown-linux \
--enable-load-all-modules \
--enable-mime-magic \
--enable-expires \
--enable-unique-id \
--enable-usertrack \
--enable-remoteip \
--enable-session \
--enable-session-cookie \
--enable-ssl \
--enable-ssl-staticlib-deps \
--with-ssl=/hdd/server \
--with-crypto \
--enable-dav \
--enable-info \
--enable-cgi \
--enable-negotiation \
--enable-rewrite \
--with-z=/hdd/server \
--with-pcre=/hdd/server \
--with-openssl=/hdd/server \
--with-libxml2=/hdd/server \
--with-apr=/hdd/server/bin \
--with-apr-util=/hdd/server/bin \
--enable-auth-digest \
--enable-deflate \
--enable-logio \
--with-mpm=prefork \
--enable-static \
--enable-mods-static=all \
--disable-shared \
--enable-so \
--disable-lbmethod-heartbeat \
--disable-ext-filter \
--disable-heartbeat \
--disable-heartmonitor \
--enable-proxy \
--disable-slotmem-plain \
--enable-suexec \
--with-suexec-bin=/hdd/server/bin/suexec \
--with-suexec-caller=root \
ac_cv_file__dev_zero=yes \
ac_cv_func_setpgrp_void=yes \
apr_cv_process_shared_works=yes \
apr_cv_mutex_robust_shared=yes \
apr_cv_tcp_nodelay_with_cork=yes \
ap_cv_void_ptr_lt_long=no \
ac_cv_struct_rlimit=yes \
ac_cv_func_timegm=no \
ac_cv_func_fopen64=no \
ac_cv_func_getloadavg=no \
ac_cv_func_kqueue=no \
ac_cv_func_port_create=no \
ac_cv_func_bindprocessor=no \
ac_cv_lib_m_sqrt=yes \
ac_cv_search_sqrt=-lm \
ac_cv_search_crypt="-lcrypt -lcrypto" \
program_prefix="" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "apache2\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/hdd/server/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install LIBTOOL="/hdd/server/build-1/libtool" ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
cp -fr envvars /hdd/server/bin/
#####################################################################
echo ""
echo -e "${RED}COMPILING C-CLIENT (IMAP)${NC}"
echo ""
sleep 3
cd imap-2007f
for TT in `find . -type f -name *.o`; do rm $TT; done
echo -e "imap\n\n" >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
make clean
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc" RANLIB="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib" AR="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar" LD="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld" STRIP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip" CXX="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++" ASCPP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as" OBJDUMP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-objdump" EXTRACFLAGS="-fPIC -I/hdd/server/include" SSLDIR="/hdd/server/ssl" SSLLIB="/hdd/server/lib" EXTRALDFLAGS="-L/hdd/server/lib -lcrypt -lssl -lcrypto" make lnx ; then
    echo "Failure!!!"
    exit 1
fi
cp -fr c-client/*.a /hdd/server/lib/
cp --remove-destination c-client/*.h /hdd/server/include/
mkdir -p /hdd/server/usr
cp -fr dmail/dmail /hdd/server/bin/
mkdir -p /hdd/server/docs/imap
cp -fr docs /hdd/server/docs/imap/
cp -fr imapd/imapd /hdd/server/bin/
cp -fr ipopd/ipop2d /hdd/server/bin/
cp -fr ipopd/ipop3d /hdd/server/bin/
cp -fr mailutil/mailutil /hdd/server/bin/
cp -fr mlock/mlock /hdd/server/sbin/
cp -fr mtest/mtest /hdd/server/sbin/
cp -fr tmail/tmail /hdd/server/bin/
cp -fr dmail/dmail /hdd/server/bin/
mkdir -p /hdd/server/usr/spool/mail
mkdir -p /hdd/server/usr/spool/news
mkdir -p /hdd/server/usr/ucb/rsh
mkdir -p /hdd/server/var/lib/news
mkdir -p /hdd/server/var/spool
make clean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING PHP-5.5.1${NC}"
echo ""
sleep 3
mkdir -p /root/x-tools
ln -s /compilation/mipsel-unknown-linux-gnu /root/x-tools/mipsel-unknown-linux-gnu
cd php-5.5.1
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc" \
RANLIB="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib" \
AR="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar" \
LD="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld" \
STRIP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip" \
CXX="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++" \
ASCPP="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as" \
CFLAGS="-I/hdd/server/bind/include -I/hdd/server/include" \
CXXFLAGS="-I/hdd/server/bind/include -I/hdd/server/include" \
./configure --prefix=/hdd/server \
--target=mipsel-unknown-linux \
--host=mipsel-unknown-linux \
--with-apxs2=/hdd/server/bin/apxs \
--enable-sigchild \
--enable-sockets \
--enable-wddx \
--enable-cgi \
--with-libxml-dir=/hdd/server \
--with-libexpat-dir=/hdd/server \
--enable-zip \
--with-zlib-dir=/hdd/server \
--with-pear \
--enable-zend-signals \
--with-pic \
--enable-libxml \
--with-libxml-dir=/hdd/server \
--enable-simplexml \
--with-openssl=/hdd/server \
--with-pcre-regex=/hdd/server \
--with-zlib \
--with-zlib-dir=/hdd/server \
--with-bz2=/hdd/server \
--enable-ctype \
--enable-bcmath \
--enable-mbstring \
--without-mm \
--with-libmbfl=/hdd/server \
--with-mcrypt=/hdd/server \
--with-mysql=/hdd/server \
--with-mysqli=/hdd/server/bin/mysql_config \
--with-pdo-mysql=/hdd/server \
--with-libexpat-dir=/hdd/server \
--without-readline \
--with-iconv=/hdd/server \
--with-pcre-dir=/hdd/server \
--enable-opcache=no \
--enable-shared \
--with-mhash=/hdd/server \
--enable-ftp \
--with-config-file-path=/hdd/server/conf \
--with-config-file-scan-dir=/hdd/server/conf/php \
--disable-phar \
--with-gd=/hdd/server \
--with-jpeg-dir=/hdd/server \
--with-png-dir=/hdd/server \
--with-zlib-dir=/hdd/server \
--with-freetype-dir=/hdd/server \
--enable-gd-native-ttf \
--enable-gd-jis-conv \
--with-gettext=/hdd/server \
--with-gmp=/hdd/server \
--enable-exif \
--with-imap=/hdd/server \
--with-imap-ssl=/hdd/server \
ac_cv_lib_gd_gdImageCreateFromXpm=no \
ac_cv_lib_ssl_SSL_CTX_set_ssl_version=yes \
ac_cv_lib_crypt_crypt=yes \
ac_cv_crypt_SHA256=yes \
ac_cv_crypt_SHA512=yes \
ac_cv_crypt_blowfish=yes \
ac_cv_crypt_md5=yes \
ac_cv_func_crypt=yes \
program_prefix="" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "php\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
rm -rf /root/x-tools
#####################################################################
echo ""
echo -e "${RED}COMPILING lynx${NC}"
echo ""
sleep 3
cd lynx2-8-8
for TT in `find . -type f -name *.o`; do rm $TT; done
if ! CC="$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-gcc" RANLIB=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ranlib AR=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ar LD=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-ld STRIP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-strip CXX=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-c++ ASCPP=$TOOLCHAIN/bin/mipsel-unknown-linux-gnu-as CFLAGS="--sysroot=$PLATFORM" CFLAGS="-I/hdd/server/include" LDFLAGS="-L/hdd/server/lib" LIBS="-lmissing -lcrypt -lssl -lcrypto -lz -lbz2 -liconv" ./configure --prefix=/hdd/server/lynx --target=mipsel-unknown-linux --host=mipsel-unknown-linux --enable-static --disable-shared --with-libiconv=/hdd/server/lib/libiconv.a --with-libiconv-prefix=/hdd/server --with-ssl=/hdd/server/lib --enable-local-docs --with-bzlib --with-zlib --without-x ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "lynx 2.8.8\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
if ! make install ; then
    echo "Failure installing!!!"
    exit 1
fi
make distclean
cd ..
cp -fr /compilation/lynx /hdd/server/bin/
mkdir -p /hdd/server/lynx/cache
mv /hdd/server/lynx/bin/mipsel-unknown-linux-lynx /hdd/server/lynx/lynx
rm -rf /hdd/server/lynx/bin
###################################################################
echo ""
echo -e "${RED}COPYING CONFIGURATIONS, INSTALLING NANO, TERMINFO${NC}"
echo ""
sleep 3
cp -fr /compilation/httpd.conf /hdd/server/conf/
#cp -fr /compilation/my.cnf /hdd/server/
cp -fr /compilation/nano-bin /hdd/server/bin/
cp -fr /compilation/nano /hdd/server/bin/
cp -fr /compilation/nano-bin /hdd/server/bin/
#note: next 2 folders allso you need to link to the /system/etc/ inside android to get terminfo working!
ln -s /hdd/server/share/terminfo /hdd/server/terminfo
ln -s /hdd/server/share/tabset /hdd/server/tabset
#
mkdir /hdd/server/session22
chmod 777 /hdd/server/session22
mkdir /hdd/server/tmp
chmod 777 /hdd/server/tmp
#cp /compilation/php-5.5.1/php.ini-production /hdd/server/conf/php.ini
cp -fr /compilation/php.ini /hdd/server/conf/php.ini
cp -fr httpd.conf /hdd/server/conf/
for TT in `tar tzf /compilation/lib.tar.gz | sed 's/.\///g'`; do rm -rf /hdd/server/lib/$TT; done
#####################################################################
cd /
rm -rf hdd.tar.gz
tar pczvf hdd.tar.gz hdd
echo ""
echo -e "${BLUE}DONE !!!${NC}"
