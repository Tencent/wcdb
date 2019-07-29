#!/bin/bash

root=`git rev-parse --show-toplevel`
cd ${root}

branch="WeChat/iOS"
githubBranch="pre_v1.1.0"

git checkout ${githubBranch}
git submodule update --init sqlcipher
git pull --rebase github ${githubBranch}:${githubBranch}
git submodule update sqlcipher

git checkout ${branch}
git submodule update sqlcipher
git merge ${githubBranch}
