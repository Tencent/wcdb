#!/bin/bash

# 检查是否提供了足够的命令行参数
if [ $# -lt 3 ]; then
    echo "Usage: $0 target_directory base_directory patterns_file"
    exit 1
fi

# 使用符号链接的目标目录
target_dir="$1"

# 基础路径
base_directory="$2"

# 从文件中读取路径模式
patterns_file="$3"
path_patterns=()
while IFS= read -r line || [[ -n "$line" ]]; do
    # 使用正则表达式匹配引号中的内容，如果没有引号则匹配整行内容
    if [[ $line =~ \"([^\"]*)\" ]]; then
        pattern="${BASH_REMATCH[1]}"
    else
        pattern="$line"
    fi
    path_patterns+=("$pattern")
done < "$patterns_file"

# 初始化头文件数组
header_files=()

# 使用 find 命令查找匹配的路径模式，并将它们添加到数组中
for pattern in "${path_patterns[@]}"; do
    files=()
    if [[ $pattern == *"*"* ]]; then
        # 如果路径模式包含通配符，处理 {h,hpp} 部分
        if [[ $pattern == *\{*\}* ]]; then
            prefix="${pattern%%\{*\}}"
            suffixes="${pattern#*\{}"
            suffixes="${suffixes%%\}*}"
            suffixes=(${suffixes//,/ })
            postfix="${pattern#*\}}"
            for suffix in "${suffixes[@]}"; do
                new_pattern="${prefix}${suffix}${postfix}"
                while IFS= read -r -d '' file; do
                    # 跳过在 target_dir 中的文件
                    if [[ ! $file =~ ^$target_dir ]]; then
                        files+=("$file")
                    fi
                done < <(find -L "$base_directory" -type f -path "${base_directory}/${new_pattern}" -print0)
            done
        else
            while IFS= read -r -d '' file; do
                # 跳过在 target_dir 中的文件
                if [[ ! $file =~ ^$target_dir ]]; then
                    files+=("$file")
                fi
            done < <(find -L "$base_directory" -type f -path "${base_directory}/${pattern}" -print0)
        fi
    else
        # 如果路径模式不包含通配符，直接查找文件
        file="$base_directory/$pattern"
        if [[ -f $file && ! $file =~ ^$target_dir ]]; then
            files+=("$file")
        fi
    fi
    if [ ${#files[@]} -eq 0 ]; then
        echo "No files found for pattern: $pattern"
    else
        header_files+=("${files[@]}")
    fi
done

# 清空 target_dir 中的文件
rm -rf "$target_dir"/*
mkdir -p "$target_dir"

# 为数组中的每个头文件创建符号链接
for file in "${header_files[@]}"; do
    # 获取文件的基本名（不包含路径）
    base=$(basename "$file")
    # 使用 Python3 计算相对路径
    relative_path=$(python3 -c "import os.path; print(os.path.relpath('$file', '$target_dir'))")
    # 创建符号链接
    ln -s "$relative_path" "$target_dir/$base"
done