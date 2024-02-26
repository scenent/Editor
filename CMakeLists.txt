cmake_minimum_required(VERSION 3.22)
project(Editor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_GENERATOR_PLATFORM Win32)

set(GLAD_DIR thirdparty/GLAD/cmake)
set(GLFW_DIR thirdparty/GLFW/cmake)
set(GLFW_LIBRARY glfw3)
set(GLFW_LIBRARIES ${GLFW_LIBRARY})
set(FREETYPE_DIR thirdparty/freetype/cmake)
set(FREETYPE_LIBRARY freetype)
set(FREETYPE_LIBRARIES ${FREETYPE_LIBRARY})


find_package(OpenGL REQUIRED)
find_package(GLAD REQUIRED)
find_package(GLFW REQUIRED)
find_package(FREETYPE REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GlAD_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(thirdparty/stb/include)
include_directories(thirdparty/GLAD/include)
include_directories(thirdparty/freetype/include)
include_directories(include)

link_directories(thirdparty/GLFW/lib)
link_directories(thirdparty/freetype/lib)

add_executable(${PROJECT_NAME}
        ${GLAD_SOURCE}
        thirdparty/stb/include/stb_image.h

        src/main.cpp
        src/camera.cpp
        include/camera.h
        src/color_rect.cpp
        include/color_rect.h
        src/editor.cpp
        include/editor.h
        src/glyph_texture.cpp
        include/glyph_texture.h
        src/label.cpp
        include/label.h
        src/texture.cpp
        include/texture.h
        src/tween.cpp
        include/tween.h
        include/macros.h
        include/math_utils.h
        include/shader.h
        include/utils.h
        )

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${FREETYPE_LIBRARIES})
