cmake_minimum_required(VERSION 3.14)

project(Bookstore)

set(CMAKE_CXX_STANDARD 20)

include_directories("${PROJECT_SOURCE_DIR}/include")

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1 # or a later release
)

execute_process(
        COMMAND git ls-remote https://github.com/catchorg/Catch2.git HEAD
        RESULT_VARIABLE GIT_CONNECTION_STATUS
        OUTPUT_QUIET
        ERROR_QUIET
)

set(MAIN_SOURCES)

set(TEST_SOURCES tests/test.cpp)

add_executable(code ${MAIN_SOURCES} src/main.cpp)

if (GIT_CONNECTION_STATUS EQUAL 0)
    FetchContent_MakeAvailable(Catch2)
    add_executable(mytests ${TEST_SOURCES} ${MAIN_SOURCES})
    target_link_libraries(mytests PRIVATE Catch2::Catch2WithMain)
    include(CTest)
    include(Catch)
    catch_discover_tests(mytests)
else ()
    message(STATUS "Cannot connect to Catch2 repository. Skipping FetchContent.")
endif ()