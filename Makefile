CXX = g++
INCLUDE = include
LIBS = glfw GLEW #GL
FLAGS = -march=native

MAIN = main.cpp
SRC =

OUT = main

LIB_FLAGS := $(foreach lib, $(LIBS),$(addprefix -l, $(lib)))

all : maintest


maintest :
	$(CXX) -I$(INCLUDE) $(FLAGS) $(MAIN) $(SRC) $(LIB_FLAGS) -o $(OUT)

clean :
	rm $(OUT)