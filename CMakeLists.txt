cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fmacro-prefix-map=${CMAKE_SOURCE_DIR}=)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the name of project 
set(CMAKE_PROJECT_NAME fc)
project(${CMAKE_PROJECT_NAME})

# Create an executable target
add_executable(${CMAKE_PROJECT_NAME})

# Add the source directory
add_subdirectory(src)

# Add the FC directory
add_subdirectory(FC)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
    FC-libs
)

if (BUILD_TEST)
    add_subdirectory(test)
endif()