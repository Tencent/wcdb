#!/bin/bash

# Used configurations
OPENSSL_CONFIG="no-rc2 no-cast \
  no-md2 no-md4 no-md5 no-ripemd no-mdc2 \
  no-rsa no-dsa no-dh no-ec no-ecdsa no-ecdh \
  no-sock no-ssl no-tls no-krb5 \
  no-shared no-comp no-hw no-engine no-zlib no-fips \
  -ffunction-sections -fdata-sections -Wl,--gc-sections"

SQLCIPHER_CFLAGS="-DSQLITE_DEFAULT_WORKER_THREADS=2 \
  -DSQLITE_DEFAULT_JOURNAL_SIZE_LIMIT=1048576 \
  -DUSE_PREAD64=1 \
  -DSQLITE_ENABLE_FTS3_PARENTHESIS \
  -DSQLITE_ENABLE_FTS3_TOKENIZER \
  -DSQLITE_ENABLE_STAT4 \
  -DSQLITE_ENABLE_EXPLAIN_COMMENTS \
  -DSQLITE_ENABLE_DBSTAT_VTAB \
  -DSQLITE_HAS_CODEC -DOMIT_MEMLOCK -DOMIT_MEM_SECURITY"

SQLCIPHER_CONFIG="--disable-shared --enable-static \
  --enable-fts3 --enable-fts4 --enable-fts5 --enable-json1 --enable-session \
  --enable-tempstore=always --enable-threadsafe=multi --disable-tcl"

USE_CLANG=1
BUILD_ARCHS="arm,arm64,x86,x86_64"
BUILD_API=16
BUILD_PIE=1
BUILD_CRYPTO=1
BUILD_SQLCIPHER=0

###############################
# Help message
###############################
show_help()
{
  cat <<_EOF
Usage: build-depends-android.sh [OPTIONS]

OPTIONS:
    -h            Show this help message.
    -n <path>     Specify Android NDK installation path.
                  If this option is omitted, use ANDROID_NDK_ROOT environment variable.
    -a <archs>    Comma separated architectures to build. Default to "arm,arm64,x86,x86_64"
    -i <number>   API level to build. Default to 14.
    -e            Do not build position-independent executables.
    -g            Uses GCC toolchains.
    -G            Uses clang toolchains. This is the default.
    -c            Do not build libcrypto.
    -s            Build SQLCipher library and executables.

_EOF
}

# Parse arguments.
#argv=`getopt -o 'h' -l 'help,ndk:,arch:,api:,clang,gcc,no-pie,no-crypto,build-sqlcipher' -- "$@"`
#[ $? != 0 ] && exit 1

#eval set -- "$argv"

while getopts ':hn:a:i:egGcs' arg; do
  case "$arg" in
    h) show_help; exit 1 ;;
    n) ANDROID_NDK_ROOT="$OPTARG";;
    a) BUILD_ARCHS="$OPTARG";;
    i) BUILD_API="$OPTARG";;
    G) USE_CLANG=1;;
    g) USE_CLANG=0;;
    e) BUILD_PIE=0;;
    c) BUILD_CRYPTO=0;;
    s) BUILD_SQLCIPHER=1;;
    *) echo "Unreconized option: $arg"; exit 1 ;;
  esac
done


# Locate Android NDK
if [ -z "$ANDROID_NDK_ROOT" ] || [ ! -d "$ANDROID_NDK_ROOT" ]; then
  echo "Error: ANDROID_NDK_ROOT is not a valid path."
  echo 'Set Android NDK location using `export ANDROID_NDK_ROOT=/path/to/ndk`'
  exit 1
fi
echo " * Android NDK location: $ANDROID_NDK_ROOT"

# NDK must have GCC 4.9 toolchains for arm, arm64 and x86 platform.
if [ ! -d "$ANDROID_NDK_ROOT/toolchains/arm-linux-androideabi-4.9" ] || \
    [ ! -d "$ANDROID_NDK_ROOT/toolchains/aarch64-linux-android-4.9" ] || \
    [ ! -d "$ANDROID_NDK_ROOT/toolchains/x86-4.9" ]; then
  echo "NDK must have GCC 4.9 toolchains for arm, arm64 and x86 platform."
  echo "Google and download the latest one."
  exit 1
fi

# Detect NDK host
ndk_host=""
for host in "windows-x86_64" "windows-x86" "linux-x86_64" "linux-x86" "darwin-x86_64"
do
  if [ -d "$ANDROID_NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/$host/bin" ]; then
    ndk_host=$host
    break
  fi
done

# Fix API level and architectures
case "$BUILD_API" in
  [0-9]|1[0-5]) BUILD_API=16 ;;
  20) BUILD_API=19 ;;
  22) BUILD_API=21 ;;
  25) BUILD_API=24 ;;
  [0-9][0-9]) ;;
  *) BUILD_API=16 ;;
