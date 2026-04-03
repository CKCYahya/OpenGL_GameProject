cmake_minimum_required(VERSION 3.10)
project(GameProject VERSION 1.0)

# C++ Standartları
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# GLFW Ayarları (Sadece kütüphaneyi derle, testleri/dokümanları atla)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(libs/glfw)

# OpenGL Kütüphanesini Sistemde Bul
find_package(OpenGL REQUIRED)

# GLM Ayarları
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/libs/glm)

# STB Image Ayarları
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb)

# ImGui Derleme Ayarları
add_library(imgui STATIC
    libs/imgui/imgui.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/backends/imgui_impl_glfw.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends
)
target_include_directories(imgui PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include
)
target_compile_definitions(imgui PRIVATE GLFW_INCLUDE_NONE)

# Ana Oyun Uygulaması (Tüm kaynak kodlar ve glad.c)
add_executable(GameProject
    src/main.cpp
    libs/glad/src/glad.c
    src/Camera.cpp
    src/EBO.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/stb_image.cpp
    src/MapGenerator.cpp
    src/VAO.cpp
    src/VBO.cpp
    src/Window.cpp
    src/GameMap.cpp
    src/Player.cpp
    src/NewMap.cpp
    src/Panel.cpp
    src/Items.cpp
    src/Fishing.cpp
)

# Projenin Başlık (Header) Dosyalarının Yolları
target_include_directories(GameProject PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Header
    ${CMAKE_CURRENT_SOURCE_DIR}/include        # nlohmann/json.hpp buradan okunacak
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends
)

target_compile_definitions(GameProject PRIVATE GLFW_INCLUDE_NONE)

# Kütüphaneleri Ana Uygulamaya Bağla
target_link_libraries(GameProject PRIVATE
    glfw
    OpenGL::GL
    glm
    stb_image
    imgui
    ${CMAKE_DL_LIBS} 
)