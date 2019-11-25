//
// Created by Maxwell on 2019-11-24.
//

#ifndef MILT_H
#define MILT_H

using f_type = float;

#include "ext/imgui/imgui_user.h"
#include "GL/glew.h"
#include "octree.h"
#include "sample_scene.h"
#include <thread>
#if defined(LINUX)
#include "GL/glxew.h"
    #define GLFW_EXPOSE_NATIVE_X11
    #define GLFW_EXPOSE_NATIVE_GLX
#elseif defined(MSVC)
    #include "GL/wglew.h"
    #define GLFW_EXPOSE_NATIVE_WIN32
    #define GLFW_EXPOSE_NATIVE_WGL
#else
#include "GL/wglew.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLEW_DLL
#endif

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include <cstdio>
#include "ext/camera.h"
#include "ext/shader.h"
#include "gl_version.h"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){

    switch(action){
        case GLFW_PRESS:
            switch(key){
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                    break;


            } break;
        case GLFW_RELEASE:
            break;
    }
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void glew_required_version(bool GLEW_VER){
    if(!GLEW_VER){
        printf("\nGLEW 1.1: %d\n", GLEW_VERSION_1_1);
        printf("GLEW 1.2: %d\n", GLEW_VERSION_1_2);
        printf("GLEW 1.3: %d\n", GLEW_VERSION_1_3);
        printf("GLEW 1.4: %d\n", GLEW_VERSION_1_4);
        printf("GLEW 1.5: %d\n", GLEW_VERSION_1_5);
        printf("GLEW 2.0: %d\n", GLEW_VERSION_2_0);
        printf("GLEW 2.1: %d\n", GLEW_VERSION_2_1);
        printf("GLEW 3.0: %d\n", GLEW_VERSION_3_0);
        printf("GLEW 3.1: %d\n", GLEW_VERSION_3_1);
        printf("GLEW 3.2: %d\n", GLEW_VERSION_3_2);
        printf("GLEW 3.3: %d\n", GLEW_VERSION_3_3);
        printf("GLEW 4.0: %d\n", GLEW_VERSION_4_0);
        printf("GLEW 4.1: %d\n", GLEW_VERSION_4_1);
        printf("GLEW 4.2: %d\n", GLEW_VERSION_4_2);
        printf("GLEW 4.3: %d\n", GLEW_VERSION_4_3);
        printf("GLEW 4.4: %d\n", GLEW_VERSION_4_4);
        printf("GLEW 4.5: %d\n", GLEW_VERSION_4_5);
        printf("GLEW 4.6: %d\n", GLEW_VERSION_4_6);
        glfwTerminate();
        exit(-1);
    }
}

void glfwErrCheck(int error_code){
    if(error_code){
        printf("GLFW Failed to Initialize\n");
        exit(-1);
    }
    else{
        printf("GLFW Successfully Initialized\n");
    }
}

void glfwWindowCheck(GLFWwindow* window){
    if(!window){
        printf("GLFW Window failed to initialize.");
        glfwTerminate();
        exit(-1);
    }
}

template <typename T>
bool node_optimization(Node<T>* nodes, int node_count, int rounds, int sdf_rounds, int post_sdf_rounds, int nearest_neighbours);

Node<f_type>* getRandomNodes(int n, f_type min, f_type max, int seed);

#endif //MILT_H
