#!/bin/bash

root=`git rev-parse --show-toplevel`

cp pre-commit $root/.git/hooks/
chmod +x $root/.git/hooks/pre-commit
echo 'pre-commit installed.'