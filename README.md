# https-server-cd
# 1. Setup Environment
## 1.1 Clone PKI repo and create CA certificates
First we will need to clone the PKI repo
```
cd ~/
git clone https://github.com/Smooth-Pacific/https-server-pki-cd.git
```
We will then move into the repository and run the `create_ca.sh` script which will create the necessary CA certificates in `~/certs`.
```
cd https-server-pki-cd
sh create_ca.sh
```

## 1.2 Clone HTTPS server repo
We can now clone and enter the https server repository
```
cd ~/
git clone https://github.com/Smooth-Pacific/https-server-cd.git
cd https-server-cd
```

## 1.3 Build docker image
We can now build the docker image with the following script:
```
sh build_img.sh Dockerfile
```
The script will automatically copy the CA certificates from the `~/certs` (created in step 1.1) directory to `./certs` in the current repo directory and copy `./certs` to `~/certs` in the container. This will allow docker to build the image with the correct certificate, as docker cannot access any other parent folders.

## 1.4 Spinup and attach to docker container
Once the docker image is created, we can spinup and attach to the container as user webserver.
```
sh spinup.sh && sh attach.sh
```
The `attach.sh` script can be used separately to attach to the container in a new terminal

## 1.5 Check that the Root Certificate was installed successfully
Once we are attached to the container, we now need to check that the root certificate was installed successfully. If the root certificate has been validated we can exit the container via the command `exit`
```
awk -v cmd='openssl x509 -noout -subject' '/BEGIN/{close(cmd)};{print | cmd}' < /etc/pki/ca-trust/source/anchors/smoothstack_root.crt | grep smoothstack

exit
```

# 2. Build and Run the webserver
## 2.1 Build project inside docker container
We can now build the webserver, but first we attach to the container.
```
sh attach.sh
```
Once attached, we can now navigate to the project folder and build.
```
cd ~/source_directory
mkdir build && cd build
cmake ..
make
```
These commands will create a binary `./server` in the `~/source_directory/build` directory and `./test` in the `~/source_directory/test` directory.

## 2.2 Customizing server options
Before we run the server we can modify server settings in the `run_server.sh` script. The server options should look like:
```
# START SERVER OPTIONS
export PORT=8081
export MAX_CONNECTIONS=32
export CONNECTION_TIMEOUT=120
export MEMORY_LIMIT=65535
export HTTPS_MEM_KEY_PATH=~/certs/server_ca/private/smoothstack_server.key
export HTTPS_MEM_CERT_PATH=~/certs/server_ca/certs/smoothstack_server.crt
export MAX_THREADS=8
# END SERVER OPTIONS
```
The script is setting environment variables which the server can then access. We can modify these options if necessary.

## 2.3 Run the server
After the project is built and all options set, we can run the webserver by running the `run_server.sh` script in the project root directory.
```
cd ~/source_directory
. ./run_server.sh
```

# 3. Testing

## 3.1 Unit tests
Unit tests can be run by running the `run_tests.sh` script in the project root directory
```
# inside the container
cd ~/source_directory
. ./run_test.sh
```

If the tests fail, then there is a problem with the code.
## 3.2 Test the server inside the docker container
We can test the server by opening a new terminal, attaching to the container, and running `curl 'https://172.17.0.2:8081/helloworld'`
```
# in a new terminal
sh attach.sh
curl -v --digest --user calvin:mypass https://172.18.0.2:8081/helloworld
# or
curl -v --digest --user calvin:mypass https://localhost:8081/helloworld
# IPv6
curl -v --digest --user calvin:mypass https://\[::1\]:8081/helloworld
```
The command should output something like this in the very last line:
```
# if using ipv4 (172.18.0.2)
::ffff:172.18.0.2%

# if using ip46 (localhost or ::1)
::1%
```

## 3.3 Simulating load to the server
We can simulate request load on the server by using the `hammer_server.py` python script.
```
# running from inside the container
cd ~/source_directory/test
pip3 install grequests
python3 hammer_server.py
```
Settings such as `URL`, `CERT_PATH`, `USER`, `PASSWORD` need to be changed according to the configuration of the server. Additionally, we can set `NUM_OF_REQUESTS` and `NUM_OF_THREADS` the script uses to send requests to the server. It needs to be noted that if `NUM_OF_THREADS` exceeds the `MAX_CONNECTIONS` of the server config, the script will stall/deadlock.  
If set up properly, the script can cause the server to hit 100% CPU usage on all cores, displaying its threading ability.

## 3.4 Simulating load to the server on a client machine
If we want to run the script from outside the container/from the host machine, we first need to install the proper CA certificate.
In order to add the Root Certificate to a client (in thise case an ubuntu system), we must first copy the `smoothstack_client.crt` file that we created to the client system.
We can do this by navigating to the project root directory and run:
```
# from the client machine
sudo cp ~/certs/root_ca/certs/smoothstack_root.crt /usr/local/share/ca-certificates/smoothstack_client.crt
sudo update-ca-certificates
```

Once the certificate is added, we need to check if it was installed successfully by running:
```
awk -v cmd='openssl x509 -noout -subject' '/BEGIN/{close(cmd)};{print | cmd}' < /etc/ssl/certs/ca-certificates.crt | grep smoothstack
```

The output of the previous command should look something like:
```
subject=C = US, ST = Texas.test, L = Houston.test, O = smoothstack.test, OU = smoothstack, CN = smoothstack, emailAddress = email@example.com
```
We can now run the `hammer_server.py` script by following the steps from section `3.3`

It should be noted that the output of the `hammer_server.py` script will return a majority of 200 response codes, it will retrun some 401 response codes. 401 means that the server was unable to authenticate the request. As of right now, I am unable to diagnose/determien the root cause of the error. However, I suspect that the issue is caused by a race condition on the server to authenticate 100s of requests on seconds.