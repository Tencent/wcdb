# 选择 Rust 的 nightly 版本
FROM rustlang/rust:nightly

# 安装 Rust 插件
RUN rustup component add rustfmt
RUN cargo install --locked cargo-tarpaulin

RUN apt-get update
RUN apt-get install -y clang-format
RUN apt-get install colordiff
RUN apt-get install -y cmake

# 安装 node 插件
RUN apt-get install -y nodejs npm
RUN npm config set registry https://registry.npmmirror.com
RUN npm install -g @commitlint/cli@19.8.0 @commitlint/config-conventional@19.8.0

# https://github.com/huacnlee/autocorrect
RUN npm install -g autocorrect
RUN echo '#!/bin/bash' > /usr/local/bin/autocorrect && \
    echo 'exec node $(npm root -g)/autocorrect/index.js "$@"' >> /usr/local/bin/autocorrect && \
    chmod +x /usr/local/bin/autocorrect