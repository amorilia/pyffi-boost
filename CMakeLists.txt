cmake_minimum_required(VERSION 2.6)
project(PYFFI)

# find boost
find_package(Boost 1.41.0 REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

# include pyffi headers
include_directories(${PYFFI_SOURCE_DIR}/include)

# build the tests
enable_testing()
add_subdirectory(test)
