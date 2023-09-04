FROM balenalib/raspberrypi3:bullseye
#FROM debian:10
# Update apt repos
RUN apt update --allow-unauthenticated
RUN apt upgrade -y

# Install packages
RUN apt -y install libmicrohttpd-dev
RUN apt -y install gnutls-bin
RUN apt -y install libssl-dev
RUN apt -y install git
RUN apt -y install curl wget
RUN apt -y install libtool
RUN apt -y install m4
RUN apt -y install cmake make automake
RUN apt -y install g++ clang
RUN apt -y install systemctl
RUN apt -y install libboost-dev 
RUN apt -y install libmariadb-dev
RUN apt -y install nlohmann-json-dev
RUN apt -y install libcurl4-openssl-dev
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

# Install mariadb conn/c++
#RUN wget https://dlm.mariadb.com/2531503/Connectors/cpp/connector-cpp-1.0.2/mariadb-connector-cpp-1.0.2-debian-bullseye-aarch64.tar.gz && \
#    tar -xvzf mariadb-connector-cpp-*.tar.gz && \
#    cd mariadb-connector-cpp-*/ &&\
#    install -d /usr/include/mariadb/conncpp && \
#    install -d /usr/include/mariadb/conncpp/compat && \
#    install include/mariadb/* /usr/include/mariadb/ && \
#    install include/mariadb/conncpp/* /usr/include/mariadb/conncpp && \
#    install include/mariadb/conncpp/compat/* /usr/include/mariadb/conncpp/compat && \
#    install -d /usr/lib/mariadb && \
#    install -d /usr/lib/mariadb/plugin && \
#    install lib/mariadb/libmariadbcpp.so /usr/lib && \
#    install lib/mariadb/plugin/* /usr/lib/mariadb/plugin

# set user; everything below is under user webserver
ENV HOME=/home/webserver
RUN useradd -ms /bin/bash webserver
#USER webserver
WORKDIR /home/webserver