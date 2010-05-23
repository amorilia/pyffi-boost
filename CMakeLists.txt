cmake_minimum_required(VERSION 2.6)
project(PYFFI)

# version info
set(PYFFI_VERSION 3.0.0)
set(PYFFI_HEXVERSION 0x030000)

# configuration
configure_file(${PYFFI_SOURCE_DIR}/include/pyffi.hpp.in ${PYFFI_SOURCE_DIR}/include/pyffi.hpp)
configure_file(${PYFFI_SOURCE_DIR}/Doxyfile.in ${PYFFI_SOURCE_DIR}/Doxyfile)

# find boost
find_package(Boost 1.43.0 REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

# include pyffi headers
include_directories(${PYFFI_SOURCE_DIR}/include)

# build the tests
enable_testing()
add_subdirectory(test)
