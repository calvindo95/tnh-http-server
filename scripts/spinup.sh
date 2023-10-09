# Spins up a Docker container

if [ $# -lt 3 ]; then # checks if number of arguments is less than 1
    echo "Less than 1 arguments were supplied"
    echo "Usage: 'sh spinup.sh <image name>' <port> <container name>"
else
    # mounts local current working directory to /home/webserver/source_directory in the container
    docker run -dit -e COLORTERM=truecolor -e HTTP_PORT=$2 -p $2:$2 --name $3 --restart always -v "$(pwd)":/home/webserver/source_directory:z $1
fi
