FROM braintwister/ubuntu-20.04-gcc-11:latest

WORKDIR /tmp

COPY . ./remote-host-controller

WORKDIR /tmp/remote-host-controller

RUN pip install conan==1.59.0
RUN conan profile new default --detect --force
RUN conan profile update settings.compiler.libcxx=libstdc++11 default


RUN mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . -- -j $(nproc --all)

CMD /tmp/remote-host-controller/build/bin/remote-host-controller