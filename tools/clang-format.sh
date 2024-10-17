#!/bin/bash

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$(dirname "$script_dir")"

find $project_dir/src \
        -iname '*.h' \
        -o -iname '*.c' \
        -o -iname '*.hpp' \
        -o -iname '*.cpp' \
    | xargs clang-format \
        --style=file:$project_dir/.clang-format \
        -i
