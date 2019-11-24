//
// Created by maxwe on 2019-11-22.
//

#ifndef MILT_SHADERS_H
#define MILT_SHADERS_H

static const char* vert_shader_node =
        "#version 460\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main(){\n"
        "gl_Position = (vec4(aPos.x, aPos.y, aPos.z, 1.0) / 500.0) - 1.0;\n"
        "}";

static const char* frag_shader_node =
        "#version 460\n"
        "out vec4 FragColor;\n"
        "void main(){\n"
        "FragColor = vec4(0.9f, 0.8f, 0.5f, 0.8f);\n"
        "}";

#endif //MILT_SHADERS_H
