#!/bin/bash

red='\e[0;31m'
RED='\e[1;31m'
blue='\e[0;34m'
BLUE='\e[1;34m'
cyan='\e[0;36m'
CYAN='\e[1;36m'
NC='\e[0m' # No Color

OLDPATH=$PATH

TOOLCHAIN=/compilation/arm-linux-androideabi-4.6
PLATFORM=$TOOLCHAIN/sysroot
export PATH=$PATH:$TOOLCHAIN/bin

# just to save this function :)
#for TT in `ls /androidserver/bin`; do mv /androidserver/bin/$TT /androidserver/bin/`echo $TT | awk '{gsub("arm-", ""); print $1 }'`; done
#$TOOLCHAIN/bin/arm-linux-androideabi-objcopy --localize-symbol=__dtoa /compilation/backup/libcrystax.a
#
#6) cd lib/bind
#7) gcc-3.4 -pthread -shared -Wl,-soname,libresolv.so.2 */*.o -o libresolv.so

# chech for needed libs for qemu
if [ ! -e /system ]; then ln -s /compilation/system /system ; fi

rm -rf /androidserver
echo "" > CONFIGURE.log

################################################################
echo ""
echo -e "${RED}COMPILING OSCAM${NC}"
echo ""
sleep 3
#make LIB_PTHREAD= CROSS=$TOOLCHAIN/bin/arm-linux-androideabi- USE_LIBCRYPTO=1 LIBCRYPTO_LIB="/system/lib/libcrypto.a" USE_SSL=1 SSL_LIB="/system/lib/libssl.a" EXTRA_CFLAGS="-I/system/include"
#################################################################
echo ""
echo -e "${RED}COMPILING GLOB${NC}"
echo ""
sleep 3
cd glob
echo -e "glob\n\n" >> ../CONFIGURE.log
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
echo -e "${RED}COMPILING PERL-5.16.3${NC}"
echo ""
sleep 3
cd perl-5.16.3
if ! ./configure --target=arm-linux-androideabi --sysroot=$PLATFORM --prefix=/androidserver --disable-mod=ext/Errno ; then
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
make install
cd ..
# after installing, perl is broken on my host machine, so fix for them is re-linking old perl again
rm -rf /usr/bin/perl
ln -s /usr/bin/perl5.14.2 /usr/bin/perl
#####################################################################
echo ""
echo -e "${RED}COMPILING NCURSES-5.7${NC}"
echo ""
sleep 3
cd ncurses-5.7
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB="$TOOLCHAIN/bin/arm-linux-androideabi-ranlib" AR="$TOOLCHAIN/bin/arm-linux-androideabi-ar" LD="$TOOLCHAIN/bin/arm-linux-androideabi-ld" STRIP="$TOOLCHAIN/bin/arm-linux-androideabi-strip" CXX="$TOOLCHAIN/bin/arm-linux-androideabi-c++" ASCPP="$TOOLCHAIN/bin/arm-linux-androideabi-as" ./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --without-cxx-binding --disable-big-core --disable-big-strings --disable-leaks --enable-expanded --disable-largefile CFLAGS="--sysroot=$PLATFORM" program_prefix="" ac_cv_func_getttynam=no ac_cv_lib_util_openpty=no cf_cv_func_openpty=no ac_cv_func_sigvec=no cf_cv_dcl_errno=yes cf_cv_have_errno=yes cf_cv_need_libm=yes ac_cv_header_locale_h=no ; then
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
make install
make distclean
cd ..
ln -s /androidserver/lib/libncurses.a /androidserver/lib/libcurses.a
#####################################################################
echo ""
echo -e "${RED}COMPILING ZLIB${NC}"
echo ""
sleep 3
cd zlib
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --static ; then
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
make install
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING BZIP2-1.0.6${NC}"
echo ""
sleep 3
cd bzip2-1.0.6
echo -e "bzip2\n\n" >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make ; then
    echo "Failure!!!"
    exit 1
