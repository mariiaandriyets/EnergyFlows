cmake_minimum_required(VERSION 3.10)
project(EnergyFlows)

set(CMAKE_CXX_STANDARD 11)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(EnergyFlows main.cpp)
target_link_libraries(EnergyFlows sfml-graphics sfml-window sfml-system ${OPENGL_LIBRARIES})

add_custom_command(TARGET EnergyFlows POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/arial.ttf
    $<TARGET_FILE_DIR:EnergyFlows>)


