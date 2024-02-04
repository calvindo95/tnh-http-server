# https-server-cd
# 1. Setup Environment
## 1.1 Clone HTTPS server repo
Clone and enter the https server repository
```
git clone https://github.com/calvindo95/tnh-http-server.git
cd tnh-http-server
```

## 1.2 Build docker image
Build the docker image with the following build script:
```
sh scripts/pipeline.sh <DB_PASSWORD>
```
The script will build 2 docker images, a base image and main imge. The base image includes buildtools for building the tnh-server binary, and is meant to reduce build times on low powered machines. The script will then spinup a container using the DB_PASSWORD to connect to mariadb/mysql DB (using tnh_server as the DB username)

# 2. Build and Run the webserver
## 2.1 Customizing server options
The server uses ENV variables and the settings.json file as its source of server options. The settings.json file contains all available server options, however any ENV variables that are set will take precedent over the json file.