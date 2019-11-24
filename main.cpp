#ifndef __MAIN__
    #define __MAIN__

#include "ext/imgui/imgui_user.h"
#include "GL/glew.h"
#include "octree.h"
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
#include "shaders.h"


using f_type = float;

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

template <typename T>
bool node_optimization(Node<T>* nodes, int node_count);

Node<f_type>* getRandomNodes(int n, f_type min, f_type max, int seed);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

int main(){

    bool is_generating = false;
    bool is_optimizing = false;
    
    if(!glfwInit()){
        printf("GLFW Failed to Initialize\n");
        return -1;
    }
    else{
        printf("GLFW Successfully Initialized\n");
    }
    glfwSetErrorCallback(glfw_error_callback);

    GLFWwindow *window;
    GLFWmonitor *monitor;
    const GLFWvidmode *videoMode;

    const char* glsl_version = "#version 460";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    monitor = glfwGetPrimaryMonitor();
    videoMode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    window = glfwCreateWindow(videoMode->width, videoMode->height, "Why won't you woooorrrkkk", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    int err = glewInit();
    glew_required_version(GLEW_VERSION_4_4);
    printf("%s\n", glewGetErrorString(err));
    glViewport(0, 0, videoMode->width, videoMode->height);

    printf("%s: %s","GL Version: ",(const char*)glGetString(GL_VERSION));

    if(!window){
        printf("GLFW Window failed to initialize.");
        glfwTerminate();
        return -1;
    }

    int node_count = 10000;
    int new_node_count = node_count;
    int seed = 1234;

    //Initialize Nodes and the glVertexBuffer
    Node<f_type>* nodes = getRandomNodes(node_count, 0, 1000, seed);
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glEnableVertexAttribArray(0);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nodes), nodes, GL_STREAM_DRAW);



    //IMGUI Stuff
    float scale_factor = 2.5;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.FontAllowUserScaling = true;
    io.FontGlobalScale = scale_factor;
    ImGui::StyleColorsLight();
    ImGui::GetStyle().ScaleAllSizes(scale_factor);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    //Compile and Link shaders.
    GLuint frag_shader, vert_shader, program;
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(frag_shader, 1, &frag_shader_node, NULL);
    glShaderSource(vert_shader, 1, &vert_shader_node, NULL);

    glCompileShader(vert_shader);
    glCompileShader(frag_shader);

    int success;
    char infoLog[512];
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vert_shader, 512, NULL, infoLog);
        printf("%s %s\n", "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n", infoLog);
    } else {
        printf("%s\n", "AY CHIEF, WE GOOD WIT DE VERTS");
    }
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
        printf("%s %s\n", "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n", infoLog);
    } else {
        printf("%s\n", "AY BOSS, WE LIVIN WIT DE FRAGS");
    }

    program = glCreateProgram();
    glAttachShader(program, frag_shader);
    glAttachShader(program, vert_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("%s %s\n", "ERROR::SHADER::PROGRAM::LINKING_FAILED\n", infoLog);
    } else {
        printf("%s\n",  "Sausage Links lol");
    }

    glDeleteShader(frag_shader);
    glDeleteShader(vert_shader);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Node<f_type>),(void*)nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    bool show_demo_window = false;
    bool show_metrics_window = false;


    //glGet(GL_VIEWPORT);

    /*
     * MAIN RENDERING LOOP
     */
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
            ImGui::SliderFloat("Scale Factor", &scale_factor, 1.0f, 4.0f);

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
            glClearColor(col.x, col.y, col.z, col.w);

            ImGui::DragInt4("DragInt4", &value);

            for(int i = 0; i < value; ++i){
                ImGui::TextColored(col, "Whoa Momma");
            }

            ImGui::End();
        }

        {
            ImGui::Begin("Rendering");
            if(ImGui::Button("Generate Nodes")){
                if(!is_generating && !is_optimizing){
                    std::thread([&nodes, new_node_count, seed, &is_generating, &node_count](){
                        is_generating = true;
                        Node<f_type>* new_nodes = getRandomNodes(new_node_count, 0, 1000, seed);
                        delete[] nodes;
                        node_count = new_node_count;
                        nodes = new_nodes;
                        is_generating = false;
                    }).detach();
                }
            }
            ImGui::SameLine();
            if(ImGui::Button("Optimize Nodes")){
                if(!is_generating && !is_optimizing){
                    std::thread([nodes, node_count, &is_optimizing](){
                        is_optimizing = true;
                        node_optimization(nodes, node_count);
                        is_optimizing = false;
                        }).detach();
                }
            }
            ImGui::InputInt("Node Count", &new_node_count);
            ImGui::InputInt("Seed", &seed);
            ImGui::Text("%s: %d", "Current Node Count: ", node_count);
            ImGui::Text("%s", is_generating ? "Is Generating..." : (is_optimizing ? "Is Optimizing..." : ""));

            //ImGuiContext

            ImGui::End();
        }


        io.FontGlobalScale = scale_factor;
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glUseProgram(program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, node_count);
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
