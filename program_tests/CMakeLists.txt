
include_directories (${PROJECT_SOURCE_DIR}/src)

add_executable (test_scalar Scalar.test.cc)
target_link_libraries(test_scalar lib_Autodiff)

add_test(NAME Test_Scalar COMMAND test_scalar)
