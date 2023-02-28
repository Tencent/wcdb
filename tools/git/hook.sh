#!/bin/bash

usage() {
    echo "Usage: bash $0 [--uninstall]"
}

if ! git rev-parse --is-inside-work-tree | true; then
    # skip
    exit 0
fi

uninstall=false
while [[ $# -gt 0 ]]
do
key="$1"
case "$key" in
    --uninstall)
    uninstall=true
    shift
    ;;
    *)
    usage
    exit 1
    ;;
esac
done

root=`git rev-parse --show-toplevel`
source="$root"/tools/git
destination="$root"/.git/hooks

hooks=(pre-commit prepare-commit-msg)
for hook in "${hooks[@]}"; do
    target="$destination/$hook"
    rm -f "$target"
    echo "$target is uninstalled."
    if ! $uninstall; then
        ln "$source/$hook" "$target"
        chmod +x "$target"
        echo "$target is installed."
    fi
done
