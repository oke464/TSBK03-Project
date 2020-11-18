cmake_minimum_required(VERSION 3.10)

# set the project name
project(myapp)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(
    myapp 
    
    main.cpp
    )

# --- Using GLFW binaries link library like this --
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(myapp glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(myapp OpenGL::GL)

# -------------------------------------------

# --- Using source GLFW link like this ------

#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#add_subdirectory(path/to/glfw)

#target_link_libraries(myapp glfw)

#find_package(OpenGL REQUIRED)
#target_link_libraries(myapp OpenGL::GL)

# ----------------------------------------------