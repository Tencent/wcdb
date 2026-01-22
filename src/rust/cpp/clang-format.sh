#!/usr/bin/env bash

FILES=$(git ls-files '*.c' '*.cpp' '*.h')
for file in $FILES; do
  clang-format -i "$file"
done