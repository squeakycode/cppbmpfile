add_executable(test_bmpfile_runner
        test_bmpfile.cpp
        )

target_include_directories(test_bmpfile_runner
PRIVATE
${PROJECT_SOURCE_DIR}/test/include
${PROJECT_SOURCE_DIR}/include
)

 get_filename_component(_TEST_DATA_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(test_bmpfile_runner
  PRIVATE
    TEST_DATA_ROOT_PATH="${_TEST_DATA_ROOT_PATH}"
)

custom_target_use_highest_warning_level(test_bmpfile_runner)

add_test(
        NAME test_bmpfile
        COMMAND test_bmpfile_runner
)
