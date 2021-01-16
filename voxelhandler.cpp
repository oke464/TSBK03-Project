#include "voxelhandler.h"

//Provide the initialized window, path to model and it's radius. 
VoxelHandler::VoxelHandler(GLFWwindow* window, string const &modelPath, const float & modelRadius)
    :   window{window},
        voxelShader{new Shader("voxelShader.vert", "voxelShader.frag")},
        voxelInitShader{new Shader("voxelInitShader.vert", "voxelInitShader.frag")},
        voxelLatticeShader{new Shader("voxelLatticeShader.vert", "voxelLatticeShader.frag")},
        voxelModelShader{new Shader("voxelModelShader.vert", "voxelModelShader.frag")},
        voxelModelShader2{new Shader("voxelModelShader2.vert", "voxelModelShader2.frag")},
        voxelActiveShader{new Shader("voxelActiveTest.vert", "voxelActiveTest.frag")},
        voxelModelShader3{new Shader("voxelModelShader3.vert", "voxelModelShader3.frag")},
        
        quadVertices{
                1.0f,  1.0f, 0.0f,  // top right
                1.0f, -1.0f, 0.0f,  // bottom right
                -1.0f, -1.0f, 0.0f,  // bottom left
                -1.0f,  1.0f, 0.0f   // top left 
                },
        quadIndices{  
                    0, 1, 3,   // first triangle
                    1, 2, 3    // second triangle
                    },
        voxelRadius{modelRadius},
        squareTexCoord{ 
                        0, 0,
                        0, 1,
                        1, 1,
                        1, 0
                      },

        voxelPosFBO{Framebuffer(1,1)},
        far{10},
        near{0},
        voxelModel{Model(modelPath)},
        voxelSizeScale{0.25f}, // Change voxel size here (size of cubes). 0.3f has good performance, but low resolution. 
        modelSizeScale{1.0f}
{   
    // This creates an offset where to place neighbouring voxel based on the scale size and radius (radius is sidelength for cube), 
    // in order to fill the entire voxelspace with packed voxels.
    float offset = 2 * voxelRadius * voxelSizeScale;

    // Generate voxelpositions based on voxelradius and far/voxelbox
    // This generates the voxel positions in the orthographic frustum [0,10].
    // Calling it the voxelSpace. 
    for (float i = near; i < far; i+=offset)
    {
        for (float j = near; j < far; j+=offset)
        {
            for (float k = near; k < far; k+=offset)
            {
                // Create vec3 for voxel position. And mat4 as transformation matrix that will be uploaded to shader.
                // Only the matrix is used in final version.
                voxelPositions.push_back(glm::vec3(i, j, -k));
                glm::vec3 pos = glm::vec3(i, j, -k);
                voxelPositionMatrices.push_back(glm::scale(glm::translate(glm::mat4(1.0f), pos), glm::vec3(voxelSizeScale)));
            }
        }   
    }

    std::cout << "Voxels amount per model: " << voxelPositions.size() << std::endl;
    
    std::cout << "Voxelmodel meshes: " << voxelModel.meshes.size() << std::endl;

    /* These are some initializations of FBOs tested, not used for version 3 (final version) voxelization. */
    // Init with amount of voxels to access by instance number in vertshader. 
    //voxelPosFBO = Framebuffer(voxelPositions.size(), voxelPositions.size());
    //voxelPosFBO = Framebuffer(voxelPositions.size(), 1);
    int wWidth, wHeight;
    glfwGetWindowSize(window, &wWidth, &wHeight);
    voxelPosFBO = Framebuffer(wWidth, wHeight);
}

VoxelHandler::~VoxelHandler()
{
    delete voxelShader;
    delete voxelLatticeShader;
    delete voxelModelShader;
    delete voxelInitShader;
    delete voxelModelShader2;
    delete voxelActiveShader;
    delete voxelModelShader3;
}

void VoxelHandler::initVoxelPosTexture()
{
/*
THis should just render a quad on offscreen FBO with NxN amount of particles with vertex colors as position indication.
Must have 8 vertices, then color so that all positions are covered. Then we can scale so that they are spaced with 2*radius.
LIKE: 


        0,0,1                     1,0,1

0,0,0                     1,0,0


        0,1,1                     1,1,1

0,1,0                     1,1,0

This will not work! 
Vertex position have to be evenly spaced, to get the right amount of fragments.
0.25 between each I guess.

Then it should be texture size: 0.25*N x 4*N.

Remember to disable depthtest. See some guide on FBO rendering. 


Extension write vertices as a grid in 2D with known positions. Then give each of those vertices the color of as "real" position vector.
*/
    voxelPosFBO.bindFBO();
    // Clear buffer, keep depth to use z-buffer in offscreen fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.9f, 0.6f, 0.6f, 1.0f);

    voxelShader->useProgram();
    // Upload texCoords for a square
    /*
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(squareTexCoord), (void*)squareTexCoord);
    */
    // Draw init voxelpositions to texture.
    

    voxelPosFBO.bindScreenFB();
}

