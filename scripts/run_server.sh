#!/bin/sh

if [ -f $TNH_EXEC ];
then
    $TNH_EXEC
else
    echo "$TNH_EXEC not found"
    exit
fi