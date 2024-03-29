if [ $# -lt 0 ]; then # checks if number of arguments is less than 2
    echo "Less than 1 arguments were supplied"
    echo "Usage: 'sh pipeine.sh"
    return 1
fi


build_main_img () {
    echo "tnh-base-arm img exists, building tnh-server-arm img"
    sh scripts/build_img.sh scripts/Dockerfile_arm tnh-server-arm

    if $(docker ps | grep -q tnh-server)
    then
        echo "tnh-server still active, stopping."
        sh scripts/stop.sh tnh-server
    else
        echo "tnh-server already stopped."
    fi

    yes | docker container prune

    # Usage: 'sh spinup.sh <image name>' <port> <container name>
    sh scripts/spinup.sh tnh-server-arm 8081 tnh-server
}

if $(docker image ls | grep -q tnh-base-arm)
then
    build_main_img
else
    echo "tnh-base-arm img does not exist, building base img"
    sh scripts/build_img.sh scripts/Dockerfile_arm_base tnh-base-arm

    build_main_img
fi