fi                                                                                                                                                                                                                                                                                                                            
make install PREFIX=/androidserver
make distclean
cd ..
################################################################
echo ""
echo -e "${RED}COMPILING TERMCAP${NC}"
echo ""
sleep 3
cd termcap-1.3.1
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --target=arm --host=arm program_prefix="" ; then
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
make install
make distclean
cd ..
#################################################################
echo ""
echo -e "${RED}COMPILING PCRE${NC}"
echo ""
sleep 3
cd pcre-8.33
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --enable-static --disable-shared --target=arm-none-linux --host=arm-none-linux --enable-utf8 --enable-unicode-properties --enable-pcregrep-libz --enable-rebuild-chartables program_prefix="" --disable-pcretest-libreadline --enable-pcregrep-libbz2 LDFLAGS="-L/androidserver/lib -lz -lbz2" CFLAGS="-I/androidserver/include --sysroot=$PLATFORM" program_prefix="" ; then
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
make install
make distclean
cd ..
#################################################################
echo ""
echo -e "${RED}COMPILING MHASH${NC}"
echo ""
sleep 3
cd mhash-0.9.9.9
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as CFLAGS="--sysroot=$PLATFORM" ./configure --prefix=/androidserver --enable-static --disable-shared --target=arm-none-linux --host=arm-none-linux --with-CC=$TOOLCHAIN/bin/arm-linux-androideabi-gcc program_prefix="" ac_cv_func_signal=no ; then
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
make install
make distclean
cd ..
##################################################################
echo ""
echo -e "${RED}COMPILING LIBMCRYPT${NC}"
echo ""
sleep 3
cd libmcrypt-2.5.8
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --enable-static --disable-shared --target=arm-none-linux --host=arm-none-linux program_prefix="" ac_cv_func_shl_load=no ac_cv_lib_dld_shl_load=no ; then
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
make install
make distclean
cd ..
##################################################################
echo ""
echo -e "${RED}COMPILING ICONV${NC}"
echo ""
sleep 3
cd libiconv-1.14
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" \
RANLIB="$TOOLCHAIN/bin/arm-linux-androideabi-ranlib" \
AR="$TOOLCHAIN/bin/arm-linux-androideabi-ar" \
LD="$TOOLCHAIN/bin/arm-linux-androideabi-ld" \
STRIP="$TOOLCHAIN/bin/arm-linux-androideabi-strip" \
CXX="$TOOLCHAIN/bin/arm-linux-androideabi-c++" \
ASCPP="$TOOLCHAIN/bin/arm-linux-androideabi-as" \
./configure --prefix=/androidserver \
--enable-extra-encodings \
--enable-static \
--disable-shared \
--target=arm \
--host=arm \
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
make install
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING EXPAT${NC}"
echo ""
sleep 3
cd expat-2.1.0
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --enable-static --disable-shared --target=arm-none-linux --host=arm-none-linux program_prefix="" ac_cv_func_getpagesize=no ; then
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
make install
make distclean
cd ..
##################################################################
echo ""
echo -e "${RED}COMPILING LIBMBFL${NC}"
echo ""
sleep 3
cd libmbfl-1.2.0
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --enable-static --disable-shared --target=arm-none-linux --host=arm-none-linux program_prefix="" ; then
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
make install
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING OPENSSL${NC}"
echo ""
sleep 3
cd openssl
#compiling static openssl
#export PATH=$PATH:/root/arm-none-linux-gnueabi/arm-2011.03/bin
#./Configure linux-generic32 -DL_ENDIAN --prefix=/androidserver no-shared -static -ldl -lgcc -lc -lm -fPIC
#make CC="arm-none-linux-gnueabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=arm-none-linux-gnueabi-ranlib
#configuring imap
#make slx CC=/root/arm-none-linux-gnueabi/arm-2011.03/bin/arm-none-linux-gnueabi-gcc RANLIB=/root/arm-none-linux-gnueabi/arm-2011.03/bin/arm-none-linux-gnueabi-ranlib AR=/root/arm-none-linux-gnueabi/arm-2011.03/bin/arm-none-linux-gnueabi-ar LD=/root/arm-none-linux-gnueabi/arm-2011.03/bin/arm-none-linux-gnueabi-ld STRIP=/root/arm-none-linux-gnueabi/arm-2011.03/bin/arm-none-linux-gnueabi-strip CXX=/root/arm-none-linux-gnueabi/arm-2011.03/bin/arm-none-linux-gnueabi-c++ ASCPP=/root/arm-none-linux-gnueabi/arm-2011.03/bin/arm-none-linux-gnueabi-as EXTRACFLAGS="-fPIC -I/androidserver/include/openssl -I/androidserver/include" SSLDIR=/androidserver/ssl SSLLIB="/androidserver/lib" EXTRALDFLAGS="-lm -lc -lgcc -lssl -lcrypto -ldl"
#cp c-client/*.a /androidserver/lib/
#cp c-client/*.h /androidserver/include/
if ! ./Configure android-armv7 -DL_ENDIAN --prefix=/androidserver no-shared -fPIC ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "openssl\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp -fPIC" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib ; then
    echo "Failure!!!"
    exit 1
