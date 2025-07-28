cmake_minimum_required(VERSION 3.10)
project(simple_TCP)

set(CMAKE_CXX_STANDARD 17)

# Include paths
include_directories(
    ${PROJECT_SOURCE_DIR}/channel
    ${PROJECT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/sender
    ${PROJECT_SOURCE_DIR}/receiver
)

# Add executable
add_executable(simple_TCP
    main.cpp
    channel/unreliable_channel.cpp
    sender/sender.cpp
    receiver/receiver.cpp
)

# Link threads
find_package(Threads REQUIRED)
target_link_libraries(simple_TCP PRIVATE Threads::Threads)
