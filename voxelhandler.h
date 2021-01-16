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

    Framebuffer getInitFBO() { return voxelPosFBO; }

    void setVoxelModel(string const &path) { voxelModel = Model(path); }

    void bindBuffersInstanced();
    void bindInitBuffersInstanced();
    void drawVoxelsInstanced(Shader* shader);
    void initDrawVoxelsInstanced(Shader* shader);
    void drawVoxelGrid(glm::mat4 view, glm::mat4 proj);
    void drawVoxelizedModel(glm::mat4 view, glm::mat4 proj, Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
        Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater);
    
    void drawVoxelModel(glm::mat4 view, glm::mat4 proj, Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
        Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater);
    void genVoxelPositions(glm::mat4 view, glm::mat4 proj, Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
        Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater);

    void genActiveVoxelTextures(Framebuffer inputDepthFBO, Framebuffer targetFBO);
    void drawVoxelModel2(glm::mat4 view, glm::mat4 proj, 
            Framebuffer FBOXminActive, Framebuffer FBOYminActive, Framebuffer FBOZminActive, 
            Framebuffer FBOXmaxActive, Framebuffer FBOYmaxActive, Framebuffer FBOZmaxActive);

    void drawVoxelModel3(glm::mat4 view, glm::mat4 proj, 
            glm::mat4 rotation, glm::mat4 translation, float modelScale,   
            Framebuffer FBOXmin, Framebuffer FBOYmin, Framebuffer FBOZmin, 
            Framebuffer FBOXmax, Framebuffer FBOYmax, Framebuffer FBOZmax);

private:
    GLFWwindow* window;
    Shader* voxelShader;
    float voxelRadius;
    GLfloat squareTexCoord[8];
    Shader* voxelLatticeShader;
    Shader* voxelModelShader;
    Shader* voxelInitShader;
    Shader* voxelModelShader2;
    Shader* voxelActiveShader;
    Shader* voxelModelShader3;
    Framebuffer voxelPosFBO;
    float near;
    float far;
    Model voxelModel;
    unsigned int voxPosBuffer;
    unsigned int modelMatrixBuffer;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int modelMatrixBuffer2;
    unsigned int squareTexCoordBuffer2;
    unsigned int VAO2;
    unsigned int VBO2;
    unsigned int EBO2;
    std::vector<glm::mat4> voxelPositionMatrices;
    float voxelSizeScale;
    float modelSizeScale;

    float quadVertices[12]; // Quad 4 vertices
    unsigned int quadIndices[6]; // Used for EBO

private: 
    void initVoxelPosTexture();
    std::vector<glm::vec3> voxelPositions;
};

#endif