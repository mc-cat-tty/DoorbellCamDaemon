CXX = g++
CXXFLAGS = -std=c++20 -Wall --pedantic -I/usr/include/opencv4 -I/usr/include/MQTTClient.h \
	-lfmt -lopencv_videoio -lopencv_core -lopencv_imgproc -lopencv_highgui \
	-lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d \
	-lopencv_objdetect -lopencv_stitching -lopencv_dnn -lpaho-mqtt3c \
	-DINLINE_ENABLED
EXEC = dcd
OBJ = main.o fsm_manager/fsm_manager.o object_detection/object_detection.o node/node.o
SHELL = /bin/bash

build: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(EXEC)

-include depedencies.txt
-include params.conf

.PHONY: run clean cleanall gendep gendoc install run_container
run:
	./$(EXEC) ${CAM_IP} ${CAM_PORT} ${USER} ${PASS} ${MQTT_ADDR} ${MQTT_TOPIC}

clean:
	rm -rf $(OBJ)

cleanall:
	rm -rf $(OBJ) $(EXEC) docs/ depedencies.txt

gendep:
	$(CXX) -MM *.cpp > depedencies.txt

gendoc:
	doxygen

install:
	ln -s doorbellcam.service /etc/systemd/system/
	systemctl start doorbellcam
	systemctl enable doorbellcam

run_container:
	./$(EXEC) ${CAM_IP_ENV} ${CAM_PORT_ENV} ${USER_ENV} ${PASS_ENV} ${MQTT_ADDR_ENV} ${MQTT_TOPIC_ENV}