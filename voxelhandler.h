#ifndef _VOXELHANDLER_H_
#define _VOXELHANDLER_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <vector> 

#include "framebuffer.h"
#include "learnOpenGL/shader.h"
#include "learnOpenGL/model.h"

class VoxelHandler 
{
public:
    VoxelHandler(GLFWwindow* window, string const &path, const float & scaleFactor);
    ~VoxelHandler();

    Framebuffer getFBO() { return voxelCoordsFBO; }
    Framebuffer getInitFBO() { return voxelInitPosFBO; }

    void setVoxelModel(string const &path) { voxelModel = Model(path); }

    void drawVoxelsInstanced(Shader* shader);
    void drawVoxelGrid(glm::mat4 view, glm::mat4 proj);
    void drawVoxelizedModel(glm::mat4 view, glm::mat4 proj, Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
        Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater);

private:
    GLFWwindow* window;
    Framebuffer voxelCoordsFBO;
    Shader* voxelShader;
    float voxelRadius;
    GLfloat squareTexCoord[8];
    Shader* voxelLatticeShader;
    Framebuffer voxelInitPosFBO;
    float near;
    float far;
    Model voxelModel;
    unsigned int voxPosBuffer;
    unsigned int modelMatrixBuffer;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    std::vector<glm::mat4> voxelPositionMatrices;
    float voxelSizeScale;
    float objectSizeScale;

private: 
    void initVoxelPosTexture();
    void genVoxelPositions(Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
        Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater);
    
    void bindBuffersInstanced();
    


    std::vector<glm::vec3> voxelPositions;

    
};

#endif