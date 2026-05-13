cmake_minimum_required(VERSION 3.14)

project(OS_Lab_NamedPipes)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_library(os_lab_core
    src/employee_file.cpp
    src/record_locks.cpp
    src/named_pipe.cpp
    src/server_logic.cpp
    src/client_logic.cpp
    src/input_utils.cpp
    src/winapi_error.cpp
)

target_include_directories(os_lab_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(Server
    src/server_main.cpp
)

target_link_libraries(Server PRIVATE os_lab_core)

add_executable(Client
    src/client_main.cpp
)

target_link_libraries(Client PRIVATE os_lab_core)

add_executable(OS_Lab_Tests
    tests/employee_file_tests.cpp
    tests/record_locks_tests.cpp
    tests/protocol_tests.cpp
)

target_link_libraries(OS_Lab_Tests PRIVATE
    os_lab_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(OS_Lab_Tests)