cmake_minimum_required(VERSION 3.14)

project(Bookstore)

set(CMAKE_CXX_STANDARD 20)

include_directories("${PROJECT_SOURCE_DIR}/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(MAIN_SOURCES src/Lexer.cpp
src/Managers.cpp src/Parser.cpp src/Statement.cpp)

add_executable(code ${MAIN_SOURCES} src/main.cpp)

