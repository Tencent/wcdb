#!/bin/bash

root=`git rev-parse --show-toplevel`
formatter=$root/tools/code_formatting/code_formatting_one.sh

for file in `git diff-index --cached --diff-filter=ACMRT --name-only HEAD` ; do  
    fullpath=$root/$file
    sh $formatter $fullpath
done  
