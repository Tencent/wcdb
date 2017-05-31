#!/bin/bash

find ../WCDB/ \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.mm" \) | xargs clang-format -i
