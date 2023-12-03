#!/bin/sh

if [ -f /usr/local/bin/tnh-server ];
then
    /usr/local/bin/tnh-server
else
    echo "/usr/local/bin/tnh-server not found"
    exit
fi