# Attaches to the first Docker container

export CONTAINER=`docker ps | tail -1 | awk '{print $1}'`

docker exec -it $CONTAINER /bin/bash