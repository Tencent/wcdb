#!/usr/bin/env bash
echo "Usage: publish_conan.sh -r [remote conan,default tencent-conan] -p platform[ios/android/applesilicon/mac] -v version[1.0.0] -d \"diretory to resources\" -n \"project name\""

dst=''
name=''
plat=''
version=''
remote='tencent-conan'
force=false
publish_ios() {
    echo "conan ios, version: $version"
    conan remote add ${remote} http://mirrors.tencent.com/repository/conan/conan-local
    conan user -p b0e97e4237ca11ec9d426c92bf47000d -r ${remote} g_wxg_upload
    cd ${dst}
    conan new ${name}-ios/${version} --bare
    # cat conanfile.py| sed 's/\"compiler\", //g' > ./conanfile2.py
    cat conanfile.py | sed -e '/settings/d' > ./conanfile2.py
    mv -f conanfile2.py conanfile.py
    conan export-pkg . ${name}-ios/${version}@conan/stable --force
    if [ $force = true ]
    then
        echo "force upload"
        conan upload ${name}-ios/${version}@conan/stable -r ${remote} --all --force
    else
        conan upload ${name}-ios/${version}@conan/stable -r ${remote} --all --no-overwrite
    fi
    if [ $? -ne 0 ]
    then
        exit 1
    fi
}

publish_watch() {
    echo "conan watch, version: $version"
    conan remote add ${remote} http://mirrors.tencent.com/repository/conan/conan-local
    conan user -p b0e97e4237ca11ec9d426c92bf47000d -r ${remote} g_wxg_upload
    cd ${dst}
    conan new ${name}-watch/${version} --bare
    # cat conanfile.py| sed 's/\"compiler\", //g' > ./conanfile2.py
    cat conanfile.py | sed -e '/settings/d' > ./conanfile2.py
    mv -f conanfile2.py conanfile.py
    conan export-pkg . ${name}-watch/${version}@conan/stable --force
    if [ $force = true ]
    then
        echo "force upload"
        conan upload ${name}-watch/${version}@conan/stable -r ${remote} --all --force
    else
        conan upload ${name}-watch/${version}@conan/stable -r ${remote} --all --no-overwrite
    fi
    if [ $? -ne 0 ]
    then
        exit 1
    fi
}

publish_android() {
    echo "conan android, version: $version"
    conan remote add ${remote} http://mirrors.tencent.com/repository/conan/conan-local
    conan user -p b0e97e4237ca11ec9d426c92bf47000d -r ${remote} g_wxg_upload
    cd ${dst}
    conan new ${name}-android/${version} --bare
    cat conanfile.py | sed -e '/settings/d' > ./conanfile2.py
    mv -f conanfile2.py conanfile.py
    conan export-pkg . ${name}-android/${version}@conan/stable --force
    if [ $force = true ]
    then
        echo "force upload"
        conan upload ${name}-android/${version}@conan/stable -r ${remote} --all --force
    else
        conan upload ${name}-android/${version}@conan/stable -r ${remote} --all --no-overwrite
    fi
    if [ $? -ne 0 ]
    then
        exit 1
    fi
}

publish_applesilicon() {
    echo "conan applesilicon, version: $version"
    conan remote add ${remote} http://mirrors.tencent.com/repository/conan/conan-local
    conan user -p b0e97e4237ca11ec9d426c92bf47000d -r ${remote} g_wxg_upload
    cd ${dst}
    conan new ${name}-applesilicon/${version} --bare
    sed -i "" 's/self.copy("\*")/self.copy("\*", symlinks=True)/g' conanfile.py 
    cat conanfile.py | sed -e '/settings/d' > ./conanfile2.py
    mv -f conanfile2.py conanfile.py
    conan export-pkg . ${name}-applesilicon/${version}@conan/stable --force
    if [ $force = true ]
    then
        echo "force upload"
        conan upload ${name}-applesilicon/${version}@conan/stable -r ${remote} --all --force
    else
        conan upload ${name}-applesilicon/${version}@conan/stable -r ${remote} --all --no-overwrite
    fi
    if [ $? -ne 0 ]
    then
        exit 1
    fi
}

publish_mac() {
    echo "conan mac, version: $version"
    conan remote add ${remote} http://mirrors.tencent.com/repository/conan/conan-local
    conan user -p b0e97e4237ca11ec9d426c92bf47000d -r ${remote} g_wxg_upload
    cd ${dst} || exit 1
    conan new ${name}-mac/${version} --bare
    sed -i "" 's/self.copy("\*")/self.copy("\*", symlinks=True)/g' conanfile.py 
    cat conanfile.py | sed -e '/settings/d' > ./conanfile2.py
    mv -f conanfile2.py conanfile.py
    conan export-pkg . ${name}-mac/${version}@conan/stable --force
    if [ $force = true ]
    then
        echo "force upload"
        conan upload ${name}-mac/${version}@conan/stable -r ${remote} --all --force
    else
        conan upload ${name}-mac/${version}@conan/stable -r ${remote} --all --no-overwrite
    fi
    if [ $? -ne 0 ]
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
if [ $plat = "ios" ]
then
    publish_ios
elif [ $plat = "watch" ]
then
    publish_watch
elif [ $plat = "android" ]
then
    publish_android
elif [ $plat = "applesilicon" ]
then
    publish_applesilicon
elif [ $plat = "mac" ]
then
    publish_mac
fi

exit 0
