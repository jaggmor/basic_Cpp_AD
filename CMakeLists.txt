# The cmake minimum version always needs to be specified.
cmake_minimum_required(VERSION 3.16.3)

# The project must always have a name
project(Autodiff-DL)

# The add subdirectory is like the #include in C++. It starts executing the
# CMakeLists.txt inside the other directory. 
add_subdirectory(src)

# This code will create tests from some executable inside the testing dir.
enable_testing()
add_subdirectory(program_tests)


