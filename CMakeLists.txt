cmake_minimum_required(VERSION 3.10)

project(http_server)

set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 启用 AddressSanitizer
set(ENABLE_ASAN FALSE)

if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
endif()

add_subdirectory(m_netlib)

add_executable(http_server 
               http_server.cc 
               http_context.cc 
               http_response.cc 
               main.cc
)

target_link_libraries(http_server m_netlib)