fi
make CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib install
make clean
cd ..
###################################################################
echo ""
echo -e "${RED}COMPILING CRYPT${NC}"
echo ""
sleep 3
cd crypt_0_03
make clean
if ! make ; then
    echo "Failure!!!"
    exit 1
fi
cp libcrypt.a /androidserver/lib/
cp -fr *.h /androidserver/include/
make clean
cd ..
# next step was:
### commented base64_encode and base64_decode in /androidserver/include/crypt.h !!! ### 
### commented (#define  MP_NEG  1) and (#define  NEG     MP_NEG) in /androidserver/include/mpi.h ###
cp -fr mpi.h /androidserver/include/
cp -fr crypt.h /androidserver/include/
echo -e "crypt\n\n" >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
####################################################################
echo ""
echo -e "${RED}COMPILING LIBXML2${NC}"
echo ""
sleep 3
cd libxml2
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --target=arm-none-linux --host=arm-none-linux --prefix=/androidserver --without-iconv --without-python --enable-static --disable-shared --without-readline --with-zlib=/androidserver/lib program_prefix="" CFLAGS="-I/androidserver/iinclude" LDFLAGS="-L/androidserver/lib" LIBS="-lz -liconv" ac_cv_func_isnand=no ac_cv_func_shl_load=no ac_cv_lib_dld_shl_load=no ac_cv_func_finite=no ac_cv_func_srand=no ac_cv_func_isinf=no ac_cv_lib_m_isinf=no ac_cv_func_isnan=no ac_cv_func_isnand=no ac_cv_func_fp_class=no ac_cv_header_fp_class_h=no ac_cv_func_fpclass=no ac_cv_func_class=no ac_cv_func_rand=no ac_cv_func_rand_r=no ac_cv_func_srand=no ac_cv_func_signal=no ac_cv_func__stat=no ; then
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
make install
make distclean
cd ..
###################################################################
echo ""
echo -e "${RED}COMPILING BIND${NC}"
echo ""
sleep 3
cd bind-9.4.1
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as BUILD_CC=gcc ./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --enable-static --disable-shared --with-openssl=/androidserver --with-randomdev=/dev/urandom --with-libiconv=/androidserver --enable-libbind ac_cv_func_sysctlbyname=no ac_cv_lib_scf_smf_enable_instance=no ac_cv_func_catgets=no ac_cv_func_getipnodebyname=no ac_cv_lib_inet6_getifaddrs=no ; then
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
make install
make distclean
cd ..
mkdir /androidserver/bind/include/sys
echo "" >/androidserver/bind/include/sys/bitypes.h
ln -s /androidserver/lib/libbind.a /androidserver/lib/libresolv.a
/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-objcopy --localize-symbol=MD5_Init /androidserver/lib/libresolv.a
/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-objcopy --localize-symbol=MD5_Final /androidserver/lib/libresolv.a
/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-objcopy --localize-symbol=MD5_Update /androidserver/lib/libresolv.a
/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-objcopy --localize-symbol=MD5_version /androidserver/lib/libresolv.a
##################################################################
echo ""
echo -e "${RED}COMPILING MYSQL${NC}"
echo ""
sleep 3
cd mysql-5.1.32
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIb AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as CC_FOR_BUILD=gcc ./configure --prefix=/androidserver --target=arm --host=arm --disable-shared --enable-static  --with-ssl --without-docs --without-man --with-readline CFLAGS="-I/androidserver/include -D_GNU_SOURCE" CXXFLAGS="-I/androidserver/include -D_GNU_SOURCE" LDFLAGS="-L/androidserver/lib" LIBS="-lncurses -lz -lssl -lcrypto" ac_cv_sys_restartable_syscalls=yes with_named_curses=yes mysql_cv_termcap_lib=yes ac_cv_func_getpwent=no program_prefix="" ac_cv_func_backtrace=no ac_cv_func_backtrace_symbols=no ac_cv_func_backtrace_symbols_fd=no ac_cv_func_p2open=no ac_cv_lib_gen_p2open=no ac_cv_func_yp_get_default_domain=no ac_cv_lib_nsl_yp_get_default_domain=no ac_cv_search_crypt=no ac_cv_func_pthread_init=no ac_cv_pthread_yield_one_arg=no ac_cv_pthread_yield_zero_arg=yeso ac_cv_func_re_comp=no ac_cv_func_getline=no ac_cv_func_tcgetattr=no ac_cv_func_wctomb=no ac_cv_func_strunvis=no ac_cv_func_strvis=no ac_cv_func__doprnt=no ac_cv_func_bcmp=no ac_cv_func_bfill=no ac_cv_func_bmove=no ac_cv_func_chsize=no ac_cv_func_cuserid=no ac_cv_func_fconvert=no ac_cv_func_fpresetsticky=no ac_cv_func_fpsetmask=no ac_cv_func_gethostbyaddr_r=no ac_cv_func_getpass=no ac_cv_func_getpassphrase=no ac_cv_func_getwd=no ac_cv_func_gethrtime=no ac_cv_func_locking=no ac_cv_func_mlockall=no ac_cv_func_mmap64=no ac_cv_func_pthread_attr_create=no ac_cv_func_getpagesize=no ac_cv_func_pthread_attr_setprio=no ac_cv_func_pthread_condattr_create=no ac_cv_func_pthread_getsequence_np=no ac_cv_func_pthread_setprio=no ac_cv_func_pthread_setprio_np=no ac_cv_func_rwlock_init=no ac_cv_func_shmget=no ac_cv_func_shmat=no ac_cv_func_shmdt=no ac_cv_func_shmctl=no ac_cv_func_sigemptyset=no ac_cv_func_sigaddset=no ac_cv_func_sighold=no ac_cv_func_sigset=no ac_cv_type_sigset_t=no ac_cv_func_sigthreadmask=no ac_cv_func_port_create=no ac_cv_func_stpcpy=no ac_cv_func_tell=no ac_cv_func_thr_setconcurrency=no ac_cv_func_posix_fallocate=no ac_cv_func_pthread_setschedprio=no ac_cv_lib_rt_aio_read=no ; then
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
make install
make distclean
cd ..
##################################################################
echo ""
echo -e "${RED}COMPILING APR${NC}"
echo ""
sleep 3
cd apr
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as CC_FOR_BUILD=gcc ./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --enable-static --disable-shared CFLAGS="-I/androidserver/include" program_prefix="" ac_cv_file__dev_zero=yes ac_cv_func_setpgrp_void=yes apr_cv_process_shared_works=yes apr_cv_mutex_robust_shared=yes apr_cv_tcp_nodelay_with_cork=yes ac_cv_search_crypt=no ac_cv_search_modf=-lm ac_cv_func_pthread_yield=no ac_cv_func_gethostbyaddr_r=no ac_cv_func_kqueue=noac_cv_func_kqueue=no ac_cv_func_port_create=no ac_cv_func_getpwnam_r=no ac_cv_func_getpwuid_r=no ac_cv_func_getgrnam_r=no ac_cv_func_getgrgid_r=no ac_cv_func_shm_open=no ac_cv_search_shm_open=no ac_cv_func_getservbyname_r=no ac_cv_func_shm_unlink=no ac_cv_func_shmget=no ac_cv_func_shmat=no ac_cv_func_shmdt=no ac_cv_func_shmctl=no ac_cv_func_create_area=no ac_cv_func_isinf=no ac_cv_func_getifaddrs=no ac_cv_func_sendfilev=no ac_cv_lib_sendfile_sendfilev=no ac_cv_func__getch=no ac_cv_func_getpass=no ac_cv_func_getpassphrase=no ac_cv_func_strnicmp=no ac_cv_func_stricmp=no ac_cv_func_semget=no ac_cv_func_semctl=no ac_cv_func_create_sem=no ac_cv_func_uuid_create=no ac_cv_search_uuid_create=no ac_cv_func_uuid_generate=no ac_cv_search_uuid_generate=no ac_cv_func_send_file=no ac_cv_func_set_h_errno=no ac_cv_func_nl_langinfo=no ; then
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
make install
make distclean
cd ..
###################################################################
echo ""
echo -e "${RED}COMPILING APR-UTIL${NC}"
echo ""
sleep 3
cd apr-util-1.5.2
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as CC_FOR_BUILD=gcc ./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --without-iconv --with-expat=/androidserver --with-mysql=/androidserver --with-openssl=/androidserver --with-crypto --with-apr=/androidserver/bin/apr-1-config CFLAGS="-I/androidserver/include" LDFLAGS="-L/androidserver/lib" LIBS="-lssl -lcrypto -liconv" LIBTOOL="/androidserver/build-1/libtool" program_prefix="" ac_cv_search_crypt=yes ac_cv_func_nl_langinfo=no ac_cv_header_langinfo_h=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "apr-util\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL="/androidserver/build-1/libtool" ; then
    echo "Failure!!!"
    exit 1
