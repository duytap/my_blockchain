cmake_minimum_required(VERSION 3.10)

project(my_blockchain)
include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(OpenSSL REQUIRED)

add_executable(my_blockchain ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(my_blockchain OpenSSL::Crypto)
