#!/bin/bash

root=`git rev-parse --show-toplevel`
hooks=$root/tools/git
gitHooks=$root/.git/hooks

install() {
    source=$1
    filename=`basename $1`
    target=$gitHooks/$filename
    rm -f $target
    ln $source $target
    chmod +x $target
    echo "$source is installed at $target"
}

install $hooks/pre-commit
install $hooks/prepare-commit-msg