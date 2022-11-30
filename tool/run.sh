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

    clang++ -cc1 -load $LIB -plugin hamsa \
    -plugin-arg-hamsa -output-format -plugin-arg-hamsa $FORMAT \
    -plugin-arg-hamsa -output-file -plugin-arg-hamsa $OUTPUT \
    $INPUT
fi