fi
make install LIBTOOL="/androidserver/build-1/libtool"
make distclean
cd ..
arm-linux-androideabi-gcc -O2 -Wall -shared -o /androidserver/lib/apr-util-1/apr_crypto_openssl.so -Wl,--whole-archive /androidserver/lib/apr-util-1/apr_crypto_openssl.a -Wl,--no-whole-archive
arm-linux-androideabi-gcc -O2 -Wall -shared -o /androidserver/lib/apr-util-1/apr_dbd_mysql.so -Wl,--whole-archive /androidserver/lib/apr-util-1/apr_dbd_mysql.a -Wl,--no-whole-archive
ln -s /androidserver/lib/apr-util-1/apr_crypto_openssl.so /androidserver/lib/apr-util-1/apr_crypto_openssl-1.so
ln -s /androidserver/lib/apr-util-1/apr_dbd_mysql.so /androidserver/lib/apr-util-1/apr_dbd_mysql-1.so
#####################################################################
echo ""
echo -e "${RED}COMPILING LIBJPEG-8d${NC}"
echo ""
sleep 3
cd jpeg-8d
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --target=arm-linux-androideabi --host=arm-linux-androideabi --disable-shared --enable-static --enable-maxmem=20 program_prefix="" LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "libjpeg\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
make install LIBTOOL=/androidserver/build-1/libtool
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING FREETYPE-2.5.0.1${NC}"
echo ""
sleep 3
cd freetype-2.5.0.1
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as \
./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --disable-shared --enable-static --without-zlib --without-bzip2 --without-png LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "freetype\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
make install LIBTOOL=/androidserver/build-1/libtool
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING FONTCONFIG-2.10.2${NC}"
echo ""
sleep 3
cd fontconfig-2.10.2
cp /compilation/ft2build.h /androidserver/include/
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --disable-shared --enable-static --enable-iconv --disable-libxml2 --disable-docs --with-arch=arm --with-libiconv=/androidserver/bin --with-libiconv-includes=/androidserver/include --with-libiconv-lib=/androidserver/lib --with-expat --with-expat-includes=/androidserver/include --with-expat-lib=/androidserver/lib --with-default-fonts=/androidserver/fonts --with-add-fonts=/system/fonts FREETYPE_CFLAGS="-I/androidserver/include/freetype2" FREETYPE_LIBS="-L/androidserver/lib -lfreetype" CFLAGS="-I/androidserver/include" LIBTOOL=/androidserver/build-1/libtool ac_cv_func__doprnt=no ac_cv_func_getpagesize=no ac_cv_func_chsize=no ac_cv_func_rand=no ac_cv_func_random=no ac_cv_func_random_r=no ac_cv_func_rand_r=no ac_cv_func_fstatvfs=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "fontconfig\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
make install LIBTOOL=/androidserver/build-1/libtool
make distclean
cd ..
/compilation/qemu-arm /androidserver/bin/fc-cache
#####################################################################
echo ""
echo -e "${RED}COMPILING LIBPNG-1.6.3${NC}"
echo ""
sleep 3
cd libpng-1.6.3
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --disable-shared --enable-static --with-zlib-prefix=/androidserver LIBTOOL=/androidserver/build-1/libtool ac_cv_func_pow=no ac_cv_func_feenableexcept=no ac_cv_lib_m_feenableexcept=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "libpng\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
make install LIBTOOL=/androidserver/build-1/libtool
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING TIFF-4.0.3${NC}"
echo ""
sleep 3
cd tiff-4.0.3
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --disable-shared --enable-static --without-x --disable-jbig --disable-lzma --with-zlib-include-dir=/androidserver/include --with-zlib-lib-dir=/androidserver/lib --with-jpeg-lib-dir=/androidserver/lib CFLAGS="-I/androidserver/include" LDFLAGS="-L/androidserver/lib" LIBTOOL=/androidserver/build-1/libtool ac_cv_func_setmode=no ac_cv_func_lfind=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "tiff\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
make install LIBTOOL=/androidserver/build-1/libtool
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING LIBGD-2.1.0${NC}"
echo ""
sleep 3
cd libgd-2.1.0
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --disable-shared --enable-static --without-x --without-libiconv-prefix --with-zlib=/androidserver/lib --with-jpeg=/androidserver/lib --without-xpm --without-vpx CFLAGS="-I/androidserver/include" LIBPNG_LIBS="-L/androidserver/lib -lpng16" LIBPNG_CFLAGS="-I/androidserver/include/libpng16" LIBFONTCONFIG_LIBS="-L/androidserver/lib -lfontconfig" LIBFONTCONFIG_CFLAGS="-I/androidserver/include/fontconfig" LIBTIFF_LIBS="-L/androidserver/lib -ltiff" LIBTIFF_CFLAGS="-I/androidserver/include" LDFLAGS="-L/androidserver/lib" LIBS="-liconv" LIBTOOL=/androidserver/build-1/libtool ac_cv_func_sin=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "libgd\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
make install LIBTOOL=/androidserver/build-1/libtool
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING GETTEXT-0.18.3${NC}"
echo ""
sleep 3
cd gettext-0.18.3
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --enable-static --disable-shared --with-libpth-prefix --disable-java --disable-native-java --disable-threads --disable-c++ --enable-relocatable --disable-curses --disable-acl --disable-openmp --without-bzip2 --without-xz --with-libiconv-prefix=/androidserver CFLAGS="-I/androidserver/include" LDFLAGS="-L/androidserver/lib" LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "gettext\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
make install LIBTOOL=/androidserver/build-1/libtool
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING GNU MP 5.1.2${NC}"
echo ""
sleep 3
cd gmp-5.1.2
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip CXX=$TOOLCHAIN/bin/arm-linux-androideabi-c++ ASCPP=$TOOLCHAIN/bin/arm-linux-androideabi-as ./configure --prefix=/androidserver --target=arm-none-linux --host=arm-none-linux --enable-static --disable-shared --without-readline --disable-cxx CFLAGS="--sysroot=$PLATFORM" LIBTOOL=/androidserver/build-1/libtool ac_cv_func_attr_get=no ac_cv_func_cputime=no ac_cv_func_getpagesize=no ac_cv_func_getsysinfo=no ac_cv_func_localeconv=no ac_cv_func_nl_langinfo=no ac_cv_func_obstack_vprintf=no ac_cv_func_processor_info=no ac_cv_func_pstat_getprocessor=no ac_cv_func_sigstack=no ac_cv_func_read_real_time=no ac_cv_func_syssgi=no ac_cv_header_sys_syssgi_h=no ac_cv_func_sysctl=no ac_cv_func_sysctlbyname=no ac_cv_header_sys_sysctl_h=no gmp_cv_m4_m4wrap_spurious=no ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "gmp\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
make install LIBTOOL=/androidserver/build-1/libtool
make distclean
cd ..
###################################################################
echo ""
echo -e "${RED}COMPILING APACHE2${NC}"
echo ""
sleep 3
cd httpd-2.4.6
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" \
RANLIB="$TOOLCHAIN/bin/arm-linux-androideabi-ranlib" \
AR="$TOOLCHAIN/bin/arm-linux-androideabi-ar" \
LD="$TOOLCHAIN/bin/arm-linux-androideabi-ld" \
STRIP="$TOOLCHAIN/bin/arm-linux-androideabi-strip" \
CXX="$TOOLCHAIN/bin/arm-linux-androideabi-c++" \
ASCPP="$TOOLCHAIN/bin/arm-linux-androideabi-as" \
CFLAGS="-D_GNU_SOURCE -DBIG_SECURITY_HOLE" \
CXXFLAGS="-D_GNU_SOURCE -DBIG_SECURITY_HOLE" \
CC_FOR_BUILD=gcc \
./configure --prefix=/androidserver \
--target=arm-none-linux \
--host=arm-none-linux \
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
--with-ssl=/androidserver \
--enable-dav \
--enable-info \
--enable-cgi \
--enable-negotiation \
--enable-rewrite \
--with-z=/androidserver \
--with-pcre=/androidserver \
--with-openssl=/androidserver \
--with-libxml2=/androidserver \
--with-apr=/androidserver/bin \
--with-apr-util=/androidserver/bin \
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
--with-suexec-bin=/androidserver/bin/suexec \
--with-suexec-caller=system \
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
ac_cv_search_crypt=no \
program_prefix="" ; then
    echo "Failure!!!"
    exit 1
