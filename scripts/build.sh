#!/bin/bash
# Compila il gamemode. Uso: scripts/build.sh [pawncc]
# Richiede pawncc 3.10.10 (open.mp compiler).
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PAWNCC="${1:-${PAWNCC:-pawncc}}"
cd "$ROOT/gamemodes" || exit 1
"$PAWNCC" lsrp.pwn -d3 \
  -i"$ROOT/toolchain/include/omp-stdlib" \
  -i"$ROOT/toolchain/include/mysql" \
  -i"$ROOT/toolchain/include/cef" \
  -i"$ROOT/toolchain/include/bcrypt" \
  -i. -olsrp.amx
