
# This compiles a library that can be linked to later.
# We begin with the name of the library and then the source files and headers.
add_library(lib_Autodiff STATIC
  # We may add more source files to the library here
  Scalar.cc Scalar.h DirectedGraph.h operations.h Operation.h Variable.h OperationUnary.h OperationBinary.h
  )

# Below we may add out specific compiler flags for the compilation
# This can be done for any compile target, such as add_library, add_executable
target_compile_features(lib_Autodiff PRIVATE cxx_std_17)
target_compile_options(lib_Autodiff PRIVATE -Wall)


add_executable(${CMAKE_PROJECT_NAME} main.cc)

# This links the library to the executable we just added.
target_link_libraries(${CMAKE_PROJECT_NAME} lib_Autodiff)

target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options (${CMAKE_PROJECT_NAME} PRIVATE -Wall)

