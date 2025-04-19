# 选择 Rust 的 nightly 版本
FROM rustlang/rust:nightly

# 安装 Rust 插件
RUN rustup component add rustfmt && \
    cargo install --locked cargo-tarpaulin && \
    apt-get update && \
    apt-get install -y --no-install-recommends clang-format colordiff cmake nodejs npm && \
    rm -rf /var/lib/apt/lists/* && \
    npm config set registry https://registry.npmmirror.com && \
    npm install -g --depth=1 @commitlint/cli@19.8.0 @commitlint/config-conventional@19.8.0 autocorrect && \
    echo '#!/bin/bash' > /usr/local/bin/autocorrect && \
    echo 'exec node $(npm root -g)/autocorrect/index.js "$@"' >> /usr/local/bin/autocorrect && \
    chmod +x /usr/local/bin/autocorrect