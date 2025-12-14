#!/usr/bin/env bash
set -e

rm -rf build

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
