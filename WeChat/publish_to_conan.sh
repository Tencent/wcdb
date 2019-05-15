#!/usr/bin/env bash

if brew ls conan > /dev/null; then
    echo "conan installed"
else
    echo "conan not installed"
    brew install conan
fi

echo "Usage: publish_conan.sh -r [remote conan,default conan-mmclient] -p platform[ios/android] -v version[1.0.0] -d \"diretory to resources\" -n \"project name\""

dst=''
name=''
plat=''
version=''
remote='conan-mmclient'
force=false
publish_ios() {
    echo "conan ios, version: $version"
    conan remote add ${remote} http://conan.weixin.oa.com/artifactory/api/conan/${remote}
    conan user -p mars@123 -r ${remote} mars
    cd ${dst}
    conan new ${name}-ios/${version} --bare
    cat conanfile.py| sed 's/\"compiler\", //g' > ./conanfile2.py
    mv -f conanfile2.py conanfile.py
    conan export-pkg . ${name}-ios/${version}@conan/stable --force
    if [[ $force == true ]]
    then
        echo "force upload"
        conan upload ${name}-ios/${version}@conan/stable -r ${remote} --all --force
    else
        conan upload ${name}-ios/${version}@conan/stable -r ${remote} --all --no-overwrite
    fi
    if [[ $? -ne 0 ]]
    then
	    exit 1
    fi
}

publish_android() {
    echo "conan android, version: $version"
    conan remote add ${remote} http://conan.weixin.oa.com/artifactory/api/conan/${remote}
    conan user -p mars@123 -r ${remote} mars
    cd ${dst}
    conan new ${name}-android/${version} --bare
    conan export-pkg . ${name}-android/${version}@conan/stable --force
    if [[ $force == true ]]
    then
        echo "force upload"
        conan upload ${name}-android/${version}@conan/stable -r ${remote} --all --force
    else
        conan upload ${name}-android/${version}@conan/stable -r ${remote} --all --no-overwrite
    fi
    if [[ $? -ne 0 ]]
    then
	    exit 1
    fi
}

while getopts "r:p:v:d:n:f" optname
do
    case "$optname" in
        p)
            plat=$OPTARG
            ;;
        v)
            version=$OPTARG
            ;;
        f)
            force=true
            ;;
        d)
            dst=$OPTARG
            ;;
        n)
            name=$OPTARG
            ;;
        r)
            remote=$OPTARG
            ;;
        ?)
            echo "argument error"
            exit 1
            ;;
    esac
done
echo "platform: $plat, version:$version, force:$force, dst:$dst, name:$name"
if [[ $plat == "ios" ]]
then
    publish_ios
elif [[ $plat == "android" ]]
then
    publish_android
fi

exit 0
