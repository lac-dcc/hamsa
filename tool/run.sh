#!/usr/bin/env bash

LIB="./build/src/libLoopInfoTool.so"

if [ $# -lt 3 ]
then
    echo 'Syntax: run input_file output_file format'
    exit 1
else
    INPUT=$1
    OUTPUT=$2
    FORMAT=$3

    mkdir -p output

    clang++ -fsyntax-only -Xclang -load -Xclang $LIB -Xclang -plugin -Xclang hamsa \
    -Xclang -plugin-arg-hamsa -Xclang -output-format -Xclang -plugin-arg-hamsa -Xclang $FORMAT \
    -Xclang -plugin-arg-hamsa  -Xclang -output-file -Xclang -plugin-arg-hamsa -Xclang $OUTPUT \
    $INPUT
fi