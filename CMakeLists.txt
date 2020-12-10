cmake_minimum_required(VERSION 3.10)

# This is just to make friends with vscode include and error squiggly lines
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the project name
project(Simulation)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(
    Simulation 
    main.cpp
    learnOpenGL/shader.cpp
    sandbox.cpp
    tiles.cpp
    stb/stb_image.cpp
    learnOpenGL/mesh.cpp
    learnOpenGL/model.cpp
    object.cpp 
    framebuffer.cpp
    voxelhandler.cpp
    )

# --- Link GLAD lib ---
add_subdirectory(glad/)
# -------------------

# Find glm lib
set(glm_DIR /usr/local/Cellar/glm/0.9.9.8/lib/cmake/glm)
find_package(glm REQUIRED)

# --- Add include paths ---------
target_include_directories(Simulation PUBLIC
                          "${PROJECT_BINARY_DIR}" 
                          "${PROJECT_SOURCE_DIR}" 
                          /usr/local/Cellar/glm/0.9.9.8/include
                          /usr/local/Cellar/assimp/5.0.1/include
                          )


# --- Using GLFW binaries link library like this --
find_package(glfw3 3.3 REQUIRED)

set(assimp_DIR /usr/local/Cellar/assimp/5.0.1/lib/cmake/assimp-5.0)
find_package(assimp 5.0 REQUIRED)

target_link_libraries(
                    Simulation 
                    glfw
                    glad
                    glm::glm
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit /usr/local/lib/libassimp.dylib")
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



