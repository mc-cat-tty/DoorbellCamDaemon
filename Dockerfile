FROM ubuntu:latest

LABEL version="1.0" \
      description="DoorbellCamDaemon"

ENV CXX /usr/bin/g++
ARG DEBIAN_FRONTEND=noninteractive

# Install dependencies for OpenCV with apt
RUN apt-get update && apt-get install -y \
    build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev \
    python3-dev python3-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev \
    libcanberra-gtk-module libcanberra-gtk3-module

# Install GStreamer with apt
RUN apt-get update && apt-get install -y \
    libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav \
    gstreamer1.0-tools libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    protobuf-compiler libgtk2.0-dev ocl-icd-opencl-dev \
    gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 \
    gstreamer1.0-qt5 gstreamer1.0-pulseaudio

WORKDIR /root/home/

# Install OpenCV
RUN git clone https://github.com/opencv/opencv.git && \
    cd opencv/ && mkdir build && cd build && \
    cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D BUILD_DOCS=OFF -D BUILD_EXAMPLES=OFF -D BUILD_opencv_apps=OFF \
    -D BUILD_opencv_python2=OFF -D BUILD_opencv_python3=OFF -D BUILD_SHARED_LIBS=ON -D WITH_GSTREAMER=ON \
    -D BUILD_TESTS=OFF -D ENABLE_PRECOMPILED_HEADERS=OFF -D OPENCV_GENERATE_PKGCONFIG=OFF .. && \
    make -j"$(nproc)" && \
    make install && \
    ldconfig && \
    rm -rf /root/home/opencv/

# Install dependencies for Paho MQT with apt
RUN apt-get update && apt-get install -y \
    build-essential gcc ninja-build cmake cmake-gui cmake-curses-gui \
    fakeroot fakeroot devscripts dh-make lsb-release libssl-dev

# Install Paho MQTT
RUN git clone https://github.com/eclipse/paho.mqtt.c.git && \
    mkdir /tmp/build.paho && cd /tmp/build.paho && \
    cmake -GNinja -D PAHO_WITH_SSL=TRUE -D PAHO_BUILD_STATIC=FALSE /root/home/paho.mqtt.c && \
    cmake --build . && cmake --install . && rm -rf /root/home/paho.mqtt.c/

# ENV PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib64/pkgconfig:/usr/lib/pkgconfig
# RUN pkg-config --cflags --libs --static opencv4

# Build and install the project
RUN apt-get update && apt-get install -y g++ make libfmt-dev

WORKDIR /root/home/DoorbellCamDaemon/
COPY . .
RUN make clean && make gendep && make -f MakefileDebug

ENTRYPOINT ["make", "run_container"]