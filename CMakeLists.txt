cmake_minimum_required(VERSION 2.6)
project(PYFFI)

# version info
set(PYFFI_VERSION 3.0.0)
set(PYFFI_HEXVERSION 0x030000)

# configuration
configure_file(${PYFFI_SOURCE_DIR}/include/pyffi.hpp.in ${PYFFI_SOURCE_DIR}/include/pyffi.hpp)
configure_file(${PYFFI_SOURCE_DIR}/Doxyfile.in ${PYFFI_SOURCE_DIR}/Doxyfile)

# find boost
if(MINGW)
  set(Boost_COMPILER -gcc45)
endif(MINGW)
find_package(Boost 1.45.0 REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

# include pyffi headers
include_directories(${PYFFI_SOURCE_DIR}/include)

# build the actual library
add_library(pyffi
    src/pyffi/object_models/scope_generate.cpp
    src/pyffi/object_models/scope_parse.cpp
    src/pyffi/object_models/scope_parse_xml.cpp
    src/pyffi/object_models/scope_compile.cpp
    src/pyffi/object_models/scope_fix.cpp
    src/pyffi/object_models/attr.cpp
    src/pyffi/object_models/attr_map.cpp
    src/pyffi/object_models/class.cpp
    src/pyffi/object_models/instance.cpp
    src/pyffi/object_models/scope.cpp
)

# build the tests
enable_testing()
add_subdirectory(test)
