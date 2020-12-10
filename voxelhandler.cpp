#include "voxelhandler.h"

VoxelHandler::VoxelHandler(GLFWwindow* window)
    :   window{window},
        voxelCoordsFBO{Framebuffer(1,1)},
        voxelShader{new Shader("voxelShader.vert", "voxelShader.frag")},
        voxelRadius{1.0f},
        squareTexCoord{ 0, 0,
                        0, 1,
                        1, 1,
                        1, 0 }
{   
    int wWidth, wHeight;
    glfwGetWindowSize(window, &wWidth, &wHeight);

    voxelCoordsFBO = Framebuffer(wWidth, wHeight);
}

VoxelHandler::~VoxelHandler()
{
    delete voxelShader;
}

void VoxelHandler::genVoxelPositions(Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
        Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater)
{
    //Bind voxelFBO to write result to 
    voxelCoordsFBO.bindFBO();
    
    // Clear buffer, keep depth to use z-buffer in offscreen fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind all textures to sample depthvalues from
    voxelShader->useProgram();
    glActiveTexture(GL_TEXTURE0);
    FBOX.bindTex(voxelShader, "texFBOX", 0);
    glActiveTexture(GL_TEXTURE1);
    FBOY.bindTex(voxelShader, "texFBOY", 1);
    glActiveTexture(GL_TEXTURE2);
    FBOZ.bindTex(voxelShader, "texFBOZ", 2);
    glActiveTexture(GL_TEXTURE3);
    FBOXGreater.bindTex(voxelShader, "texFBOXGreater", 3);
    glActiveTexture(GL_TEXTURE4);
    FBOYGreater.bindTex(voxelShader, "texFBOYGreater", 4);
    glActiveTexture(GL_TEXTURE5);
    FBOZGreater.bindTex(voxelShader, "texFBOZGreater", 5);

    // Upload voxelradius 
    voxelShader->uploadFloat("voxelRadius", voxelRadius);

    // Bad style now should pass as argument or put tex generation in this class.
    float near = 0;
    float far = 10;
    voxelShader->uploadFloat("near", near);
    voxelShader->uploadFloat("far", far);

    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(squareTexCoord), (void*)squareTexCoord);

    // Unbind voxelFBO
    voxelCoordsFBO.bindScreenFB();
}   