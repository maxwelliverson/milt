#version 460

layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

void main(){

    //vec3 inter = (aPos / 500.0) - 1.0;
    gl_Position = projection * view * vec4(aPos, 1.0);
}
