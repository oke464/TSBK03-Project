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
    shader.cpp
    )

# --- Link GLAD lib ---
add_subdirectory(glad/)
# -------------------

# --- Add include paths ---------
target_include_directories(Simulation PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}"
                          )


# --- Using GLFW binaries link library like this --
find_package(glfw3 3.3 REQUIRED)

target_link_libraries(
                    Simulation 
                    glfw
                    glad
                    )

find_package(OpenGL REQUIRED)

target_link_libraries(
                    Simulation 
                    OpenGL::GL
                    )

# -------------------------------------------

find_library(COCOA_LIBRARY Cocoa)
find_library(IOKIT_LIBRARY IOKit)
find_library(COREVID_LIBRARY CoreVideo)
message(${COCOA_LIBRARY})
message(${IOKIT_LIBRARY})
message(${COREVID_LIBRARY})


target_link_libraries(Simulation ${COCOA_LIBRARY} ${COREVID_LIBRARY} ${IOKIT_LIBRARY})


IF(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit")
ENDIF(APPLE)


message(${CMAKE_EXE_LINKER_FLAGS})






















# --- Using source GLFW link like this ------

#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#add_subdirectory(path/to/glfw)

#target_link_libraries(Simulation glfw)

#find_package(OpenGL REQUIRED)
#target_link_libraries(Simulation OpenGL::GL)

# ----------------------------------------------



