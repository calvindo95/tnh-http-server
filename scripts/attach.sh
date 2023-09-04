# Attaches to the first Docker container

if [ $# -lt 1 ]; then # checks if number of arguments is less than 1
    echo "Less than 1 arguments were supplied"
    echo "Usage: 'sh attach.sh <container id>'"
else
    docker exec -it $1 /bin/bash
fi