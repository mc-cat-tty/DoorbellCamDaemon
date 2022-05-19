CXX = g++
CXXFLAGS = -std=c++20 -Wall --pedantic -I/usr/include/opencv4 \
	-lfmt -lopencv_videoio -lopencv_core -lopencv_imgproc -lopencv_highgui \
	-lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d \
	-lopencv_objdetect -lopencv_stitching -lopencv_dnn \
	-DDEBUG \
	# -DTESTING \
	-DINLINE_ENABLED
EXEC = dcd
OBJ = main.o fsm_manager/fsm_manager.o object_detection/object_detection.o
SHELL = /bin/bash

build: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(EXEC)

-include depedencies.txt
-include params.conf

.PHONY: run clean cleanall gendep gendoc loadconf test
run:
	./$(EXEC) ${CAM_IP} ${CAM_PORT} ${USER} ${PASS}

test:  # compile with TESTING macro before using this target
	./$(EXEC) samples/street.mp4

clean:
	rm -rf $(OBJ)

cleanall:
	rm -rf $(OBJ) $(EXEC) docs/ depedencies.txt

gendep:
	$(CXX) -MM *.cpp > depedencies.txt

gendoc:
	doxygen