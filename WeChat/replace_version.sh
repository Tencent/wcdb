#!/bin/bash

basedir=$(dirname "$0")
pushd "$basedir"
awk -f parse_version.awk -v date="$(date +'%Y%m%d')" -v rev_short="$(git rev-parse --short=8 HEAD)" wconan_package.yaml > ../VERSION
popd
