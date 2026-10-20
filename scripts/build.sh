#!/bin/bash
# Compila il gamemode.
#   scripts/build.sh          -> gamemodes/lsrp.amx            (con CEF, richiede cef.dll/cef.so)
#   scripts/build.sh --nocef  -> gamemodes/lsrp_senza_cef.amx  (solo dialog/TextDraw)
#   scripts/build.sh --voice  -> gamemodes/lsrp_voce.amx       (CEF + chat vocale SampVoice: serve il plugin)
# Variabile PAWNCC = percorso di pawncc 3.10.10 (default: pawncc nel PATH).
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PAWNCC="${PAWNCC:-pawncc}"
DEFS=""; OUT="lsrp.amx"
# default: lsrp.amx = CEF + voce (il gamemode ufficiale). --test = senza CEF (solo per il server di prova Linux)
if [ "$1" == "--test" ]; then DEFS="LSRP_NO_CEF=1"; OUT="lsrp_test.amx"; fi
cd "$ROOT/gamemodes" || exit 1
# file unico per Qawno, sempre allineato ai moduli
python3 "$ROOT/scripts/flatten.py" lsrp.pwn lsrp_completo.pwn && sed -i '1i // lsrp_completo.pwn - GENERATO da scripts/flatten.py (lsrp.pwn + modules/) in un solo file per Qawno.\n// Non modificare qui: modificare lsrp.pwn/modules e rigenerare.' lsrp_completo.pwn
"$PAWNCC" lsrp.pwn -d3 $DEFS \
  -i"$ROOT/toolchain/include/omp-stdlib" \
  -i"$ROOT/toolchain/include/mysql" \
  -i"$ROOT/toolchain/include/cef" \
  -i"$ROOT/toolchain/include/bcrypt" \
  -i"$ROOT/toolchain/include/sampvoice" \
  -i. -o"$ROOT/gamemodes/$OUT"
