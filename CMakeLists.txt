cmake_minimum_required(VERSION 3.13)
project(milt)

set(CMAKE_CXX_STANDARD 17)

include_directories(ext/imgui)
include_directories(ext/imgui/misc)
include_directories(ext/imgui/misc/cpp)
include_directories(ext/imgui/misc/fonts)
include_directories(ext/imgui/misc/freetype)
include_directories(include)
if(MINGW)
    include_directories(c:/msys64/mingw64/include)
    include_directories(c:/msys64/ming64/lib)
elseif(UNIX)
    include_directories(/usr/include)
    include_directories(/usr/lib)
    include_directories(/usr/lib/x86_64-linux-gnu)
endif()

#set(GLFW_PATH)
#set(GLEW_PATH)
find_library(GLFW_PATH NAMES glfw glfw3 libglfw libglfw3 PATHS /usr/lib)
find_library(GLEW_PATH NAMES GLEW /usr/lib)
find_library(GL_PATH NAMES OpenGL.so GL PATHS /usr/lib/x86_64-linux-gnu)

find_package(OpenGL MODULE REQUIRED)

#set(CMAKE_MODULE_PATH c:/msys64/mingw64)
#find_library(GLEW_PATH NAMES glew glew32s glew32 libglew libglew32 PATHS c:/msys64/mingw64/lib C:/Users/maxwe/Documents/glew-2.1.0-win32/glew-2.1.0/lib/Release/x64 REQUIRED)

#set(GLEW_ROOT c:/msys64/mingw64)
#set(GLEW_VERBOSE true)
#link_directories(c:/msys64/mingw64/lib)

#find_package(GLEW MODULE REQUIRED)
#message(DEBUG ${GLEW_LIBRARIES})

add_executable(milt
    ext/imgui/misc/cpp/imgui_stdlib.cpp
    ext/imgui/misc/cpp/imgui_stdlib.h
    ext/imgui/imconfig.h
    ext/imgui/imgui.cpp
    ext/imgui/imgui.h
    ext/imgui/imgui_demo.cpp
    ext/imgui/imgui_draw.cpp
    ext/imgui/imgui_impl_glfw.cpp
    ext/imgui/imgui_impl_glfw.h
    ext/imgui/imgui_impl_opengl3.cpp
    ext/imgui/imgui_impl_opengl3.h
    ext/imgui/imgui_internal.h
    ext/imgui/imgui_user.h
    ext/imgui/imgui_widgets.cpp
    ext/imgui/imstb_rectpack.h
    ext/imgui/imstb_textedit.h
    ext/imgui/imstb_truetype.h
    include/algorithm.hpp
    include/internal_settings.hpp
    include/math_base.hpp
    include/render_types.hpp
    include/rendercontext.hpp
    include/settings.hpp
    defaults.cpp
    main.cpp
    settings.cpp)

target_link_libraries(milt ${GLFW_PATH} ${GLEW_PATH} ${OPENGL_gl_LIBRARY})
#target_link_libraries(milt ${GLFW_PATH} ${GLEW_PATH} ${GL_PATH})