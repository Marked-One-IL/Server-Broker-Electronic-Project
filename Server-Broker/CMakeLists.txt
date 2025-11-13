cmake_minimum_required(VERSION 3.28.3) # Cmake version (The latest is 4.0 but VS22 latest is 3.28.3).
project(Server-Broker) # Project name.

set(CMAKE_CXX_STANDARD 23) # C++ version (The Clion latest is 26 but VS22 latest is 23).

# Those are not necessary but recommended.
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Compiler must support C++23 or higher.
set(CMAKE_CXX_EXTENSIONS OFF) # Prevent of using compiler specific extensions.

# Source files.
add_executable( Server-Broker main.cpp
        MainMCU/Server.cpp
        ClientMCU/Server.cpp
        General/SensorsData.cpp
        General/ThreadSafeCout.cpp
        General/Connection.cpp "ClientHttp/Server.cpp" "General/Helper.cpp")

target_link_libraries(Server-Broker PRIVATE ws2_32) # Link 'ws2_32'.