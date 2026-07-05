cmake_minimum_required(VERSION 3.26)
project(pagultimo)

set(CMAKE_CXX_STANDARD 17)

file (GLOB MY_FILES *.cpp)
file (GLOB GLAD_FILES glad/src/*.c)
file (GLOB IM_GUI_FILES imgui/src/*.cpp)

add_executable(${PROJECT_NAME} ${MY_FILES} ${GLAD_FILES} ${IM_GUI_FILES}
        Material.cpp
        Material.h
        Luz.cpp
        Luz.h
        Textura.cpp
        Textura.h
        lodepng.cpp
        lodepng.h
        )
target_include_directories(${PROJECT_NAME} PUBLIC imgui/include glad/include)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

find_package(lodepng CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE lodepng)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR} )