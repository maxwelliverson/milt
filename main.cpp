#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#include "GL/glew.h"
#include "GL/glxew.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"


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
    return;
}

int main(){
    GLFWwindow *window;
    GLFWmonitor *monitor;
    const GLFWvidmode *videoMode;

    if(!glfwInit()){
        return -1;
    }

    if(!glewInit()){
        return -1;
    }
    /*
    int n = 2;
    GLFWmonitor** monitors = glfwGetMonitors(&n);
    monitor = monitors[0];
    */
    monitor = glfwGetPrimaryMonitor();
    videoMode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(videoMode->width, videoMode->height, "Whoa Momma", monitor, NULL);

    if(!window){
        glfwTerminate();
        return -1;
    }
    /*
    GLuint* fbo;
    glGenFramebuffers(1, fbo);
    */
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;


}
