#!/bin/bash

if ! git rev-parse --is-inside-work-tree; then
    # skip
    exit 0
fi

root=`git rev-parse --show-toplevel`
template="$root"/tools/git
hooks="$root"/.git/hooks

install() {
    source="$1"
    file=`basename $1`
    target="$hooks/$file"
    rm -f "$target"
    ln "$source" "$target"
    chmod +x "$target"
    echo "$source is installed at $target"
}

install "$template"/pre-commit
install "$template"/prepare-commit-msg