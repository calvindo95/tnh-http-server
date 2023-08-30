# Spins up a Docker container

# sets image name
export docker_img="dockerdev"

# mounts local current working directory to /home/webserver/source_directory in the container
docker run -dit -e COLORTERM=truecolor --network host -v "$(pwd)":/home/webserver/source_directory:z $docker_img bash
