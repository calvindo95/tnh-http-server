sudo sh scripts/build_img.sh scripts/Dockerfile_arm tnh-server-arm && \
sudo sh scripts/stop.sh tnh-server && \
sudo docker container prune && \
sudo sh scripts/spinup.sh tnh-server-arm 8081 tnh-server $1
