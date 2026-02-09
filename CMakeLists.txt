cmake_minimum_required(VERSION 3.10)

project(blockchain_core)
include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(OpenSSL REQUIRED)

add_executable(blockchain_core ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(blockchain_core OpenSSL::Crypto)
