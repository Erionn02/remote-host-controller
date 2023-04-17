docker build -f ./UbuntuDockerfile.dockerfile -t ubuntu-remote-host-controller .
docker run --rm --net host --name my_ssh_container ubuntu-remote-host-controller:latest