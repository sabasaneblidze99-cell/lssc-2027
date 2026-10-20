#!/bin/bash
# Compila il gamemode.
#   scripts/build.sh          -> gamemodes/lsrp.amx            (con CEF, richiede cef.dll/cef.so)
#   scripts/build.sh --nocef  -> gamemodes/lsrp_senza_cef.amx  (solo dialog/TextDraw)
# Variabile PAWNCC = percorso di pawncc 3.10.10 (default: pawncc nel PATH).
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PAWNCC="${PAWNCC:-pawncc}"
DEFS="LSRP_CEF=1"; OUT="lsrp.amx"
if [ "$1" == "--nocef" ]; then DEFS=""; OUT="lsrp_senza_cef.amx"; fi
cd "$ROOT/gamemodes" || exit 1
"$PAWNCC" lsrp.pwn -d3 $DEFS \
  -i"$ROOT/toolchain/include/omp-stdlib" \
  -i"$ROOT/toolchain/include/mysql" \
  -i"$ROOT/toolchain/include/cef" \
  -i"$ROOT/toolchain/include/bcrypt" \
  -i. -o"$ROOT/gamemodes/$OUT"
