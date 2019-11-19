#ifndef __MAIN__
    #define __MAIN__

#include "ext/imgui/imgui_user.h"
#include "GL/glew.h"
#ifdef __WIN32__
    #include "GL/wglew.h"
    #define GLFW_EXPOSE_NATIVE_WIN32
    #define GLFW_EXPOSE_NATIVE_WGL
#else
    #include "GL/glxew.h"
    #define GLFW_EXPOSE_NATIVE_X11
    #define GLFW_EXPOSE_NATIVE_GLX
#endif
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include <stdio.h>


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

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(){
    
    if(!glfwInit()){
        printf("GLFW Failed to Initialize\n");
        return -1;
    }
    else{
        printf("GLFW Successfully Initialized\n");
    }
    glfwSetErrorCallback(glfw_error_callback);
    /*if(!glewInit()){
        printf("GLEW Failed to Initialize\n");
        return -1;
    }
    else {
        printf("GLEW Successfully Initialized\n");
    }*/

    GLFWwindow *window;
    GLFWmonitor *monitor;
    //const GLFWvidmode *videoMode;

    const char* glsl_version = "#version 460";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    monitor = glfwGetPrimaryMonitor();
    //videoMode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(1280, 720, "Why won't you woooorrrkkk", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    int err = glewInit();
    printf("%s\n", glewGetErrorString(err));

    printf("%s",(const char*)glGetString(GL_VERSION));

    if(!GLEW_VERSION_4_6)
    {
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
        return -1;
    }

    if(!window){
        printf("GLFW Window failed to initialize.");
        glfwTerminate();
        return -1;
    }

    //IMGUI Stuff
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    bool show_demo_window = true;
    bool show_metrics_window = true;

    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        if(show_metrics_window)
            ImGui::ShowMetricsWindow(&show_metrics_window);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Metric Window", &show_metrics_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        //Testing Functionality
        {
            static int value = 0;
            static float color[4]= {0.5f, 0.5f, 0.5f, 1.0f};
            static ImVec4 col;

            ImGui::Begin("Value Callback");

            ImGui::ColorPicker4("Color Picker", color);
            col = ImVec4(color[0], color[1], color[2], color[3]);

            ImGui::DragInt4("DragInt4", &value);

            for(int i = 0; i < value; ++i){
                ImGui::TextColored(col, "Whoa Momma");
            }

            ImGui::End();
        }

        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;


}

#endif
