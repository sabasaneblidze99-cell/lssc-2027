#!/bin/bash
# Starts omp-server headless in /home/claude/testenv/server. Usage: start_server.sh [seconds]  (default: run until killed)
S=/home/claude/testenv/server
cd $S || exit 1
pkill -f "$S/omp-server" 2>/dev/null; sleep 1
: > log.txt
export LD_LIBRARY_PATH=$S:$LD_LIBRARY_PATH
if [ -n "$1" ]; then
  timeout -s INT "$1" ./omp-server > console.out 2>&1
else
  nohup ./omp-server > console.out 2>&1 &
  echo "omp-server pid $!"
fi
