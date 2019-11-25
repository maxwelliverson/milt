//
// Created by Maxwell on 2019-11-24.
//

#ifndef MILT_GL_VERSION_H
#define MILT_GL_VERSION_H

#define concat(arg, ...) arg##concat(__VA_ARGS__)
#define glsl_v concat(REQ_GL_MAJOR, REQ_GL_MINOR, 0)
#define REQ_GL_MAJOR 4
#define REQ_GL_MINOR 6
#define REQ_GLSL_VERSION "#version 460"

#endif //MILT_GL_VERSION_H
