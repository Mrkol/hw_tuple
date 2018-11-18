cmake_minimum_required(VERSION 3.5)

project(allocators CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

enable_testing()

add_executable(
	tuple_unit_tests 
	./src/tuple_unit_tests.cpp
	)

target_include_directories(tuple_unit_tests PUBLIC "./include")

target_link_libraries(tuple_unit_tests 
	PUBLIC GTest::GTest)
