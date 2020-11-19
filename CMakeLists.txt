cmake_minimum_required(VERSION 3.10)

# set the project name
project(Simulation)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(
    Simulation 
    main.cpp
    )

# --- Link GLAD lib ---
add_subdirectory(glad/)
# -------------------

#[[
target_include_directories(Simulation PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/Glad/include"

                          )
]]
# --- Using GLFW binaries link library like this --
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(
    Simulation 
    glfw
    glad
    )

find_package(OpenGL REQUIRED)
target_link_libraries(Simulation OpenGL::GL)

# -------------------------------------------

# --- Using source GLFW link like this ------

#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#add_subdirectory(path/to/glfw)

#target_link_libraries(Simulation glfw)

#find_package(OpenGL REQUIRED)
#target_link_libraries(Simulation OpenGL::GL)

# ----------------------------------------------



