#!/usr/bin/env bash
if brew ls conan > /dev/null; then
    echo "conan installed"
else
    echo "conan not installed"
    brew install conan
fi

dst=''
name=''
plat=''
version=''
force=false
publish_ios() {
    echo "conan ios, version: $version"
    conan user -p releaser@123 -r conan-mmclient releaser
    conan remote add conan-mmclient http://conan.weixin.oa.com/artifactory/api/conan/conan-mmclient
    cd ${dst}
    conan new ${name}-ios/${version} --bare
    conan export-pkg . ${name}-ios/${version}@conan/stable --force
    if [[ $force == true ]]
    then
        echo "force upload"
        conan upload ${name}-ios/${version}@conan/stable -r conan-mmclient --all --force
    else
        conan upload ${name}-ios/${version}@conan/stable -r conan-mmclient --all --no-overwrite
    fi
}

publish_android() {
    echo "conan android, version: $version"
    conan user -p releaser@123 -r conan-mmclient releaser
    conan remote add conan-mmclient http://conan.weixin.oa.com/artifactory/api/conan/conan-mmclient
    cd ${dst}
    conan new ${name}-android/${version} --bare
    conan export-pkg . ${name}-android/${version}@conan/stable --force
    if [[ $force == true ]]
    then
        echo "force upload"
        conan upload ${name}-android/${version}@conan/stable -r conan-mmclient --all --force
    else
        conan upload ${name}-android/${version}@conan/stable -r conan-mmclient --all --no-overwrite
    fi
}

while getopts "p:v:d:n:f" optname
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
        ?)
            echo "argument error"
            exit 1
            ;;
    esac
done
echo "platform: $plat, version:$version, force:$force, dst:$dst, name:$name"
if [ $plat == "ios" ]
then
    publish_ios
elif [ $plat == "android" ]
then
    publish_android
fi

exit 0
