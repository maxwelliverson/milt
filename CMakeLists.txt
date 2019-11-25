cmake_minimum_required(VERSION 3.13)
project(milt)

set(CMAKE_CXX_STANDARD 20)

include_directories(ext/imgui)
include_directories(ext/imgui/misc)
include_directories(ext/imgui/misc/cpp)
include_directories(ext/imgui/misc/fonts)
include_directories(ext/imgui/misc/freetype)
include_directories(include)


set(GLEW_PATH)
set(GLFW_PATH)

if(MINGW)
    include_directories(c:/msys64/mingw64/include)
    include_directories(c:/msys64/ming64/lib)
    find_library(GLFW_PATH NAMES glfw glfw3 libglfw libglfw3 PATHS c:/msys64/mingw64/lib)
    find_library(GLEW_PATH NAMES glew32 GLEW glew32s glew PATHS c:/msys64/mingw64/lib)
elseif(UNIX)
    include_directories(/usr/include)
    include_directories(/usr/lib)
    include_directories(/usr/lib/x86_64-linux-gnu)
    find_library(GLFW_PATH NAMES glfw glfw3 libglfw libglfw3 PATHS /usr/lib)
    find_library(GLEW_PATH NAMES GLEW /usr/lib)
else()
    #include_directories(c:/users/maxwe/downloads/glew-2.2.0/include)
    include_directories(c:/Program\ Files\ \(x86\)/GLFW/include)
    include_directories(c:/Program\ Files\ \(x86\)/GLFW/lib/GLFW)
    #find_library(GLFW_PATH NAMES glfw3 libglfw libglfw3 PATHS c:/Program\ Files\ \(x86\)/GLFW/lib/GLFW)
    #find_library(GLEW_PATH NAMES glew32s glew32s PATHS c:/users/maxwe/documents/glew-2.1.0-win32/glew-2.1.0/lib/Release/x64)
    set(GLEW_VERBOSE 1)
    set(CMAKE_GENERATOR_PLATFORM "x64")
    #set(GLEW_USE_STATIC_LIBS 1)
    set(glfw3_DIR "C:/Program Files (x86)/GLFW/lib/cmake/glfw3")
    #set(ENV{GLEW_ROOT} "C:/Users/maxwe/downloads/glew-2.2.0/")
    find_package(glfw3 REQUIRED CONFIG )
    find_package(GLEW REQUIRED )
endif()

#set(GLFW_PATH)
#set(GLEW_PATH)
find_library(GL_PATH NAMES OpenGL.so GL PATHS /usr/lib/x86_64-linux-gnu)
find_package(OpenGL MODULE REQUIRED)

message(STATUS CMAKE_LINK_LIBRARIES_SUFFIX = ${CMAKE_LINK_LIBRARY_SUFFIX})

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
    shaders/node.frag
    shaders/node.vert
    main.cpp
    settings.cpp include/neighbours.h include/node.h include/ray.h include/octree.h include/scene.h include/sample_scene.h milt_tests.cpp ext/camera.h ext/shader.h include/milt.h include/gl_version.h)


if(MSVC)
   # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODE")
    target_link_libraries(milt glfw GLEW::GLEW OpenGL::GL)
else()
    target_link_libraries(milt ${GLFW_PATH} ${GLEW_PATH} OpenGL::GL)
endif()