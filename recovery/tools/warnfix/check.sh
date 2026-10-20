#!/bin/bash
# compila con 3.10.11 (conteggio warning) e 3.10.10 (confronto bytecode con la base)
cd /home/claude/lssc-2027/gamemodes
I="-i../toolchain/include/omp-stdlib -i../toolchain/include/mysql -i../toolchain/include/cef -i../toolchain/include/bcrypt -i../toolchain/include/sampvoice -i."
LD_LIBRARY_PATH=/home/claude/tools/omp-compiler/build /home/claude/tools/omp-compiler/build/pawncc lsrp.pwn -d3 $I -o/home/claude/wfix/w311.amx > /home/claude/wfix/w.txt 2>&1
grep -E " error " /home/claude/wfix/w.txt | head -20
echo "warnings: $(grep -c ' warning ' /home/claude/wfix/w.txt)  errors: $(grep -c ' error ' /home/claude/wfix/w.txt)"
/home/claude/tools/pawn-src/build/pawncc lsrp.pwn -d3 $I -o/home/claude/wfix/new.amx > /dev/null 2>&1
cd /home/claude/tools/amx && python3 cmp.py /home/claude/wfix/base.amx /home/claude/wfix/new.amx 2>&1 | tail -${1:-3}
