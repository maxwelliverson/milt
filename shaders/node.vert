#version 460

layout (location = 0) in vec3 aPos;

void main(){

    vec3 inter = (aPos / 500.0) - 1.0;
    gl_Position = vec4(inter, 1.0);

}
