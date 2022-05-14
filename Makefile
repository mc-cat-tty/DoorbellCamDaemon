CXX = g++
CXXFLAGS = -std=c++20 -Wall --pedantic -DDEBUG -I/usr/include/opencv4 -lfmt -lopencv_videoio -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_stitching
EXEC = dcd
OBJ = main.o

build: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(EXEC)

-include depedencies.txt

.PHONY: clean cleanall gendep gendoc
clean:
	rm -rf $(OBJ)

cleanall:
	rm -rf $(OBJ) $(EXEC) docs/ depedencies.txt

gendep:
	$(CXX) -MM *.cpp > depedencies.txt

gendoc:
	doxygen 