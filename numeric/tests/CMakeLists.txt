cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(numbers_test)

#find_library(NUMERIC Numeric PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ REQUIRED)
find_package(Catch2 REQUIRED)

file(GLOB TEST_FILES *test.cpp)
# add_executable(numbers_test numbers_test.cpp)
add_executable(numbers_test ${TEST_FILES})
target_link_libraries(numbers_test PUBLIC  gmp gmpxx Catch2::Catch2WithMain)
target_include_directories(numbers_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/)