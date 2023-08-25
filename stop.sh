export CONTAINER=`docker ps | tail -1 | awk '{print $1}'`

docker stop $CONTAINER