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
#include <exception>
#include <imgui.h>
#include "milt_tests.h"

int milt_width;
int milt_height;
bool move_forwards = false;
bool move_backwards = false;
bool move_left = false;
bool move_right = false;
bool can_look = false;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){

    switch(action){
        case GLFW_PRESS:
            switch(key){
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                    break;

                case GLFW_KEY_W:
                case GLFW_KEY_UP:
                    move_forwards = true;
                    break;
                case GLFW_KEY_S:
                case GLFW_KEY_DOWN:
                    move_backwards = true;
                    break;
                case GLFW_KEY_A:
                case GLFW_KEY_LEFT:
                    move_left = true;
                    break;
                case GLFW_KEY_D:
                case GLFW_KEY_RIGHT:
                    move_right = true;
                    break;
            } break;
        case GLFW_RELEASE:
            switch(key){

                case GLFW_KEY_W:
                case GLFW_KEY_UP:
                    move_forwards = false;
                    break;
                case GLFW_KEY_S:
                case GLFW_KEY_DOWN:
                    move_backwards = false;
                    break;
                case GLFW_KEY_A:
                case GLFW_KEY_LEFT:
                    move_left = false;
                    break;
                case GLFW_KEY_D:
                case GLFW_KEY_RIGHT:
                    move_right = false;
                    break;
            }
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_1){
        can_look = (action == GLFW_PRESS);
    }
}

void scrollwheel_callback(GLFWwindow* window, double xoff, double yoff){
    camera.ProcessMouseScroll(yoff);
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

void moveCamera(){
    ImVec2 offset;

    if(move_forwards)
        camera.ProcessKeyboard(FORWARD, ImGui::GetIO().DeltaTime);
    if(move_backwards)
        camera.ProcessKeyboard(BACKWARD, ImGui::GetIO().DeltaTime);
    if(move_left)
        camera.ProcessKeyboard(LEFT, ImGui::GetIO().DeltaTime);
    if(move_right)
        camera.ProcessKeyboard(RIGHT, ImGui::GetIO().DeltaTime);
    if(can_look) {
        offset = ImGui::GetIO().MouseDelta;
        camera.ProcessMouseMovement(-offset.x, offset.y);
    }
}

#endif //MILT_H
