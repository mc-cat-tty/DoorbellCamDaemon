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

## Container
TODO

## How to build
```bash
make gendep && make
make clean  # optional
./dcd CAMERA_IP_ADDRESS CAMERA_PORT MQTT_SERVER_ADDR MQTT_SERVER_PORT MQTT_TOPIC
```