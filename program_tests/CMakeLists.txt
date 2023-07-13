
include_directories(${PROJECT_SOURCE_DIR}/src)

# Scalar test
add_executable(test_scalar Scalar.test.cc)
target_link_libraries(test_scalar lib_Autodiff)

add_test(NAME Test_Scalar COMMAND test_scalar)


# Graph test
add_executable(test_graph DirectedGraph.test.cc)
target_link_libraries(test_graph lib_Autodiff)

add_test(NAME Test_Graph COMMAND test_graph)
