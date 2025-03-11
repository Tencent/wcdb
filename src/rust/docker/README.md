# 0. 前提
问题1：为什么需要 docker？

问题2：docker 的镜像和容器什么关系？

# 1. 编写 Dockerfile

目的：搭建 docker 镜像

文件内容：一堆的 shell 命令集合，用于安装各种插件、工具

## 1.1. 决定使用哪个基础镜像

Ubuntu 基础镜像: 空白 ubuntu

node 基础镜像: ubuntu + node

rust 基础镜像: ubuntu + rust

## 1.2. 安装具体的插件/工具（可选）
基础镜像能满足条件，就可以不安装插件。

需要：确定插件是哪个环境的。

例如 clang-format 是 Ubuntu 操作系统的

例如 commitlint 是 node 的

例如 tarpaulin 是 rust 的



# 2. 编译镜像

编译 x86_64 架构的镜像，因为：gitlab 是 x86_64 架构的。花费很长时间
```shell
# 在 Dockerfile 同级目录下执行
docker build --platform=linux/amd64 -t my-rust-nightly .
```

日志中会记录每一行配置的耗时
```text
$ docker build --platform=linux/amd64 -t my-rust-nightly .
[+] Building 2910.0s (16/16) FINISHED                                                              docker:desktop-linux
 => [internal] load build definition from Dockerfile                                                               0.0s
 => => transferring dockerfile: 733B                                                                               0.0s
 => [internal] load metadata for docker.io/rustlang/rust:nightly                                                   0.2s
 => [internal] load .dockerignore                                                                                  0.0s
 => => transferring context: 2B                                                                                    0.0s
 => [ 1/12] FROM docker.io/rustlang/rust:nightly@sha256:794df8defd490aee4c9cf3532d79a5b1847404ff18ee4916638caa12f  0.0s
 => CACHED [ 2/12] RUN rustup component add rustfmt                                                                0.0s
 => CACHED [ 3/12] RUN cargo install --locked cargo-tarpaulin                                                      0.0s
 => CACHED [ 4/12] RUN apt-get update                                                                              0.0s
 => CACHED [ 5/12] RUN apt-get install -y clang-format                                                             0.0s
 => CACHED [ 6/12] RUN apt-get install colordiff                                                                   0.0s
 => CACHED [ 7/12] RUN apt-get install -y cmake                                                                    0.0s
 => [ 8/12] RUN apt-get install -y nodejs npm                                                                   2879.2s
 => [ 9/12] RUN npm config set registry https://registry.npmmirror.com                                             3.5s
 => [10/12] RUN npm install -g @commitlint/cli@19.8.0 @commitlint/config-conventional@19.8.0                      19.6s
 => [11/12] RUN npm install -g autocorrect                                                                         6.0s
 => [12/12] RUN echo '#!/bin/bash' > /usr/local/bin/autocorrect &&     echo 'exec node $(npm root -g)/autocorrect  0.2s
 => exporting to image                                                                                             1.2s
 => => exporting layers                                                                                            1.2s
 => => writing image sha256:459c729326e2b54597dd766257c81229a94ded046658850c9c1deb337f061de2                       0.0s
 => => naming to docker.io/library/my-rust-nightly                                                                 0.0s                                                           0.0s                                                              0.0s
```

# 3. 检查 docker 环境

```shell
# 运行容器
docker run -it -d my-rust-nightly
```
 
 
```shell
# 查看容器列表
docker container ls
```

```shell
# 进入 docker 内部 bash 环境，检查各种插件安装是否正常，此处为容器 id
docker exec -it 0841a23747c7 /bin/bash
```
# 4. 提交镜像并推送镜像

```shell
# 提交修改，此处为容器 id
docker commit 1bc1eb154052 harbor.rongcloud.net/library/rust/wcdb:0.0.1

# 如需创建新标签
docker tag harbor.rongcloud.net/library/rust/wcdb:0.0.1 harbor.rongcloud.net/library/rust/wcdb:0.0.2
```

```shell
# 登录融云 docker hub
docker logout
docker login -u jenkins -p xxxx harbor.rongcloud.net

# 推送刚刚生成的 image
docker push harbor.rongcloud.net/library/rust/wcdb:0.0.1
```

# 5. 其他命令

```shell
# 查看容器列表
docker container ls
# 停止容器
docker container stop bafa301a4514
```

```shell
# 查看镜像列表
docker images
# 删除镜像
docker rmi -f bdf979d715dd
```

```shell
# 查看镜像信息
docker image inspect harbor.rongcloud.net/library/rust/wcdb:0.1.0
# 查看镜像信息，镜像 id
docker image inspect f765eaf5084c
```