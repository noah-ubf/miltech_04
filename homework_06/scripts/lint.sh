#!/usr/bin/env bash
set -euo pipefail

ROOT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/.." &> /dev/null && pwd )
echo "Running clang-tidy on C++ sources in ${ROOT}"

clang-tidy -p build/debug $ROOT/include/ballistics.hpp
clang-tidy -p build/debug $ROOT/src/ballistics.cpp
clang-tidy -p build/debug $ROOT/src/main.cpp
clang-tidy -p build/debug $ROOT/tests/ballistics_tests.cpp