void VoxelHandler::genVoxelPositions(glm::mat4 view, glm::mat4 proj, Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
        Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater)
{

    glDisable(GL_DEPTH_TEST);
    //Bind voxelFBO to write result to 
    voxelPosFBO.bindFBO();
    //voxelPosFBO.bindScreenFB();
    // Clear buffer, keep depth to use z-buffer in offscreen fbo
    // Set aplha to 0 because if voxelModelShader checks if alpha == 1 to know if voxel is active
    glClearColor(0,1,0,0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    voxelInitShader->useProgram();

    // Use near and far for z-buffer generation.        
    float near = 0.0f;
    float far = 10.0f;
    voxelInitShader->uploadFloat("near", near);
    voxelInitShader->uploadFloat("far", far);

    // Bind all textures to sample depthvalues from
    voxelInitShader->useProgram();
    glActiveTexture(GL_TEXTURE0);
    FBOX.bindTex(voxelInitShader, "texFBOX", 0);
    glActiveTexture(GL_TEXTURE1);
    FBOY.bindTex(voxelInitShader, "texFBOY", 1);
    glActiveTexture(GL_TEXTURE2);
    FBOZ.bindTex(voxelInitShader, "texFBOZ", 2);
    glActiveTexture(GL_TEXTURE3);
    FBOXGreater.bindTex(voxelInitShader, "texFBOXGreater", 3);
    glActiveTexture(GL_TEXTURE4);
    FBOYGreater.bindTex(voxelInitShader, "texFBOYGreater", 4);
    glActiveTexture(GL_TEXTURE5);
    FBOZGreater.bindTex(voxelInitShader, "texFBOZGreater", 5);

    bindInitBuffersInstanced();
    initDrawVoxelsInstanced(voxelInitShader);
    // Unbind voxelFBO
    voxelPosFBO.bindScreenFB();

    glEnable(GL_DEPTH_TEST);
}   

void VoxelHandler::bindBuffersInstanced()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // set the vertex attribute pointers # WARNING # Uses Vertex struct in mashes class
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Fetching the data hardcoded like this is a hack to utilized the model loader, note that we get the first index, this works 
    // because we only have one mesh for a square, don't know what happens if we were to have multiple meshes. 
    glBufferData(GL_ARRAY_BUFFER, voxelModel.meshes[0].vertices.size() * sizeof(Vertex), &voxelModel.meshes[0].vertices[0], GL_STATIC_DRAW);  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, voxelModel.meshes[0].indices.size() * sizeof(unsigned int), &voxelModel.meshes[0].indices[0], GL_STATIC_DRAW);
    
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent /* NOT USED IN SHADER */
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent /* NOT USED IN SHADER */
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    // Bind positions /* NOT USED IN SHADER */ 
    glBindVertexArray(VAO);
    glGenBuffers(1, &voxPosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, voxPosBuffer);
    glBufferData(GL_ARRAY_BUFFER, voxelPositions.size() * sizeof(glm::vec3), &voxelPositions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(6, 1);

    // Bind transformation matrices 
    glGenBuffers(1, &modelMatrixBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBuffer);
    glBufferData(GL_ARRAY_BUFFER, voxelPositions.size() * sizeof(glm::mat4), &voxelPositionMatrices[0], GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    std::size_t vec4Size = sizeof(glm::vec4);
    // Uploads matrix row by row
    glEnableVertexAttribArray(7); 
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(8); 
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(9); 
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(10); 
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);

    glBindVertexArray(0);
}

void VoxelHandler::bindInitBuffersInstanced()
{
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO2);

    glBindVertexArray(VAO2);
    // set the vertex attribute pointers # WARNING # Uses Vertex struct in mashes class
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    // vertex Positions
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);  
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    
    // Vertex texture coords. One per instance so no attribDivisor.
    glGenBuffers(1, &squareTexCoordBuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, squareTexCoordBuffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoord), squareTexCoord, GL_STATIC_DRAW);  
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
   
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    
    
    // Bind transformation matrices 
    glGenBuffers(1, &modelMatrixBuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBuffer2);
    glBufferData(GL_ARRAY_BUFFER, voxelPositions.size() * sizeof(glm::mat4), &voxelPositionMatrices[0], GL_STATIC_DRAW);

    // vertex attributes
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(7); 
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(8); 
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(9); 
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(10); 
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
    

    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);

    glBindVertexArray(0);
}

