
include_directories(${PROJECT_SOURCE_DIR}/src)

# Scalar test
add_executable(test_scalar Scalar.test.cc)
target_link_libraries(test_scalar lib_Autodiff)
add_test(NAME Test_Scalar COMMAND test_scalar)

# Graph test
add_executable(test_graph DirectedGraph.test.cc)
target_link_libraries(test_graph lib_Autodiff)
add_test(NAME Test_Graph COMMAND test_graph)

# Add computational graph test
add_executable(test_cgraph computational_graph.test.cc)
target_link_libraries(test_cgraph lib_Autodiff)
add_test(NAME Test_Computational_Graph COMMAND test_cgraph)

# Add Unit class test
add_executable(test_unitcl unit_class.test.cc)
target_link_libraries(test_unitcl lib_Autodiff)
add_test(NAME Test_Unit_Class COMMAND test_unitcl)

# Add util functions test
add_executable(test_functions functions.test.cc)
target_link_libraries(test_functions lib_Autodiff)
add_test(NAME Test_Functions COMMAND test_functions)


