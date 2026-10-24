#!/usr/bin/env bash
# Runs a command, keeps its output in the job log, and if it fails repeats the last lines of that output as an error
# annotation (visible on the run page and through the API even when the raw job log is not reachable).
#
# usage: bash .github/scripts/ci-run.sh <title> <command> [arguments...]
title=$1
shift
log="$(mktemp)"

"$@" 2>&1 | tee "$log"
status=${PIPESTATUS[0]}

if [ "$status" -ne 0 ]; then
  # workflow commands need % and newlines escaped
  message=$(tail -n 60 "$log" | tr -d '\r' | sed -e 's/%/%25/g' | awk 'BEGIN { ORS = "%0A" } { print }')
  echo "::error title=${title} failed (exit code ${status})::${message}"
fi
exit "$status"