void VoxelHandler::drawVoxelsInstanced(Shader* shader)
{
    shader->useProgram();
    glBindVertexArray(VAO);
    // Draw instanced
    // Again this is hack to utilize model loader to get the indices.
    glDrawElementsInstanced(GL_TRIANGLES, voxelModel.meshes[0].indices.size(), GL_UNSIGNED_INT, 0, voxelPositions.size()); // 6 indices in index buffer object
    glBindVertexArray(0);
}

void VoxelHandler::initDrawVoxelsInstanced(Shader* shader)
{
    shader->useProgram();
    glBindVertexArray(VAO2);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, voxelPositions.size()); // 6 indices in index buffer object
    //std::cout << voxelModel.meshes[0].indices.size()<< std::endl;
    glBindVertexArray(0);
}

void VoxelHandler::drawVoxelGrid(glm::mat4 view, glm::mat4 proj)
{
    // Unbind voxelFBO
    voxelPosFBO.bindScreenFB();
    voxelLatticeShader->useProgram();
    // Bind FBO to get output.

    // Create camera for depth buffer generator
    glm::vec3 dPos(0, 0, 5);
    glm::vec3 dFront(0, 0, -1);
    glm::vec3 dUp(0, 1, 0);
    glm::mat4 depthView = glm::lookAt(dPos, glm::vec3(0, 0, 0), dUp);
    voxelLatticeShader->uploadMat4("dView", view);
    
    // Create Projection for depth buffer generator, will be a box. Use near and far for z-buffer generation.
    float left = -0.5f;
    float right = 0.5f;
    float bottom = -0.5f;
    float top = 0.5f;          
    float near = 0.0f;
    float far = 10.0f;
    glm::mat4 depthProj = glm::ortho(left, right, bottom, top, near, far);
    voxelLatticeShader->uploadMat4("dProj", proj);
    //voxelLatticeShader->uploadFloat("near", near);
    //voxelLatticeShader->uploadFloat("far", far);

    drawVoxelsInstanced(voxelLatticeShader);

}

void VoxelHandler::drawVoxelizedModel(glm::mat4 view, glm::mat4 proj, Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
        Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater)
{
    // Unbind voxelFBO
    voxelPosFBO.bindScreenFB();
    voxelShader->useProgram();
    // Bind FBO to get output.

    // Create camera for depth buffer generator
    voxelShader->uploadMat4("dView", view);
    
    // Create Projection for depth buffer generator, will be a box. Use near and far for z-buffer generation.        
    float near = 0.0f;
    float far = 10.0f;
    voxelShader->uploadMat4("dProj", proj);
    //voxelLatticeShader->uploadFloat("near", near);
    //voxelLatticeShader->uploadFloat("far", far);

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



    drawVoxelsInstanced(voxelShader);

}

void VoxelHandler::drawVoxelModel(glm::mat4 view, glm::mat4 proj, Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
        Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater)
{
    /*
    genVoxelPositions(view, proj, FBOX, FBOY,FBOZ, 
        FBOXGreater, FBOYGreater, FBOZGreater);
    */
    voxelPosFBO.bindScreenFB();
    voxelModelShader->useProgram();

    glActiveTexture(GL_TEXTURE0);
    voxelPosFBO.bindTex(voxelModelShader, "voxPosTex", 0);

    //###!!! Remove later
    glActiveTexture(GL_TEXTURE1);
    FBOY.bindTex(voxelModelShader, "texFBOY", 1);

    // Pass sandbox camera
    voxelModelShader->uploadMat4("dView", view);
    
    // Pass sandbox projection. don't think near and far are used.      
    float near = 0.0f;
    float far = 10.0f;
    voxelModelShader->uploadMat4("dProj", proj);

    bindBuffersInstanced();
    drawVoxelsInstanced(voxelModelShader);


    
}

