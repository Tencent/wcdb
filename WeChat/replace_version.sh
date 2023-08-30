#!/bin/bash

basedir=$(dirname "$0")
awk -f "$basedir/parse_version.awk" -v date="$(date +'%Y%m%d')" -v rev_short="$(git rev-parse --short=9 HEAD)" "$basedir/wconan_package.yaml" > "$basedir/../VERSION"
