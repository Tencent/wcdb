#!/bin/bash

find ../ \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.mm" -or -name "*.m" \) | xargs clang-format -i
find ../../repair/ \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | xargs clang-format -i
