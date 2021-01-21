#include "voxelhandler.h"

//Provide the initialized window, path to model and it's radius. 
VoxelHandler::VoxelHandler(GLFWwindow* window, string const &modelPath, const float & modelRadius)
    :   window{window},
        voxelLatticeShader{new Shader("voxelLatticeShader.vert", "voxelLatticeShader.frag")},
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
        far{10}, // Hardcoded from orthographic frustum size.
        near{0}, // Hardcoded from orthographic frustum size.
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
    delete voxelLatticeShader;
    delete voxelModelShader3;
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

void VoxelHandler::drawVoxelsInstanced(Shader* shader)
{
    shader->useProgram();
    glBindVertexArray(VAO);
    // Draw instanced
    // Again this is hack to utilize model loader to get the indices.
    glDrawElementsInstanced(GL_TRIANGLES, voxelModel.meshes[0].indices.size(), GL_UNSIGNED_INT, 0, voxelPositions.size()); // 6 indices in index buffer object
    glBindVertexArray(0);
}

void VoxelHandler::drawVoxelGrid(glm::mat4 view, glm::mat4 proj)
{
    // Bind screen FB to display in scene
    voxelPosFBO.bindScreenFB();
    voxelLatticeShader->useProgram();

    // Create camera for depth buffer generator
    glm::vec3 dPos(0, 0, 5);
    glm::vec3 dFront(0, 0, -1);
    glm::vec3 dUp(0, 1, 0);
    glm::mat4 depthView = glm::lookAt(dPos, glm::vec3(0, 0, 0), dUp);
    voxelLatticeShader->uploadMat4("View", view);
    
    // Pass sandbox projection matrix
    voxelLatticeShader->uploadMat4("Proj", proj);

    // Upload model scaling, rotaion, translation if desired.
    voxelLatticeShader->uploadFloat("modelScaleFactor", 1.0f);
    voxelLatticeShader->uploadMat4("modelRotation", glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    voxelLatticeShader->uploadMat4("modelTranslation", glm::translate(glm::mat4(1.0f), glm::vec3(9.0f, 5.0f, -25.0f)));
    voxelLatticeShader->uploadMat4("originTranslation", glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -5.0f, 5.0f)));

    bindBuffersInstanced();
    drawVoxelsInstanced(voxelLatticeShader);

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