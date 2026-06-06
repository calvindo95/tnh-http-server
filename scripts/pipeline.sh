if [ $# -lt 0 ]; then # checks if number of arguments is less than 2
    echo "Less than 1 arguments were supplied"
    echo "Usage: 'sh pipeine.sh"
    return 1
fi

build_main_img () {
    sh scripts/build_img.sh scripts/Dockerfile_arm tnh-server-arm
}

build_base_img() {
    sh scripts/build_img.sh scripts/Dockerfile_arm_base tnh-base-arm
}

restart_container(){
    if $(docker ps | grep -q tnh-server)
    then
        echo "tnh-server still active, stopping."
        sh scripts/stop.sh tnh-server
    else
        echo "tnh-server already stopped."
    fi

    yes | docker container prune
    yes | docker image prune

    # Usage: 'sh spinup.sh <image name>' <port> <container name>
    sh scripts/spinup.sh tnh-server-arm 8081 tnh-server
}

##### MAIN #####
while getopts "a" opt; do
    case "${opt}" in
        a) 
            echo "Building Base img"
            build_base_img;;
    esac
done

build_main_img
restart_container