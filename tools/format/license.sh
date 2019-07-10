#!/bin/bash

usage() {    
    echo "Usage: sh $0 -p/--path path"
}

path=""
while [[ $# -gt 0 ]]
do
key="$1"
case "$key" in
    -p|--path)
    path="$2"
    shift
    shift
    ;;
    *)
    usage
    exit 1
    ;;
esac
done

# check parameters
if [[ ! -a "$path" ]]; then
    usage
    exit 1    
fi
extension=${path##*.}

license='''/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */'''

if [ $extension = "h" ] || [ $extension = "hpp" ] || [ $extension = "c" ] || [ $extension = "cpp" ] || [ $extension = "m" ] || [ $extension = "mm" ] || [ $extension = "swift" ]; then
    fullpath="$root/$path"
    content=`cat $path`
    if [[ $content != "$license"* ]];
    then
        echo "Error: License declaretion in $path is not correct."
        exit 1
    fi
fi
