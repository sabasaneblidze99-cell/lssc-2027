#!/bin/bash
# NPC-based smoke test: open.mp NPC "Test_Bot" driven by filterscripts/bot_harness.amx
# Usage: bot_smoke.sh [--fresh] [--intro]   --fresh deletes the Test_Bot account first; --intro removes the intro skip flag
T=/home/claude/testenv; S=$T/server; SOCK=$T/db/mysqld.sock
FRESH=0; INTRO=0
for a in "$@"; do [ "$a" == "--fresh" ] && FRESH=1; [ "$a" == "--intro" ] && INTRO=1; done
$T/start_db.sh >/dev/null || exit 1
if [ $FRESH == 1 ]; then
  ACC=$(mariadb --socket=$SOCK -uroot lsrp -Nse "SELECT id FROM accounts WHERE name='Test_Bot'")
  if [ -n "$ACC" ]; then
    mariadb --socket=$SOCK -uroot lsrp -e "DELETE FROM characters WHERE account_id=$ACC; DELETE FROM accounts WHERE id=$ACC;" 2>&1  # FK cascades remove story/inventory rows
  fi
fi
if [ $INTRO == 1 ]; then rm -f $S/scriptfiles/lsrp_intro_skip.flag; else touch $S/scriptfiles/lsrp_intro_skip.flag; fi
python3 - <<PY
import json; p='$S/config.json'; d=json.load(open(p)); d['pawn']['side_scripts']=['filterscripts/bot_harness']; json.dump(d,open(p,'w'),indent=4)
PY
cd $S; pkill -f "$S/omp-server"; sleep 1; : > log.txt; : > scriptfiles/lsrp.log 2>/dev/null
export LD_LIBRARY_PATH=$S
./omp-server > console.out 2>&1 &
PID=$!
for i in $(seq 180); do
  sleep 1
  grep -qE "\[BOT\] (done|FAIL|TIMEOUT|bot not connected)" console.out && break
done
sleep 4; kill -INT $PID; sleep 3
python3 - <<PY
import json; p='$S/config.json'; d=json.load(open(p)); d['pawn']['side_scripts']=[]; json.dump(d,open(p,'w'),indent=4)
PY
echo "===== console (filtered) ====="
grep -vE "Loading component|Successfully loaded component" console.out
echo "===== plugin logs ====="; for f in logs/*.log; do echo "-- $f"; cat $f; done
echo "===== DB rows ====="
mariadb --socket=$SOCK -uroot lsrp -e "SELECT id,name,hash_algo,LEFT(password_hash,7) h,failed_logins,intro_seen FROM accounts WHERE name='Test_Bot'\G; SELECT c.* FROM characters c JOIN accounts a ON a.id=c.account_id WHERE a.name='Test_Bot'\G"
