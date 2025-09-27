cmake_minimum_required(VERSION 3.16)
project(mi_sfml_app CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML 3
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

add_executable(app
    main.cpp
    # añade aquí más .cpp si los tienes
)

target_link_libraries(app PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)
