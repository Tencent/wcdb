#!/bin/bash

usage() {    
    echo "Usage: sh $0 [path]"
}

file=${1}
if [[ ! -a $file ]]; then
    usage
    exit 1    
fi
extension=${file##*.}

# correct file macro
if [ $extension = "h" ] || [ $extension = "hpp" ] || [ $extension = "c" ] || [ $extension = "cpp" ]; then
    formatting=true
    fileMacro=`basename $file | sed "s/[^[:alnum:]_]/_/g"`
    awk -v fileMacro=${fileMacro} '''{ 
        output=$0
        newFileMacro="_WCDB_" toupper(fileMacro)
        if ($0=="#ifndef " fileMacro) {
            output="#ifndef " newFileMacro
        }else if ($0=="#define " fileMacro) {
            output="#define " newFileMacro
        }else if ($0=="#endif \/\* " fileMacro " \*\/") {
            output="#endif /* " newFileMacro " */"
        }
        print output
    }''' $file > tmp
    mv tmp $file    
    echo "File macro of ${file} is formatted."
else
    echo "Skip Formatting for ${file}"
fi
