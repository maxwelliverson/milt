CXX = g++
INCLUDE = include
LIBS = glfw GLEW GL
FLAGS = -march=native -g

MAIN = main.cpp
SRC = ext/imgui/imgui.cpp ext/imgui/imgui_impl_opengl3.cpp ext/imgui/imgui_impl_glfw.cpp ext/imgui/imgui_demo.cpp ext/imgui/imgui_draw.cpp ext/imgui/imgui_widgets.cpp

OUT = main

LIB_FLAGS := $(foreach lib, $(LIBS),$(addprefix -l, $(lib)))

all : maintest


maintest :
	$(CXX) -I$(INCLUDE) $(FLAGS) $(MAIN) $(SRC) $(LIB_FLAGS) -o $(OUT)

clean :
	rm $(OUT)