cmake_minimum_required(VERSION 3.0.2)

project(ayanami)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/glfw)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/lua)

set(GL_LOADER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/gl.c
    ${CMAKE_CURRENT_LIST_DIR}/gl.h)

add_library(gl_loader
            ${GL_LOADER_SOURCES})

target_include_directories(gl_loader PUBLIC
                           ${CMAKE_CURRENT_LIST_DIR})

add_definitions("-DIMGUI_IMPL_OPENGL_LOADER_CUSTOM=\"gl.h\"")

add_library(imgui
  ${CMAKE_CURRENT_LIST_DIR}/third_party/imgui/imgui.cpp
  ${CMAKE_CURRENT_LIST_DIR}/third_party/imgui/imgui_draw.cpp
  ${CMAKE_CURRENT_LIST_DIR}/third_party/imgui/imgui_widgets.cpp
  ${CMAKE_CURRENT_LIST_DIR}/third_party/imgui/imgui_demo.cpp
  ${CMAKE_CURRENT_LIST_DIR}/third_party/imgui/examples/imgui_impl_glfw.cpp
  ${CMAKE_CURRENT_LIST_DIR}/third_party/imgui/examples/imgui_impl_opengl3.cpp
  ${CMAKE_CURRENT_LIST_DIR}/third_party/ImGuiColorTextEdit/TextEditor.h
  ${CMAKE_CURRENT_LIST_DIR}/third_party/ImGuiColorTextEdit/TextEditor.cpp)

target_include_directories(imgui PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/third_party/imgui
  ${CMAKE_CURRENT_LIST_DIR}/third_party/ImGuiColorTextEdit
  ${CMAKE_CURRENT_LIST_DIR}/third_party/imgui-filebrowser)

target_compile_options(imgui PRIVATE "--std=c++17")

target_link_libraries(imgui glfw gl_loader)

set(AYANAMI_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/camera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/random.cpp
    ${CMAKE_CURRENT_LIST_DIR}/framebuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lambertian.cpp
    ${CMAKE_CURRENT_LIST_DIR}/metal.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dielectric.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sphere.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lua_env.cpp
    ${CMAKE_CURRENT_LIST_DIR}/scene.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hitable_list.cpp
    ${CMAKE_CURRENT_LIST_DIR}/worker_pool.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ayanami.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bvh_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/aabb.h
    ${CMAKE_CURRENT_LIST_DIR}/bvh_node.h
    ${CMAKE_CURRENT_LIST_DIR}/ayanami.h
    ${CMAKE_CURRENT_LIST_DIR}/worker_pool.h
    ${CMAKE_CURRENT_LIST_DIR}/lua_env.h
    ${CMAKE_CURRENT_LIST_DIR}/scene.h
    ${CMAKE_CURRENT_LIST_DIR}/hitable_list.h
    ${CMAKE_CURRENT_LIST_DIR}/sphere.h
    ${CMAKE_CURRENT_LIST_DIR}/hitable.h
    ${CMAKE_CURRENT_LIST_DIR}/lambertian.h
    ${CMAKE_CURRENT_LIST_DIR}/metal.h
    ${CMAKE_CURRENT_LIST_DIR}/dielectric.h
    ${CMAKE_CURRENT_LIST_DIR}/camera.h
    ${CMAKE_CURRENT_LIST_DIR}/random.h
    ${CMAKE_CURRENT_LIST_DIR}/framebuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/ray.h
    ${CMAKE_CURRENT_LIST_DIR}/hit_record.h
    ${CMAKE_CURRENT_LIST_DIR}/material.h
    ${CMAKE_CURRENT_LIST_DIR}/nicemath.h)


add_executable(ayanami
               ${AYANAMI_SOURCES})

target_link_libraries(ayanami gl_loader imgui glfw lua)

