#!/usr/bin/env bash
set -euo pipefail

ROOT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/.." &> /dev/null && pwd )
echo "Formatting C++ sources in ${ROOT}"

clang-format -i $ROOT/include/ballistics.hpp $ROOT/src/ballistics.cpp $ROOT/src/main.cpp	$ROOT/tests/ballistics_tests.cpp
