#ifndef _VOXELHANDLER_H_
#define _VOXELHANDLER_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h> 

#include "framebuffer.h"
#include "learnOpenGL/shader.h"

class VoxelHandler 
{
public:
    VoxelHandler(GLFWwindow* window);
    ~VoxelHandler();

    Framebuffer getFBO() { return voxelCoordsFBO; }
private:
    GLFWwindow* window;
    Framebuffer voxelCoordsFBO;
    Shader* voxelShader;
    float voxelRadius;
    GLfloat squareTexCoord[8];

private: 
    void genVoxelPositions(Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
        Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater);
};

#endif