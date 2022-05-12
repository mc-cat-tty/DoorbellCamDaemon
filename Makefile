CXX = g++
CXXFLAGS = -std=c++2a -Wall --pedantic -DDEBUG -lvlc
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