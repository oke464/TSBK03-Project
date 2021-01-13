#include "voxelhandler.h"

/*
struct Voxel
{
    glm::vec3 position;
    bool active; 

}Voxel;
*/

//Provide the initialized window, path to model and it's radius. 
VoxelHandler::VoxelHandler(GLFWwindow* window, string const &modelPath, const float & modelRadius)
    :   window{window},
        //voxelCoordsFBO{Framebuffer(1,1)},
        voxelShader{new Shader("voxelShader.vert", "voxelShader.frag")},
        voxelInitShader{new Shader("voxelInitShader.vert", "voxelInitShader.frag")},
        
        quadVertices{
                1.0f,  1.0f, 0.0f,  // top right
                1.0f, -1.0f, 0.0f,  // bottom right
                -1.0f, -1.0f, 0.0f,  // bottom left
                -1.0f,  1.0f, 0.0f   // top left 
                },
        
       /*
        quadVertices{
                0.5f,  0.5f, 0.0f,  // top right
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  // bottom left
                -0.5f,  0.5f, 0.0f   // top left 
                },
        */
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
        voxelLatticeShader{new Shader("voxelLatticeShader.vert", "voxelLatticeShader.frag")},
        voxelModelShader{new Shader("voxelModelShader.vert", "voxelModelShader.frag")},
        voxelPosFBO{Framebuffer(1,1)},
        far{10},
        near{0},
        voxelModel{Model(modelPath)},
        voxelSizeScale{1.0f},
        objectSizeScale{1.0f}
{   
    int wWidth, wHeight;
    glfwGetWindowSize(window, &wWidth, &wHeight);

    //voxelCoordsFBO = Framebuffer(wWidth, wHeight);

    // Generate voxelpositions based on voxelradius and far/voxelbox
    float offset = 2 * voxelRadius * voxelSizeScale;
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(objectSizeScale));

    //glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0,1,0));
    for (float i = near; i < far; i+=offset)
    {
        for (float j = near; j < far; j+=offset)
        {
            for (float k = near; k < far; k+=offset)
            {
                // Last argument is 0 indicates inactive particle.
                voxelPositions.push_back(glm::vec3(i, j, -k));
                glm::vec3 pos = glm::vec3(i, j, -k);
                //std::cout << "Voxels pos: " << pos.x << "  " <<  pos.y << "  " << pos.z << std::endl;   
                
                voxelPositionMatrices.push_back(glm::scale(glm::translate(glm::mat4(1.0f), pos), glm::vec3(voxelSizeScale)));
            }
        }   
    }

    std::cout << "Voxels amount: " << voxelPositions.size() << std::endl;

    // Init with amount of voxels to access by instance number in vertshader. 
    //voxelPosFBO = Framebuffer(voxelPositions.size(), voxelPositions.size());
    //voxelPosFBO = Framebuffer(voxelPositions.size(), 1);
    voxelPosFBO = Framebuffer(wWidth, wHeight);
    
    std::cout << "Voxelmodel meshes: " << voxelModel.meshes.size() << std::endl;

    bindBuffersInstanced();
    bindInitBuffersInstanced();
    //initVoxelPosTexture();
}

VoxelHandler::~VoxelHandler()
{
    delete voxelShader;
    delete voxelLatticeShader;
    delete voxelModelShader;
    delete voxelInitShader;
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
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
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
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    
    glVertexAttribDivisor(2, 1);

    // Bind positions 
    glBindVertexArray(VAO);
    glGenBuffers(1, &voxPosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, voxPosBuffer);
    //std::cout << voxelPositions.size() << std::endl;
    glBufferData(GL_ARRAY_BUFFER, voxelPositions.size() * sizeof(glm::vec3), &voxelPositions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(6, 1);


    // Bind transformation matrices 
    glGenBuffers(1, &modelMatrixBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBuffer);
    glBufferData(GL_ARRAY_BUFFER, voxelPositions.size() * sizeof(glm::mat4), &voxelPositionMatrices[0], GL_STATIC_DRAW);

    glBindVertexArray(VAO);
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
    
void VoxelHandler::bindInitBuffersInstanced()
{
    // create buffers/arrays
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
    
    glBindVertexArray(VAO2);
    glGenBuffers(1, &squareTexCoordBuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, squareTexCoordBuffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoord), &squareTexCoord[0], GL_STATIC_DRAW);  
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
   
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    
    glBindVertexArray(VAO2);
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
    glDrawElementsInstanced(GL_TRIANGLES, voxelModel.meshes[0].indices.size(), GL_UNSIGNED_INT, 0, voxelPositions.size()); // 6 indices in index buffer object
    //std::cout << voxelModel.meshes[0].indices.size()<< std::endl;
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

    // Create camera for depth buffer generator
    voxelModelShader->uploadMat4("dView", view);
    
    // Create Projection for depth buffer generator, will be a box. Use near and far for z-buffer generation.        
    float near = 0.0f;
    float far = 10.0f;
    voxelModelShader->uploadMat4("dProj", proj);

    bindBuffersInstanced();
    drawVoxelsInstanced(voxelModelShader);


    
}