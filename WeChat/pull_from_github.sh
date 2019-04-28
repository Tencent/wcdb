
root=`git rev-parse --show-toplevel`

branch="WeChat/iOS"
githubBranch="pre_v1.1.0"
git checkout ${branch}
git pull --rebase github ${githubBranch}:${githubBranch}
git rebase ${githubBranch}
git rebase origin/${branch}

sh ${root}/tools/version/bump.sh
git add ${root}/objc/source/utility/Version.h
git commit -m "Re-bump version"
git push origin ${branch}:${branch}
