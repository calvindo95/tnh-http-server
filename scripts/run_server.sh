#!/bin/sh

if [ -f ./build/src/tnh-server ];
then
    ./build/src/tnh-server
else
    echo "./build/src/tnh-server not found"
    exit
fi