fi
echo -e "apache2\n\n" >> ../CONFIGURE.log
cat config.log | grep undefined >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
if ! make LIBTOOL=/androidserver/build-1/libtool ; then
    echo "Failure!!!"
    exit 1
fi
make install LIBTOOL=/androidserver/build-1/libtool
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COMPILING C-CLIENT (IMAP)${NC}"
echo ""
sleep 3
cd imap-2007f
echo -e "imap\n\n" >> ../CONFIGURE.log
echo -e "\n---------------------------------------\n" >> ../CONFIGURE.log
echo "" > /androidserver/include/shadow.h
cp /compilation/sysexits.h /androidserver/include/
if ! make lnx CC=arm-linux-androideabi-gcc RANLIB=arm-linux-androideabi-ranlib AR=arm-linux-androideabi-ar LD=arm-linux-androideabi-ld STRIP=arm-linux-androideabi-strip CXX=arm-linux-androideabi-c++ ASCPP=arm-linux-androideabi-as EXTRACFLAGS="-fPIC -I/androidserver/include/openssl -I/androidserver/include -I/compilation/crypt_uclibs" SSLDIR=/androidserver/ssl SSLLIB="/androidserver/lib" EXTRALDFLAGS="-lssl -lcrypto" ; then
    echo "Failure!!!"
    exit 1
