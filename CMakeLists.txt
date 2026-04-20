cmake_minimum_required(VERSION 3.5)
project(ServerBroker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(ServerBroker ${SOURCES})

target_include_directories(ServerBroker PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(ServerBroker PRIVATE
    CMAKE_PROJECT_ROOT="${CMAKE_SOURCE_DIR}/"
)

target_link_libraries(ServerBroker PRIVATE ws2_32) # Link 'ws2_32'.