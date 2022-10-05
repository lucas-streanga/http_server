SRC1 = src/main.cpp
SRC2 = src/socket.cpp
SRC3 = src/request.cpp

OBJ1 = $(SRC1:.cpp=.o)
OBJ2 = $(SRC2:.cpp=.o)
OBJ3 = $(SRC3:.cpp=.o)

CXX = g++

EXE = http_server

HFILES = include/socket.h include/request.h
CFLAGS = -O2 -std=c++17
LDFLAGS = -lpthread
INCDIR = -Iinclude


all : $(EXE)

$(EXE) : $(OBJ1) $(OBJ2) $(OBJ3)
	$(CXX) -o $(EXE)  $(OBJ1) $(OBJ2) $(OBJ3) $(LDFLAGS)

$(OBJ1) : $(SRC1) $(HFILES)
	$(CXX) $(INCDIR) -c $(CFLAGS) $(SRC1) -o $(OBJ1)

$(OBJ2) : $(SRC2) $(HFILES)
	$(CXX) $(INCDIR) -c $(CFLAGS) $(SRC2) -o $(OBJ2)

$(OBJ3) : $(SRC3) $(HFILES)
	$(CXX) $(INCDIR) -c $(CFLAGS) $(SRC3) -o $(OBJ3)


debug: CFLAGS = -g -Wall -DDEBUG=1
debug: all

cleanup:
	@echo "Deleting object files..."
	rm -f src/*.o

clean:
	make -B
	make cleanup

clean-debug:
	make debug -B
	make cleanup

