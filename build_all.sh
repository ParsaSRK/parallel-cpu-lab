#!/usr/bin/env bash

set -e

for preset in debug release asan-ubsan tsan relwithdebinfo; do
    cmake --preset "$preset"
    cmake --build --preset "$preset"
done
