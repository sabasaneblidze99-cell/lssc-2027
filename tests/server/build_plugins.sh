#!/bin/bash
# Rebuilds the 32-bit plugins and gamemode used by the test server (already done once; idempotent).
# Prereqs (apt): gcc-multilib g++-multilib cmake libboost-dev libmysqlclient-dev:i386 rustc cargo libstd-rust-dev:i386
# Source patches applied in src/ (testenv only):
#  - SA-MP-MySQL/src/CConnection.cpp: my_bool -> bool (MySQL 8 client removed my_bool)
#  - SA-MP-MySQL/libs/samp-log-core/src/amx/amx.c:1714: 'goto **cip++' -> 'goto *(void *)*cip++' (modern GCC)
#  - cmake-shim/: replacements for Zeex/cmake-modules (AMXConfig, AddSAMPPlugin, FindSAMPSDK) - repo not clonable
set -e
T=/home/claude/testenv; S=$T/server
cd $T/src/SA-MP-MySQL && mkdir -p build && cd build
cmake .. -DCMAKE_MODULE_PATH=$T/cmake-shim -DCMAKE_BUILD_TYPE=Release >/dev/null && make -j8 >/dev/null
cd $T/src/samp-bcrypt && CARGO_NET_GIT_FETCH_WITH_CLI=true /usr/bin/cargo build --release --target i686-unknown-linux-gnu
mkdir -p $S/plugins
cp $T/src/SA-MP-MySQL/build/src/mysql.so $S/plugins/
cp $T/src/SA-MP-MySQL/build/libs/samp-log-core/src/log-core.so $S/
cp $T/src/samp-bcrypt/target/i686-unknown-linux-gnu/release/libsamp_bcrypt.so $S/plugins/samp_bcrypt.so
# gamemode (no-CEF), compiled into testenv without touching the repo
R=/home/claude/lssc-2027; cd $R/gamemodes
/home/claude/tools/pawn-src/build/pawncc lsrp.pwn -d3 LSRP_NO_CEF=1 -i$R/toolchain/include/omp-stdlib -i$R/toolchain/include/mysql \
  -i$R/toolchain/include/cef -i$R/toolchain/include/bcrypt -i$R/toolchain/include/sampvoice -i. -o$S/gamemodes/lsrp_senza_cef.amx | tail -2
# NPC harness filterscript
/home/claude/tools/pawn-src/build/pawncc $T/harness/bot_harness.pwn -d3 -i$R/toolchain/include/omp-stdlib -o$S/filterscripts/bot_harness.amx | tail -1
