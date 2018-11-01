#!/bin/bash

root=`git rev-parse --show-toplevel`
formatter=$root/tools/formatting/format.sh

for file in `git diff-index --cached --diff-filter=ACMRT --name-only HEAD` ; do  
    fullpath=$root/$file
    sh $formatter --add $fullpath
done  
