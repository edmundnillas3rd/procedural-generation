cmake_minimum_required(VERSION 3.20)
project(procedural_generation)

find_package(SDL2 CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/main.cpp)

target_include_directories(
    ${PROJECT_NAME} 
    SYSTEM
    PUBLIC 
    ${CMAKE_SOURCE_DIR}
)


target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2 SDL2::SDL2main)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)