esac
[ "$BUILD_API" -gt 28 ] && BUILD_API=28
BUILD_ARCHS=${BUILD_ARCHS//,/ }

# Check dependency sources
source_root=$(dirname "${BASH_SOURCE[0]}")
if [ ! -d "$source_root/openssl" ] || [ ! -d "$source_root/sqlcipher" ]; then
  echo "Error: Submodule directories don't exist. Try git submodule update --init"
  exit 1
fi

# Print build options
cat <<_EOF
NDK root: ${ANDROID_NDK_ROOT}
Build architectures: ${BUILD_ARCHS}
Build API level: ${BUILD_API}
Build PIE: $([ "$BUILD_PIE" == 1 ] && echo 'yes' || echo 'no')
Toolchain: $([ "$USE_CLANG" == 1 ] && echo 'clang' || echo 'GCC')

_EOF

# Save current directory
pushd "$source_root" >/dev/null 2>&1

# Determine position-independent executable flags
[ "$BUILD_PIE" == 1 ] && android_pie='-fPIE -pie' || android_pie=

# Patch OpenSSL
cat patch/openssl/*.patch | patch -p1 -R -dopenssl --dry-run > /dev/null
if [ $? != 0 ]; then 
  cat patch/openssl/*.patch | patch -p1 -N -dopenssl || exit 1
fi

sqlite3_headers_generated=0

for android_arch in $BUILD_ARCHS; do
  echo "###########################################"
  echo "# Building for architecture: $android_arch "
  echo "###########################################"

  # Setup current architecture
  case "$android_arch" in
    arm)
      android_eabi=arm-linux-androideabi
      gcc_prefix=arm-linux-androideabi
      clang_target=arm-none-linux-androideabi
      android_api=16
      android_cflags="-Os -fpic -mthumb -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16"
      openssl_conf="linux-armv4"
      ;;
    arm64)
      android_eabi=aarch64-linux-android
      gcc_prefix=aarch64-linux-android
      clang_target=aarch64-none-linux-android
      android_api=21
      android_cflags="-O2 -fpic"
      openssl_conf="linux-aarch64"
      ;;
    x86)
      android_eabi=x86
      gcc_prefix=i686-linux-android
      clang_target=i686-none-linux-android
      android_api=16
      android_cflags="-O2 -fPIC"
      openssl_conf="linux-elf"
      ;;
    x86_64)
      android_eabi=x86_64
      gcc_prefix=x86_64-linux-android
      clang_target=x86_64-none-linux-android
      android_api=21
      android_cflags="-O2 -fPIC"
      openssl_conf="linux-x86_64"
      ;;
    *)
      echo "Unsupported architecture: $android_arch"
      exit 1
      ;;
  esac

  [ "$BUILD_API" -gt "$android_api" ] && android_api="$BUILD_API"
  android_sysroot="$ANDROID_NDK_ROOT/platforms/android-$android_api/arch-$android_arch"
  # If deprecated headers don't exist, uses unified headers
  if [ ! -d "$android_sysroot/usr/include" ]; then
    android_sysroot="$android_sysroot -isystem $ANDROID_NDK_ROOT/sysroot/usr/include"
    android_sysroot="$android_sysroot -isystem $ANDROID_NDK_ROOT/sysroot/usr/include/$gcc_prefix"
  fi

  android_cc="$gcc_prefix-gcc --sysroot=$android_sysroot"
  android_cflags="$android_cflags -g -Wall -funwind-tables -fstack-protector -fomit-frame-pointer -DNDEBUG -DANDROID -D__ANDROID_API__=$android_api"
  android_path="$ANDROID_NDK_ROOT/toolchains/$android_eabi-4.9/prebuilt/$ndk_host/bin:$ANDROID_NDK_ROOT/prebuilt/$ndk_host/bin"
  android_prefix="$(pwd)/android/prebuilt/$android_arch"
  
  if [ "$USE_CLANG" == 1 ]; then
    android_cc="clang -target $clang_target -gcc-toolchain $ANDROID_NDK_ROOT/toolchains/$android_eabi-4.9/prebuilt/$ndk_host --sysroot=$android_sysroot"
    android_cflags="-fno-integrated-as $android_cflags"
    android_path="$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/$ndk_host/bin:$android_path"
  fi

  # Export Android toolchain to PATH
  original_path="$PATH"
  export PATH="$android_path":"$PATH"

  # Build and install libcrypto
  if [ "$BUILD_CRYPTO" == 1 ]; then
    cd openssl
    [ -f 'Makefile' ] && make clean
    CC="$android_cc" \
    AR="$gcc_prefix-ar" \
    RANLIB="$gcc_prefix-ranlib" \
    ./Configure $openssl_conf $android_cflags $OPENSSL_CONFIG \
      --openssldir="$android_prefix" || exit 1
    # Edit generated Makefile
    sed -i -e \
      '/^CFLAG=/ s/ \+-O[0-3s] \+/ /2; /^MAKEDEPPROG=/ s/^MAKEDEPPROG=.*$/MAKEDEPPROG=$(CC)/; /^DIRS=/ s/^DIRS=.*$/DIRS= crypto/' \
      Makefile
    make depend -j2 || exit 1
    make build_crypto libcrypto.pc libssl.pc openssl.pc -j2 || exit 1
    make install_sw || exit 1
    make clean dclean || exit 1
    cd ..
  fi

  # Build and install sqlcipher
  if [ "$BUILD_SQLCIPHER" == 1 -o "$sqlite3_headers_generated" == 0 ]; then
    cd sqlcipher
    [ -f 'Makefile' ] && make distclean
    CC="$android_cc" \
    CFLAGS="-I$android_prefix/include $android_cflags $SQLCIPHER_CFLAGS" \
    LDFLAGS="-L$android_prefix/lib $SQLCIPHER_LDFLAGS $android_pie" \
    ./configure --host="$gcc_prefix" --prefix="$android_prefix" $SQLCIPHER_CONFIG || exit 1
    
    # Generate sqlite3.c/.h
    if [ "$sqlite3_headers_generated" == 0 ]; then
      make sqlite3.h sqlite3.c || exit 1
      cp sqlite3.h sqlite3.c sqlite3ext.h sqlite3session.h src/sqlcipher.h ../android/sqlcipher/ || exit 1
      sqlite3_headers_generated=1
    fi

    if [ "$BUILD_SQLCIPHER" == 1 ]; then
      make all install || exit 1
    fi

    make distclean || exit 1
    cd ..
  fi

  # Recover original PATH
  export PATH="$original_path"
  
done

# Recover saved directory
popd >/dev/null 2>&1
exit 0
