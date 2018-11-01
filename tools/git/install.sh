#!/bin/bash

root=`git rev-parse --show-toplevel`
hooks=$root/.git/hooks/

install() {
    cp $1 $hooks
    chmod +x $hooks/$1
    echo "$1 is installed at $hooks"
}

install pre-commit
install prepare-commit-msg