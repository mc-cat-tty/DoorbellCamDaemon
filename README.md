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
make run
```

## Environment configuration
 1. Populate the file named _params.conf_
 4. To execute the software use `make run` command
