
#https://curl.haxx.se/docs/install.html

ARCH=arch-arm
#ARCH=arch-x86
#ARCH=arch-mips

CPU=arm
#CPU=x86
#CPU=mips

export SSL_PATH=~/C_C++/curl_new/openssl-android
export NDK=$ANDROID_NDK
export PATH=~/curl_toolchain/bin:$PATH

export PREBUILT=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt
export SYSROOT=$NDK/platforms/android-21/$ARCH

#使用独立toolchain
export CXX=arm-linux-androideabi-g++
export CC=arm-linux-androideabi-gcc
export AR=arm-linux-androideabi-ar
export LD=arm-linux-androideabi-gcc
export RANLIB=arm-linux-androideabi-ranlib
export STRIP=arm-linux-androideabi-strip
#export CXXFLAGS="-lstdc++"

#export CC="$PREBUILT/darwin-x86_64/bin/arm-linux-androideabi-gcc --sysroot=$SYSROOT"

#export CXX="$PREBUILT/darwin-x86_64/bin/arm-linux-androideabi-g++ --sysroot=$SYSROOT"


export LDFLAGS="-L$NDK/platforms/android-21/arch-arm/usr/lib -L$SSL_PATH/libs/armeabi"
export CPPFLAGS="-I$NDK/platforms/android-21/arch-arm/usr/include -I$SSL_PATH/include/openssl"


export PLATFORM=$NDK/platforms/android-21/$ARCH


export PREFIX=~/C_C++/curl_new/output



#./$NDK/make-standalone-toolchain.sh  --install-dir=../../android-21 --platform=android-21 --toolchain=arm-linux-androideabi-4.9

build_one(){
./configure --host=arm-linux-androideabi --prefix=$PREFIX --enable-debug --enable-silent-rules --enable-http \
--disable-ftp --disable-file --disable-ldap --disable-ldaps --disable-rtsp --disable-telnet --disable-tftp --disable-pop3 \
--disable-imap --disable-smb  --disable-smtp --disable-gopher   --disable-tls-srp --without-ca-bundle \
--without-librtmp --without-winidn  --disable-dict --disable-sspi --with-ssl --enable-static=$PREFIX/libs/libcurl-android.so \
--with-zlib
}

build_one

make
make install
