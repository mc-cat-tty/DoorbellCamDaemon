# DoorbellCamDaemon
A small piece of software for people recognition through a RTSP video stream. Object (people) detection is implemented with YOLO. The internal FSM's status can be published on a MQTT broker.

You can find the other parts of the project at:
 - https://github.com/mc-cat-tty/DoorbellCamFirmware
 - https://github.com/mc-cat-tty/DoorbellCamPhy

## Dependencies
 - libfmt
 - opencv
 - darknet
 - YOLOv5
 - Paho MQTT C client library: https://www.eclipse.org/paho/clients/c/

## Container
 1. Clone this repository
    ```bash
    git clone https://github.com/mc-cat-tty/DoorbellCamDaemon
    cd DoorbellCamDaemon/
    ```
 2. Build the image
    ```bash
    docker build -t doorbellcam .
    ```
 3. Set environment variables by editing _env.list_ file
 4. Run the image
    ```bash
    docker run --restart always --env-file env.list --net=host doorbellcam
    ```

You can find a pre-built version of the image at:
 - amd64: https://hub.docker.com/repository/docker/francescomecatti/doorbellcam
 - arm64: https://hub.docker.com/repository/docker/francescomecatti/doorbellcam-arm

## Development environment configuration
 1. Populate the file named _params.conf_
 4. To execute the software use `make run` command

## How to build
```bash
make gendep && make
make clean  # optional
make run
```
