#ifndef __MAIN__
    #define __MAIN__



#include "milt.h"

#define cube_t 0
#define sphere_t 1
#define box_t 2

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
f_type lastX;
f_type lastY;

int main(){
    try {

        const auto minval = (f_type) 0;
        const auto maxval = (f_type) 1000;
        auto halfway = (minval + maxval) / (f_type) 2;
        auto center_point = new Node<f_type>(halfway, halfway, halfway);


        //GLFW INIT
        //---------------------------------------------
        glfwErrCheck(!glfwInit());
        glfwSetErrorCallback(glfw_error_callback);
        auto monitor = glfwGetPrimaryMonitor();
        auto videoMode = glfwGetVideoMode(monitor);
        lastX = (f_type) (videoMode->width / 2);
        lastY = (f_type) (videoMode->height / 2);
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
        glew_required_version(GLEW_VERSION_4_4);
        printf("%s\n", glewGetErrorString(err));
        glViewport(0, 0, videoMode->width, videoMode->height);
        printf("%s: %s\n", "Using GL Version: ", (const char *) glGetString(GL_VERSION));


        //Node & GL Buffer Initialization
        //-------------------------------------------------------
        int node_count = 10000;
        int new_node_count = node_count;
        int seed = 1234;

        Node<f_type> *nodes = getRandomNodes(node_count, minval, maxval, seed);
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
        ImGuiIO &io = ImGui::GetIO();
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Node<f_type>), (void *) nullptr);
        glEnableVertexAttribArray(0);
        //node_shader.setVec4("NodeColor", 0.9f, 0.8f, 0.5f, 1.0f);

        //Declare & initialize state variables
        //----------------------------------------------------
        int current_render_mode = 0;
        int sdf_rounds = 10;
        int opt_rounds = 100;
        int post_sdf_rounds = 6;
        int nearest_neighbours = 5;
        bool is_generating = false;
        bool is_optimizing = false;

        auto oct = new Octree<f_type>(center_point, halfway);
        oct->addNodes(nodes, node_count);
        auto active_scene = new sample_scene<f_type>();
        active_scene->addObj(new cube<f_type>(center_point, halfway));
        active_scene->addObj(new sphere<f_type>(new Node<f_type>(150, 300, 700), 100));
        active_scene->addObj(new sphere<f_type>(new Node<f_type>(800, 500, 850), 150));
        active_scene->addObj(
                new rounded_box<f_type>(new Node<f_type>(400, 800, 300), new Node<f_type>(250, 100, 200), 30));
        active_scene->addObj(
                new rounded_box<f_type>(new Node<f_type>(600, 100, 100), new Node<f_type>(70, 100, 150), 10));

        /*
         * MAIN RENDERING LOOP
         */
        //-----------------------------------------------------
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            //Create a new frame
            //-------------------------
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //Color Callback window
            //--------------------------
            {

                static float bc[4] = {0.0f, 0.0f, 0.0f, 0.8f};
                static float node_color[4] = {0.9f, 0.8f, 0.5f, 1.0f};
                static float nc[4] = {0.0f};

                ImGui::Begin("Value Callback");

                if (ImGui::TreeNode("Background Color")) {
                    ImGui::ColorPicker4("Color Picker", bc);
                    glClearColor(bc[0], bc[1], bc[2], bc[3]);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Node Color")) {
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
                if (ImGui::Button("Generate Nodes")) {
                    if (!is_generating && !is_optimizing) {
                        std::thread([&nodes, new_node_count, seed, &is_generating, &node_count, &oct, minval, maxval]() {
                            is_generating = true;
                            Node<f_type> *new_nodes = getRandomNodes(new_node_count, minval, maxval, seed);
                            delete[] nodes;
                            oct->reset();
                            node_count = new_node_count;
                            nodes = new_nodes;
                            oct->addNodes(nodes, node_count);
                            is_generating = false;
                        }).detach();
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Optimize Nodes")) {
                    if (!is_generating && !is_optimizing) {
                        std::thread(
                                [active_scene, &oct, nodes, node_count, opt_rounds, sdf_rounds, post_sdf_rounds, nearest_neighbours, &is_optimizing]() {
                                    is_optimizing = true;
                                    node_optimization(active_scene, oct, nodes, node_count, opt_rounds, sdf_rounds, post_sdf_rounds,
                                                      nearest_neighbours);
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

                glPolygonMode(GL_FRONT_AND_BACK,
                              (current_render_mode == 0 ? GL_POINT : (current_render_mode == 1 ? GL_LINE : GL_FILL)));

                ImGui::End();

            }

            {
                static int type_id[40] = {cube_t, sphere_t, sphere_t, box_t, box_t};
                static bool p_open = false;
                static int obj_type;
                static bool rm_obj = false;

                ImGui::Begin("Scene Editor");

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
                ImGui::Columns(2);
                ImGui::Separator();

                struct objs {
                    static void showObject(scene_obj<f_type> *obj, int id) {
                        ImGui::PushID(obj);
                        ImGui::AlignTextToFramePadding();
                        bool node_open = ImGui::TreeNode(obj, "%s %u", obj->name(), ImGui::GetID(obj));
                        //ImGui::NextColumn();
                        ImGui::AlignTextToFramePadding();
                        if (node_open) {
                            switch (id) {
                                case cube_t: {
                                    auto cube_obj = (cube<f_type> *) obj;
                                    auto center = cube_obj->center;
                                    f_type side_len = cube_obj->len_to_sides;
                                    float c_vec[3] = {center->x, center->y, center->z};
                                    ImGui::SliderFloat("Half Sidelength", &side_len, 0, 1000);
                                    ImGui::SetNextItemWidth(-1);
                                    ImGui::SliderFloat3("Center", c_vec, 0, 1000);
                                    cube_obj->len_to_sides = side_len;
                                    cube_obj->center->x = c_vec[0];
                                    cube_obj->center->y = c_vec[1];
                                    cube_obj->center->z = c_vec[2];
                                    break;
                                }
                                case sphere_t: {
                                    auto sphere_obj = (sphere<f_type> *) obj;
                                    auto center = sphere_obj->center;
                                    f_type radius = sphere_obj->radius;
                                    float c_vec[3] = {center->x, center->y, center->z};
                                    ImGui::AlignTextToFramePadding();
                                    ImGui::SliderFloat("Radius", &radius, 0, 1000);
                                    ImGui::SetNextItemWidth(-1);
                                    ImGui::SliderFloat3("Center", c_vec, 0, 1000);
                                    sphere_obj->radius = radius;
                                    sphere_obj->center->x = c_vec[0];
                                    sphere_obj->center->y = c_vec[1];
                                    sphere_obj->center->z = c_vec[2];
                                    break;
                                }
                                case box_t: {
                                    auto box_obj = (rounded_box<f_type> *) obj;
                                    //static auto center = box_obj->center;
                                    //static auto to_faces = box_obj->lengths_to_sides;
                                    //static f_type round = box_obj->round;
                                    ImGui::SliderFloat3("Center", &(box_obj->center->x), 0, 1000);
                                    ImGui::SliderFloat3("Side Lengths", &(box_obj->lengths_to_sides->x), 0, 1000);
                                    ImGui::SliderFloat("Rounding", &(box_obj->round), 0, 200);
                                    break;
                                }
                                default:
                                    ImGui::Text("Well this is embarassing...");
                            }
                            if (ImGui::Button("Delete Object")) {
                                rm_obj = true;
                            }
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                };
                ImGui::NextColumn();

                for (int i = 1; i < active_scene->obj_count; ++i) {
                    objs::showObject(active_scene->objs[i], type_id[i]);
                    if (rm_obj) {
                        auto removed = active_scene->removeObj(active_scene->objs[i]);
                        if (removed != nullptr) {
                            for (int j = i + 1; j < active_scene->obj_count; ++j) {
                                type_id[j - 1] = type_id[j];
                            }
                        }
                        delete removed;
                        rm_obj = false;
                    }
                }

                ImGui::NextColumn();
                if (ImGui::Button("Create New...")) {
                    p_open = true;
                    ImGui::BeginPopupModal("Object Types", &p_open);
                        ImGui::Combo("", &obj_type, "Cube\0Sphere\0Box");
                        if (ImGui::Button("Create")) {
                            //p_open = false;
                            // type_id[active_scene->obj_count] = obj_type;
                            switch (obj_type) {
                                case cube_t:
                                    active_scene->addObj(new cube<f_type>());
                                    break;
                                case sphere_t:
                                    active_scene->addObj(new sphere<f_type>());
                                    break;
                                case box_t:
                                    active_scene->addObj(new rounded_box<f_type>());
                                    break;
                                default:
                                    printf("The Object Creation Interface attempted to create an undefined object.");
                                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                            }
                    }
                }
                //ImGui::BeginPopup("Create a New Object");
				ImGui::PopStyleVar();
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
        glfwDestroyWindow(window);
    } catch (std::exception& e){
        std::cerr << "Exception Caught: " << e.what() << endl;
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

#endif