fi
cp -fr c-client/*.a /androidserver/lib/
cp --remove-destination c-client/*.h /androidserver/include/
rm /androidserver/include/shadow.h
rm /androidserver/include/sysexits.h
make clean
cd ..
#####################################################################
export PATH=$OLDPATH
TOOLCHAIN=/compilation/crystax-1-linux-x86_64_toolchain
PLATFORM=$TOOLCHAIN/sysroot
export PATH=$PATH:$TOOLCHAIN/bin
echo ""
echo -e "${RED}COMPILING PHP-5.5.1${NC}"
echo ""
sleep 3
cd php-5.5.1
if ! CC="$TOOLCHAIN/bin/arm-linux-androideabi-gcc -march=armv7-a -mfloat-abi=softfp -mfpu=vfp" \
RANLIB="$TOOLCHAIN/bin/arm-linux-androideabi-ranlib" \
AR="$TOOLCHAIN/bin/arm-linux-androideabi-ar" \
LD="$TOOLCHAIN/bin/arm-linux-androideabi-ld" \
STRIP="$TOOLCHAIN/bin/arm-linux-androideabi-strip" \
CXX="$TOOLCHAIN/bin/arm-linux-androideabi-c++" \
ASCPP="$TOOLCHAIN/bin/arm-linux-androideabi-as" \
CFLAGS="-I/androidserver/bind/include -I/androidserver/include" \
CXXFLAGS="-I/androidserver/bind/include -I/androidserver/include" \
./configure --prefix=/androidserver \
--target=arm-none-linux \
--host=arm-none-linux \
--with-apxs2=/androidserver/bin/apxs \
--enable-sigchild \
--enable-sockets \
--enable-wddx \
--enable-cgi \
--with-libxml-dir=/androidserver \
--with-libexpat-dir=/androidserver \
--enable-zip \
--with-zlib-dir=/androidserver/lib \
--with-pear \
--enable-zend-signals \
--with-pic \
--enable-libxml \
--with-libxml-dir=/androidserver \
--enable-simplexml \
--with-openssl=/androidserver \
--with-pcre-regex=/androidserver \
--with-zlib \
--with-zlib-dir=/androidserver/lib \
--with-bz2=/androidserver \
--enable-ctype \
--enable-bcmath \
--enable-mbstring \
--without-mm \
--with-libmbfl=/androidserver \
--with-mcrypt=/androidserver \
--with-mysql=/androidserver \
--with-mysqli=/androidserver/bin/mysql_config \
--with-pdo-mysql=/androidserver \
--with-libexpat-dir=/androidserver \
--without-readline \
--with-iconv=/androidserver \
--with-pcre-dir=/androidserver \
--enable-opcache=no \
--enable-shared \
--with-mhash=/androidserver \
--enable-ftp \
--with-config-file-path=/androidserver/conf \
--with-config-file-scan-dir=/androidserver/conf/php \
--disable-phar \
--with-gd=/androidserver \
--with-jpeg-dir=/androidserver \
--with-png-dir=/androiserver \
--with-zlib-dir=/androidserver/lib \
--with-freetype-dir=/androidserver \
--enable-gd-native-ttf \
--enable-gd-jis-conv \
--with-gettext=/androidserver \
--with-gmp=/androidserver \
--enable-exif \
--with-imap=/androidserver \
--with-imap-ssl=/androidserver \
ac_cv_lib_gd_gdImageCreateFromXpm=no \
ac_cv_lib_crypt_crypt=yes \
ac_cv_crypt_SHA256=yes \
ac_cv_crypt_SHA512=yes \
ac_cv_crypt_blowfish=yes \
ac_cv_crypt_md5=yes \
ac_cv_func_crypt=yes \
ac_cv_func_glob=yes \
program_prefix="" \
LDFLAGS="-L/androidserver/lib" LIBS="-lresolv -lbind9 -lcharset -ldns -ltermcap -lglob" ; then
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
make install
make distclean
cd ..
#####################################################################
echo ""
echo -e "${RED}COPYING CONFIGURATIONS, INSTALLING LYNX, INSTALLING NANO, TERMINFO${NC}"
echo ""
sleep 3
cp -fr /compilation/httpd.conf /androidserver/conf/
cp -fr /compilation/my.cnf /androidserver/
cp -fr /compilation/lynxdir /androidserver/lynx
cp -fr /compilation/lynx /androidserver/bin/
cp -fr /compilation/nano-bin /androidserver/bin/
cp -fr /compilation/nano /androidserver/bin/
cp -fr /compilation/nano-bin /androidserver/bin/
#note: next 2 folders allso you need to link to the /system/etc/ inside android to get terminfo working!
ln -s /androidserver/share/terminfo /androidserver/terminfo
ln -s /androidserver/share/tabset /androidserver/tabset
#
mkdir /androidserver/session22
chmod 777 /androidserver/session22
mkdir /androidserver/tmp
chmod 777 /androidserver/tmp
#cp /compilation/php-5.5.1/php.ini-production /androidserver/conf/php.ini
cp /compilation/php.ini /androidserver/conf/php.ini
#####################################################################
cd /
rm -rf androidserver.tar.gz
tar pczvf androidserver.tar.gz androidserver
echo ""
echo -e "${BLUE}DONE !!!${NC}"
