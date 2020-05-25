cmake_minimum_required(VERSION 3.2)
project(plytapus)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(TestBigEndian)

file(GLOB LIB_SOURCES "plytapus/*.cpp")
file(GLOB TEST_SOURCES "test/*.cpp")

set(PLYTAPUS_MAJOR_VERSION 0)
set(PLYTAPUS_MINOR_VERSION 6)
set(PLYTAPUS_PATCH_VERSION 0)
set(PLYTAPUS_VERSION ${PLYTAPUS_MAJOR_VERSION}.${PLYTAPUS_MINOR_VERSION}.${PLYTAPUS_PATCH_VERSION})

include_directories("plytapus")

add_library(plytapus0 STATIC ${LIB_SOURCES})
add_library(plytapus1 SHARED ${LIB_SOURCES})
add_executable(plytapus_test ${TEST_SOURCES})
target_link_libraries(plytapus_test plytapus0)
set_target_properties(plytapus0 PROPERTIES OUTPUT_NAME "plytapus" VERSION ${PLYTAPUS_VERSION})
set_target_properties(plytapus1 PROPERTIES OUTPUT_NAME "plytapus" VERSION ${PLYTAPUS_VERSION})
install(TARGETS plytapus0 plytapus1 DESTINATION lib)
install(FILES plytapus/plytapus.h plytapus/textio.h DESTINATION include)

include_directories(${PROJECT_BINARY_DIR})
configure_file(config.h.in config.h)
configure_file(test/data/test.ply test.ply COPYONLY)
configure_file(test/data/test_bin.ply test_bin.ply COPYONLY)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT plytapus_test)
