cmake_minimum_required(VERSION 3.18.2)
project(myproject)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_OUTPUT_EXTENSION_REPLACE ON)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)
add_compile_options(-g -O0 --coverage -Dgtest)
link_libraries(gcov)

set(TARGET_LIB flib)
add_library(${TARGET_LIB} STATIC fshare.c fshare.h)

#[[
set(TARGET_APP fshare_app)
add_executable(${TARGET_APP} main.c)
target_include_directories(${TARGET_APP} PRIVATE .)
target_link_directories(${TARGET_APP} PRIVATE .)
target_link_libraries(${TARGET_APP} ${TARGET_LIB})
]]

set(TARGET_TEST run_tests)
add_executable(${TARGET_TEST} test.cc)
target_include_directories(${TARGET_TEST} PRIVATE . googletest/googletest/include/gtest)
target_include_directories(${TARGET_TEST} PRIVATE . googletest/googlemock/include/gmock)
target_link_directories(${TARGET_TEST} PRIVATE . googletest/googletest/build)
target_link_directories(${TARGET_TEST} PRIVATE . googletest/googlemock/build)
target_link_libraries(${TARGET_TEST} PRIVATE ${TARGET_LIB} gtest gtest_main gmock pthread)


set(COVERAGE_REPORT_DIR coverage_report)
set(GCOV_FILE_DIR CMakeFiles/${TARGET_LIB}.dir/src)
add_custom_target(fshare_test
    COMMENT "UNIT TEST"
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    DEPENDS ${TARGET_TEST}
    COMMAND mkdir -p ${COVERAGE_REPORT_DIR}
    COMMAND rm -rf ${COVERAGE_REPORT_DIR})







