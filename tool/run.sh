#!/usr/bin/env bash

LIB="./build/src/libLoopInfoTool.so"

INPUT=$1
OUTPUT=$2
FORMAT=$3

mkdir -p output

clang++ -cc1 -load $LIB -plugin hamsa \
-plugin-arg-hamsa -output-format -plugin-arg-hamsa $FORMAT \
-plugin-arg-hamsa -output-file -plugin-arg-hamsa $OUTPUT \
$INPUT