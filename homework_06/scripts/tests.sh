#!/usr/bin/env bash
set -euo pipefail

ROOT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/.." &> /dev/null && pwd )
BUILD_DIR=$( cd -- "$ROOT/../build/debug" &> /dev/null && pwd )

echo "[tests.sh] Configuring and building tests..."
cmake --preset debug
cmake --build --preset debug

echo "[tests.sh] Running ballistics_tests..."
"$BUILD_DIR/homework_06/ballistics_tests"
RESULT=$?

if [ $RESULT -eq 0 ]; then
  echo "[tests.sh] All tests passed."
else
  echo "[tests.sh] Some tests failed."
fi
exit $RESULT