void VoxelHandler::genActiveVoxelTextures(Framebuffer depthFBO, Framebuffer targetFBO)
{
    // Don't know if this makes a difference for offscreen FBO, but did some debugging with on-screen so needed there. 
    glDisable(GL_DEPTH_TEST);

    // Bind target FBO
    targetFBO.bindFBO();
    //targetFBO.bindScreenFB();

    // Clear buffer, keep depth to use z-buffer in offscreen fbo
    // Set aplha to 0 because if voxelModelShader checks if alpha == 1 to know if voxel is active
    glClearColor(0,1,0,0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    voxelActiveShader->useProgram();

    // Bind texture containing depth data.
    glActiveTexture(GL_TEXTURE1);
    depthFBO.bindTex(voxelActiveShader, "depthTex", 1);

    voxelActiveShader->useProgram();
    // Use near and far for scaling the voxel position to z-buffer space [0,1]   
    float near = 0.0f;
    float far = 10.0f;
    voxelActiveShader->uploadFloat("near", near);
    voxelActiveShader->uploadFloat("far", far);

    // Bind everything and draw
    bindInitBuffersInstanced();
    initDrawVoxelsInstanced(voxelActiveShader);

    targetFBO.bindScreenFB();

    glEnable(GL_DEPTH_TEST);
}

void VoxelHandler::drawVoxelModel2(glm::mat4 view, glm::mat4 proj, 
    Framebuffer FBOXminActive, Framebuffer FBOYminActive, Framebuffer FBOZminActive, 
    Framebuffer FBOXmaxActive, Framebuffer FBOYmaxActive, Framebuffer FBOZmaxActive)

{
    // Bind screen FBO
    voxelPosFBO.bindScreenFB();

    // Use correct shader
    voxelModelShader2->useProgram();

    // Upload all 6 active voxel test textures
    glActiveTexture(GL_TEXTURE0);
    FBOXminActive.bindTex(voxelModelShader2, "FBOXminActive", 0);
    glActiveTexture(GL_TEXTURE1);
    FBOYminActive.bindTex(voxelModelShader2, "FBOYminActive", 1);
    glActiveTexture(GL_TEXTURE2);
    FBOZminActive.bindTex(voxelModelShader2, "FBOZminActive", 2);
    glActiveTexture(GL_TEXTURE3);
    FBOXmaxActive.bindTex(voxelModelShader2, "FBOXmaxActive", 3);
    glActiveTexture(GL_TEXTURE4);
    FBOYmaxActive.bindTex(voxelModelShader2, "FBOYmaxActive", 4);
    glActiveTexture(GL_TEXTURE5);
    FBOZmaxActive.bindTex(voxelModelShader2, "FBOZmaxActive", 5);

    // Upload view and projection matrix 
    // Pass sandbox camera
    voxelModelShader2->uploadMat4("dView", view);
    // Pass sandbox projection. 
    voxelModelShader2->uploadMat4("dProj", proj);

    // Bind and draw instanced.
    bindBuffersInstanced();
    drawVoxelsInstanced(voxelModelShader2);


}

void VoxelHandler::drawVoxelModel3(glm::mat4 view, glm::mat4 proj, 
        glm::mat4 rotation, glm::mat4 translation, float modelScale,   
        Framebuffer FBOXmin, Framebuffer FBOYmin, Framebuffer FBOZmin, 
        Framebuffer FBOXmax, Framebuffer FBOYmax, Framebuffer FBOZmax)
{
    // Bind screenFB
    voxelPosFBO.bindScreenFB();

    // Use correct shader
    voxelModelShader3->useProgram();

    // Upload view and projection matrices
    // Pass sandbox camera
    voxelModelShader3->uploadMat4("dView", view);
    // Pass sandbox projection. 
    voxelModelShader3->uploadMat4("dProj", proj);

    // Upload all 6 depthbuffer textures
    glActiveTexture(GL_TEXTURE0);
    FBOXmin.bindTex(voxelModelShader3, "FBOXmin", 0);
    glActiveTexture(GL_TEXTURE1);
    FBOYmin.bindTex(voxelModelShader3, "FBOYmin", 1);
    glActiveTexture(GL_TEXTURE2);
    FBOZmin.bindTex(voxelModelShader3, "FBOZmin", 2);
    glActiveTexture(GL_TEXTURE3);
    FBOXmax.bindTex(voxelModelShader3, "FBOXmax", 3);
    glActiveTexture(GL_TEXTURE4);
    FBOYmax.bindTex(voxelModelShader3, "FBOYmax", 4);
    glActiveTexture(GL_TEXTURE5);
    FBOZmax.bindTex(voxelModelShader3, "FBOZmax", 5);

    // Upload near and far to scale voxel positions to z-buffer coords
    float near = 0.0f;
    float far = 10.0f;
    voxelModelShader3->uploadFloat("near", near);
    voxelModelShader3->uploadFloat("far", far);

    // Upload depthFBO width and height (change these to be the same later) use for transforming look-up coordinates.
    // Supposed to be same for all FB0s 
    voxelModelShader3->uploadFloat("texWidth", FBOXmax.width);
    voxelModelShader3->uploadFloat("texHeight", FBOXmax.height);

    // Upload model scaling, rotaion, translation if desired.
    voxelModelShader3->uploadFloat("modelScaleFactor", modelScale);
    voxelModelShader3->uploadMat4("modelRotation", rotation);
    voxelModelShader3->uploadMat4("modelTranslation", translation);

    // This translates voxel space x: [0,10]; y: [0,10]; z: [0,-10]. To be in origin such that: x: [-5,5]; y: [-5,5]; z: [5,-5].
    // Then we can apply translation and rotation as we are used to. 
    voxelModelShader3->uploadMat4("originTranslation", glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -5.0f, 5.0f)));

    // Bind and draw instanced
    bindBuffersInstanced();
    drawVoxelsInstanced(voxelModelShader3);
}