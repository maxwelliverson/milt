#ifndef __MAIN__
    #define __MAIN__



#include "milt.h"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
f_type lastX;
f_type lastY;

int main(){


    //GLFW INIT
    //---------------------------------------------
    glfwErrCheck(!glfwInit());
    glfwSetErrorCallback(glfw_error_callback);
    auto monitor = glfwGetPrimaryMonitor();
    auto videoMode = glfwGetVideoMode(monitor);
    lastX = (f_type)(videoMode->width / 2);
    lastY = (f_type)(videoMode->height / 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, REQ_GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, REQ_GL_MINOR);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    auto window = glfwCreateWindow(videoMode->width, videoMode->height, "Node Visualization", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwWindowCheck(window);

    //GLEW INIT
    //------------------------------------------------------
    int err = glewInit();
    glew_required_version(GLEW_VERSION_4_6);
    printf("%s\n", glewGetErrorString(err));
    glViewport(0, 0, videoMode->width, videoMode->height);
    printf("%s: %s\n","Using GL Version: ",(const char*)glGetString(GL_VERSION));


    //Node & GL Buffer Initialization
    //-------------------------------------------------------
    int node_count = 10000;
    int new_node_count = node_count;
    int seed = 1234;

    Node<f_type>* nodes = getRandomNodes(node_count, 0, 1000, seed);
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glEnableVertexAttribArray(0);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nodes) * node_count, nodes, GL_STREAM_DRAW);


    //IMGUI Initialization
    //---------------------------------------------------
    float scale_factor = 1.75;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.FontAllowUserScaling = true;
    io.FontGlobalScale = scale_factor;
    ImGui::StyleColorsLight();
    ImGui::GetStyle().ScaleAllSizes(scale_factor);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(REQ_GLSL_VERSION);

    //Compile and Link shaders.
    //----------------------------------------------------
    Shader node_shader = Shader("shaders\\node.vert", "shaders\\node.frag");
    node_shader.setVec4("NodeColor", 0.9f, 0.8f, 0.5f, 1.0f);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Node<f_type>),(void*)nullptr);
    glEnableVertexAttribArray(0);

    //Declare & initialize state variables
    //----------------------------------------------------
    int current_render_mode = 0;
    int sdf_rounds = 10;
    int opt_rounds = 6;
    int post_sdf_rounds = 100;
    int nearest_neighbours = 5;
    bool is_generating = false;
    bool is_optimizing = false;


    /*
     * MAIN RENDERING LOOP
     */
    //-----------------------------------------------------
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        //Create a new frame
        //-------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Color Callback window
        //--------------------------
        {

            static float bc[4]= {0.0f, 0.0f, 0.0f, 0.8f};
            static float node_color[4] = {0.9f, 0.8f, 0.5f, 1.0f};
            static float nc[4] = {0.9f, 0.8f, 0.5f, 1.0f};

            ImGui::Begin("Value Callback");

            if(ImGui::TreeNode("Background Color"))
            {
                ImGui::ColorPicker4("Color Picker", bc);
                glClearColor(bc[0], bc[1], bc[2], bc[3]);
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Node Color"))
            {
                ImGui::ColorPicker4("Color Picker", node_color);
                if (nc[0] != node_color[0] || nc[1] != node_color[1] || nc[2] != node_color[2] ||
                    nc[3] != node_color[3]) {
                    nc[0] = node_color[0];
                    nc[1] = node_color[1];
                    nc[2] = node_color[2];
                    nc[3] = node_color[3];
                    node_shader.setVec4("NodeColor", nc[0], nc[1], nc[2], nc[3]);
                }
                ImGui::TreePop();
            }


            ImGui::SliderFloat("Scale Factor", &scale_factor, 1.0f, 4.0f);

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
                    std::thread([nodes, node_count, opt_rounds, sdf_rounds, post_sdf_rounds, nearest_neighbours, &is_optimizing](){
                        is_optimizing = true;
                        node_optimization(nodes, node_count, opt_rounds, sdf_rounds, post_sdf_rounds, nearest_neighbours);
                        is_optimizing = false;
                        }).detach();
                }
            }
            ImGui::InputInt("Node Count", &new_node_count);
            ImGui::InputInt("Seed", &seed);
            ImGui::InputInt("Sdf Rounds", &sdf_rounds);
            ImGui::InputInt("Post-Sdf Rounds", &post_sdf_rounds);
            ImGui::InputInt("Optimization Rounds", &opt_rounds);
            ImGui::InputInt("Nearest Neighbours", &nearest_neighbours);
            ImGui::Combo("Rendering Mode", &current_render_mode, "Points\0Lines\0Fill");
            ImGui::Text("%s: %d", "Current Node Count: ", node_count);
            ImGui::Text("%s", is_generating ? "Is Generating..." : (is_optimizing ? "Is Optimizing..." : ""));

            glPolygonMode(GL_FRONT_AND_BACK, (current_render_mode == 0 ? GL_POINT : (current_render_mode == 1 ? GL_LINE : GL_FILL)));

            ImGui::End();

        }

        {
            ImGui::Begin("Scene Editor");
            ImGui::End();
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(nodes) * node_count, nodes, GL_STREAM_DRAW);

        io.FontGlobalScale = scale_factor;
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);

        node_shader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, node_count);

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
