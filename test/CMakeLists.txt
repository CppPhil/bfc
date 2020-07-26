include(GoogleTest)

set(TEST_NAME bfc_test)

set(TEST_SOURCES src/main.cpp src/f_test.cpp)

add_executable(${TEST_NAME} ${TEST_SOURCES})

target_link_libraries(${TEST_NAME} PRIVATE bfc_lib gtest_main)

gtest_discover_tests(${TEST_NAME} WORKING_DIRECTORY ${bfc_project_SOURCE_DIR})
