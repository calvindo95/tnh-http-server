#!/bin/sh

# START SERVER OPTIONS
export HTTP_PORT=8081
export MAX_CONNECTIONS=32
export CONNECTION_TIMEOUT=120
export MEMORY_LIMIT=65535
export MAX_THREADS=8
# END SERVER OPTIONS

# START DB SERVER SETTINGS
export DB_IP=192.168.1.174
export DB_USERNAME=admin
export DB_PASSWORD=Strawb3rries
export DB_NAME=humidity_tracker
export DB_PORT=3306
# END DB SERVER SETTINGS

cd /home/webserver/source_directory

if [ -f ./build/src/server ];
then
    ./build/src/server
else
    echo "./build/src/server not found"
    exit
fi