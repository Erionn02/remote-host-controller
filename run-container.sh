docker build -f ./UbuntuDockerfile.dockerfile -t ubuntu-remote-host-controller .
docker run --rm -it --net host ubuntu-remote-host-controller:latest