FROM debian:bullseye

# Update apt repos
RUN apt update --allow-unauthenticated
RUN apt upgrade -y

# Install packages
RUN apt -y install libmicrohttpd-dev
RUN apt -y install git
RUN apt -y install curl wget
RUN apt -y install libtool
RUN apt -y install cmake make automake
RUN apt -y install g++ clang
RUN apt -y install libboost-dev libboost-all-dev
RUN apt -y install libmariadb-dev
#RUN apt -y install nlohmann-json-dev
RUN apt -y install vim

# install libhttpserver
RUN cd /tmp && \
    git clone https://github.com/etr/libhttpserver.git && \
    cd libhttpserver && \
    ./bootstrap && \
    mkdir build && \
    cd build && \
    ../configure && \
    make -j4 && \
    make install && \
    ldconfig -v

# set env variables otherwise libhttpserver doesn't work
ENV LD_LIBRARY_PATH=/usr/local/lib

# set user; everything below is under user webserver
ENV HOME=/home/webserver
RUN useradd -ms /bin/bash webserver
USER webserver
WORKDIR /home/webserver

RUN git clone -b develop https://github.com/calvindo95/tnh-http-server.git && \
    cd tnh-http-server/include && wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp && cd .. && \
    mkdir build && cd build && \
    cmake .. && \
    cmake --build . -j4

WORKDIR /home/webserver/tnh-http-server

CMD ["/home/webserver/tnh-http-server/scripts/run_server.sh"]