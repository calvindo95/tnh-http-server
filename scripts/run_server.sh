#!/bin/sh

# START SERVER OPTIONS
#export HTTP_PORT=8081
export MAX_CONNECTIONS=128
export CONNECTION_TIMEOUT=120
export MEMORY_LIMIT=65535
export MAX_THREADS=8
# END SERVER OPTIONS

# START DB SERVER SETTINGS
export DB_IP=192.168.1.7
export DB_USERNAME=admin
export DB_PASSWORD=Strawb3rries
export DB_NAME=humidity_tracker
export DB_PORT=3306
# END DB SERVER SETTINGS

if [ -f ./build/src/tnh-server ];
then
    ./build/src/tnh-server
else
    echo "./build/src/tnh-server not found"
    exit
fi