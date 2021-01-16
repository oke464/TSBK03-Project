#include "sandbox.h"

#include <iostream>

Sandbox::Sandbox(GLFWwindow* window) :
    triangleVertices{-0.5f, -0.5f, 0.0f, // left  
                    0.5f, -0.5f, 0.0f, // right 
                    0.0f,  0.5f, 0.0f  // top
                    },
    quadVertices{
                -0.5f, -0.5f, 0.0f,  // bottom left
                0.5f, -0.5f, 0.0f,  // bottom right
                0.5f,  0.5f, 0.0f,  // top right
                -0.5f,  0.5f, 0.0f   // top left 
                },
    quadIndices{  
                0, 1, 3,   // first triangle
                1, 2, 3    // second triangle
                },
    quadTexCoords{
                   0, 0,
                   1, 0,
                   1, 1,
                   0, 1
                  },
    sandboxShader{new Shader("sandboxShader.vert", "sandboxShader.frag")},
    cameraPos{glm::vec3(0.0f, 5.0f,  3.0f)},
    cameraFront{glm::vec3(0.0f, 0.0f, -1.0f)},
    cameraUp{glm::vec3(0.0f, 1.0f,  0.0f)},
    quadModel{glm::rotate(glm::mat4{1.0f}, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f))},
    window{window},
    bottomTiles{new Tiles(sandboxShader, 30, 50)},
    yaw{-90.0f},
    pitch{0.0f},
    bunnyShader{new Shader("bunnyShader.vert", "bunnyShader.frag")},
    bunny{Sphere(glm::vec3(0.0f, 0.0f, 0.0f), "assets/models/stanford-bunny.obj")},
    sphere{Sphere(glm::vec3(0.0f, 0.0f, 0.0f), "assets/models/sphere.obj")},
    cube{Sphere(glm::vec3(0.0f, 0.0f, 0.0f), "assets/models/cubeplus.obj")},
    depthShader(new Shader("depthBufferTestShader.vert", "depthBufferTestShader.frag")),
    cubeShader(new Shader("cubeShader.vert", "cubeShader.frag")),
    voxHandler{new VoxelHandler(window, "assets/models/cubeplus.obj", 0.5f)},
    bunnyToggle{false}
{
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Window width Height
    int wWidth, wHeight;
    glfwGetWindowSize(window, &wWidth, &wHeight);
    // aspect ratio = wWidth / wHeight
    projection = glm::perspective(glm::radians(45.0f), (float)(wWidth / wHeight), 0.1f, 100.0f);

    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, scrollCallback);
}

Sandbox::~Sandbox()
{
    delete sandboxShader;
    delete bottomTiles;
    delete bunnyShader;
    delete depthShader;
    delete cubeShader;
    delete voxHandler;
}

// Simple input handling function
void Sandbox::processInput()
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 0.1f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    /*
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    */
    const float rotSpeed = 0.6f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pitch += rotSpeed;
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Bunnytoggle
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        bunnyToggle = !bunnyToggle;
}

// Main loop to display everything in scene.
void Sandbox::display()
{
    int count = 0;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE); 

    float prevTime = glfwGetTime();
    glm::vec3 moveDir(0, 0, 0);//(0, 0, -1);
    glm::vec3 transVec(0, -0.1, 0);

    int wWidth, wHeight;
    glfwGetWindowSize(window, &wWidth, &wHeight);
    
    // Create a decent sized texture to get good enough resolution.
    int texSize = 2048;
    Framebuffer depthFBO_Zmin(texSize, texSize);
    Framebuffer depthFBO_Ymin(texSize, texSize);
    Framebuffer depthFBO_Xmin(texSize, texSize);
    Framebuffer depthFBO_Zmax(texSize, texSize);
    Framebuffer depthFBO_Ymax(texSize, texSize);
    Framebuffer depthFBO_Xmax(texSize, texSize);
/* Version2
    Framebuffer activeVoxelFBO_Zmin(wWidth, wHeight);
    Framebuffer activeVoxelFBO_Ymin(wWidth, wHeight);
    Framebuffer activeVoxelFBO_Xmin(wWidth, wHeight);
    Framebuffer activeVoxelFBO_Zmax(wWidth, wHeight);
    Framebuffer activeVoxelFBO_Ymax(wWidth, wHeight);
    Framebuffer activeVoxelFBO_Xmax(wWidth, wHeight);
*/

    // This generates textures with depth data
    genereteFBODepthTextures(depthFBO_Xmin, depthFBO_Ymin, depthFBO_Zmin, 
                             depthFBO_Xmax, depthFBO_Ymax, depthFBO_Zmax);

    glDepthFunc(GL_LESS);
    glClearDepth(1.0);

/* Version1
    // Generate a texture with voxelpositions, an activate voxel if it is within the
    // limits of z-buffer textures.
    voxHandler->genVoxelPositions(view, projection, 
            depthFBO_Xmin, depthFBO_Ymin, depthFBO_Zmin, 
            depthFBO_Xmax, depthFBO_Ymax, depthFBO_Zmax);
*/
/* Version2
    // Generate voxel active textures these are used to check in voxelmodel draw call.
    voxHandler->genActiveVoxelTextures(depthFBO_Zmin, activeVoxelFBO_Zmin);
    voxHandler->genActiveVoxelTextures(depthFBO_Xmin, activeVoxelFBO_Xmin);
    voxHandler->genActiveVoxelTextures(depthFBO_Ymin, activeVoxelFBO_Ymin);
    voxHandler->genActiveVoxelTextures(depthFBO_Zmax, activeVoxelFBO_Zmax);
    voxHandler->genActiveVoxelTextures(depthFBO_Xmax, activeVoxelFBO_Xmax);
    voxHandler->genActiveVoxelTextures(depthFBO_Ymax, activeVoxelFBO_Ymax);
*/    
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Handle inputs
        processInput();
 
        // Set backgroundcolor
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        // Clear buffer 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ### Render here #######
        float t = glfwGetTime() * 100;

        float currTime = glfwGetTime();
        float deltaT = (currTime - prevTime) / 2;
        prevTime = currTime;

        if (bunnyToggle)
        { 
            // --------- To bunny in screen fbo, click "b" in program
            depthShader->useProgram();
            // Bind FBO to get output.

            // Create camera for depth buffer generator
            glm::vec3 dPos(0, 0, 5);
            glm::vec3 dFront(0, 0, -1);
            glm::vec3 dUp(0, 1, 0);
            glm::mat4 depthView = glm::lookAt(dPos, glm::vec3(0, 0, 0), dUp);
            depthShader->uploadMat4("dView", depthView);
            
            // Create Projection for depth buffer generator, will be a box. Use near and far for z-buffer generation.
            float left = -0.5f;
            float right = 0.5f;
            float bottom = -0.5f;
            float top = 0.5f;
            float near = 0.0f;
            float far = 10.0f;
            glm::mat4 depthProj = glm::ortho(left, right, bottom, top, near, far);

            depthShader->uploadMat4("dProj", depthProj);
            depthShader->uploadFloat("near", near);
            depthShader->uploadFloat("far", far);

            if(transVec.z <= -6 || transVec.z >= 6)
            {
                //std::cout << "change dir: " << transVec.z << std::endl;
                moveDir = -moveDir;
                //std::cout << moveDir.z << std::endl;
            }

            // Update translation vector dep on time
            transVec += deltaT * moveDir;
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(5,5,5));

            // Rotation to Z direction
            glm::mat4 rotation = glm::rotate(scale, glm::radians(t), glm::vec3(0, 1, 0)); // Set t in radians to rotate object.

            glm::mat4 model = glm::translate(rotation, transVec);
            
            depthShader->uploadMat4("model", model);
            // Draw everything to offscreen buffer
            bunny.draw(*depthShader);
            // --------- 
        }


        //voxHandler->drawVoxelGrid(view, projection);
/*
        voxHandler->drawVoxelizedModel(view, projection, 
            depthFBO_Xmin, depthFBO_Ymin, depthFBO_Zmin, 
            depthFBO_Xmax, depthFBO_Ymax, depthFBO_Zmax);

*/
/* Version1 UNCOMMENT if we want to generate textures each frame
        voxHandler->genVoxelPositions(view, projection, 
            depthFBO_Xmin, depthFBO_Ymin, depthFBO_Zmin, 
            depthFBO_Xmax, depthFBO_Ymax, depthFBO_Zmax);
*/
/* Version1
        voxHandler->drawVoxelModel(view, projection, 
            depthFBO_Xmin, depthFBO_Ymin, depthFBO_Zmin, 
            depthFBO_Xmax, depthFBO_Ymax, depthFBO_Zmax);
*/
/* Version 2
        voxHandler->drawVoxelModel2(view, projection, 
            activeVoxelFBO_Xmin, activeVoxelFBO_Ymin, activeVoxelFBO_Zmin, 
            activeVoxelFBO_Xmax, activeVoxelFBO_Ymax, activeVoxelFBO_Zmax);
*/

        voxHandler->drawVoxelModel3(view, projection, 
            glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
            glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, -5.0f)),
            0.5f,
            depthFBO_Xmin, depthFBO_Ymin, depthFBO_Zmin, 
            depthFBO_Xmax, depthFBO_Ymax, depthFBO_Zmax);

        voxHandler->drawVoxelModel3(view, projection, 
            glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
            glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 5.0f, -5.0f)),
            0.5f,
            depthFBO_Xmin, depthFBO_Ymin, depthFBO_Zmin, 
            depthFBO_Xmax, depthFBO_Ymax, depthFBO_Zmax);

        // ------ For demo, display depth textures on quads around bunny
        // z-dir
        quadModel = glm::rotate(glm::mat4{1.0f}, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        quadModel = glm::translate(quadModel, glm::vec3(20.0f, 4.0f, -12.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        depthFBO_Zmin.bindTex(cubeShader, "texUnit", 0);
        drawQuad();

        quadModel = glm::rotate(glm::mat4{1.0f}, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        quadModel = glm::translate(quadModel, glm::vec3(20.0f, 4.0f, -18.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        depthFBO_Zmax.bindTex(cubeShader, "texUnit", 0);
        drawQuad();

        // x-dir
        quadModel = glm::translate(glm::mat4{1.0f}, glm::vec3(17.0f, 4.0f, -15.0f));
        quadModel = glm::rotate(quadModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        depthFBO_Xmin.bindTex(cubeShader, "texUnit", 0);
        drawQuad();

        quadModel = glm::translate(glm::mat4{1.0f}, glm::vec3(23.0f, 4.0f, -15.0f));
        quadModel = glm::rotate(quadModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        depthFBO_Xmax.bindTex(cubeShader, "texUnit", 0);
        drawQuad();

        // y-dir
        quadModel = glm::translate(glm::mat4{1.0f}, glm::vec3(20.0f, 7.0f, -15.0f));
        quadModel = glm::rotate(quadModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        depthFBO_Ymin.bindTex(cubeShader, "texUnit", 0);
        drawQuad();

        quadModel = glm::translate(glm::mat4{1.0f}, glm::vec3(20.0f, 1.0f, -15.0f));
        quadModel = glm::rotate(quadModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        depthFBO_Ymax.bindTex(cubeShader, "texUnit", 0);
        drawQuad();
        // ------
/* Version3 
        // ------------ Demo display active voxeltextures
        float xOffset = -7.0f;
        // z-dir
        quadModel = glm::rotate(glm::mat4{1.0f}, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        quadModel = glm::translate(quadModel, glm::vec3(20.0f + xOffset, 4.0f, -12.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        activeVoxelFBO_Zmin.bindTex(cubeShader, "texUnit", 0);
        drawQuad();

        quadModel = glm::rotate(glm::mat4{1.0f}, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        quadModel = glm::translate(quadModel, glm::vec3(20.0f + xOffset, 4.0f, -18.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        activeVoxelFBO_Zmax.bindTex(cubeShader, "texUnit", 0);
        drawQuad();

        // x-dir
        quadModel = glm::translate(glm::mat4{1.0f}, glm::vec3(17.0f + xOffset, 4.0f, -15.0f));
        quadModel = glm::rotate(quadModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        activeVoxelFBO_Xmin.bindTex(cubeShader, "texUnit", 0);
        drawQuad();

        quadModel = glm::translate(glm::mat4{1.0f}, glm::vec3(23.0f + xOffset, 4.0f, -15.0f));
        quadModel = glm::rotate(quadModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        activeVoxelFBO_Xmax.bindTex(cubeShader, "texUnit", 0);
        drawQuad();

        // y-dir
        quadModel = glm::translate(glm::mat4{1.0f}, glm::vec3(20.0f + xOffset, 7.0f, -15.0f));
        quadModel = glm::rotate(quadModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        activeVoxelFBO_Ymin.bindTex(cubeShader, "texUnit", 0);
        drawQuad();

        quadModel = glm::translate(glm::mat4{1.0f}, glm::vec3(20.0f + xOffset, 1.0f, -15.0f));
        quadModel = glm::rotate(quadModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        quadModel = glm::scale(quadModel, glm::vec3(5.0f, 5.0f, 5.0f));
        cubeShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        activeVoxelFBO_Ymax.bindTex(cubeShader, "texUnit", 0);
        drawQuad();
        // ------
*/
        // ------------------------------------


        // ------------ Things in scene ----------------------
        // Tiles bottom
        sandboxShader->useProgram();
        sandboxShader->uploadMat4("projection", projection);
        sandboxShader->uploadMat4("view", view);
        
        bottomTiles->bindBuffersInstanced();
        bottomTiles->drawTilesInstanced();

        // Draw bunny
        bunnyShader->useProgram();
        bunnyShader->uploadMat4("projection", projection);
        bunnyShader->uploadMat4("view", view);

        bunny.setPosition(glm::vec3(20.0f, 4.0f, -15.0f));
        bunny.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
        bunny.updateTransformation();
        bunnyShader->uploadMat4("model", bunny.getTransformation());
        bunny.draw(*bunnyShader);

        // ------------------------------------
        //std::cout << glGetError() << std::endl;

        // #######################

        // Swap front and back buffers 
        glfwSwapBuffers(window);
        // Poll for and process events 
        glfwPollEvents();
    }
}

void Sandbox::initShaders()
{

}

void Sandbox::genereteFBODepthTextures(Framebuffer FBOX, Framebuffer FBOY, Framebuffer FBOZ, 
                Framebuffer FBOXGreater, Framebuffer FBOYGreater, Framebuffer FBOZGreater)
{
    // ----------- DEPTH TESTING STUFF ----------------
    //std::cout << deltaT << std::endl;
    depthShader->useProgram();
    // Bind FBO to get output.

    // Create camera for depth buffer generator
    glm::vec3 dPos(0, 0, 5);
    glm::vec3 dFront(0, 0, -1);
    glm::vec3 dUp(0, 1, 0);
    glm::mat4 depthView = glm::lookAt(dPos, glm::vec3(0, 0, 0), dUp);
    depthShader->uploadMat4("dView", depthView);
    
    // Create Projection for depth buffer generator, will be a box. Use near and far for z-buffer generation.
    float left = -0.5f;
    float right = 0.5f;
    float bottom = -0.5f;
    float top = 0.5f;
    float near = 0.0f;
    float far = 10.0f;
    glm::mat4 depthProj = glm::ortho(left, right, bottom, top, near, far);
    depthShader->uploadMat4("dProj", depthProj);
    depthShader->uploadFloat("near", near);
    depthShader->uploadFloat("far", far);

    // --- Draw model 3 times rotated along axises one time with depth func 
    // greater and one with less to get depth values from each side. 
    // Create model = rot * trans 

    // Scale all to a decent size, otherwise the depthbuffer will have small margins.
    // Resulting in no voxels inside object...
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(5,5,5));

    // THE GLM math lib SEEMS TO apply first function call last for some reason (left to right) so call translate last -> applies first
    // Translates BUNNY's center to center of screen
    glm::mat4 trans = glm::translate(scale, glm::vec3(0, -0.1, 0)); 

    // --- Rotations
    //      By default we look in negative z-direction. To make all depth buffers align we have to rotate the object to look in negative x and negative y direction respectively. 
    //      Then we can make our depth test textures with the same testfunction.
    // Rotation to Z direction
    glm::mat4 modelDirZ = glm::rotate(scale, glm::radians(0.0f), glm::vec3(0, 0, 1)); // Set t in radians to rotate object.

    // Rotation to X direction. Positive rotation around Y -> looking in negative x-dir
    glm::mat4 modelDirX = glm::rotate(scale, glm::radians(90.0f), glm::vec3(0, 1, 0)); // Set t in radians to rotate object.
    
    // Rotation to Y direction. Negative rotation around X -> looking in negative y-dir
    glm::mat4 modelDirY = glm::rotate(scale, glm::radians(-90.0f), glm::vec3(1, 0, 0)); // Set t in radians to rotate object.
    // Have to rotate around Y also (Don't know why it don't align with Z without this, might be that rotation is left from x-dir rot)
    modelDirY = glm::rotate(modelDirY, glm::radians(-90.0f), glm::vec3(0, 1, 0)); // Set t in radians to rotate object.
    
    // Translations to make center of bunny in center of screen. 
    modelDirZ = glm::translate(modelDirZ, glm::vec3(0, -0.1, 0));
    modelDirX = glm::translate(modelDirX, glm::vec3(0, -0.1, 0));
    modelDirY = glm::translate(modelDirY, glm::vec3(0, -0.1, 0));
    
    // --- Bind each FBO and draw accordingly
    // *** With setting less on*
    // Depthbuffer settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);   // Make closest fragments pass, (nearest)
    glClearDepth(1.0);      // Let fragments less than 1 pass

    // Scale factor for depth values, to make min and max differ more.
    float scaleFactor = 10; // Something magical with 10, might have something to do with voxel space bein 0-10 idk.

    depthShader->uploadFloat("scaleFactor", scaleFactor);

    FBOZ.bindFBO();
    // Upload rot matrix
    depthShader->uploadMat4("model", modelDirZ);
    // Set backgroundcolor
    glClearColor(0.9f, 0.6f, 0.6f, 1.0f);
    // Clear buffer, keep depth to use z-buffer in offscreen fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw everything to offscreen buffer
    bunny.draw(*depthShader);

    FBOX.bindFBO();
    // Upload rot matrix
    depthShader->uploadMat4("model", modelDirX);
    // Set backgroundcolor
    glClearColor(0.9f, 0.6f, 0.6f, 0.0f);
    // Clear buffer, keep depth to use z-buffer in offscreen fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw everything to offscreen buffer
    bunny.draw(*depthShader);

    FBOY.bindFBO();
    // Upload rot matrix
    depthShader->uploadMat4("model", modelDirY);
    // Set backgroundcolor
    glClearColor(0.9f, 0.6f, 0.6f, 0.0f);
    // Clear buffer, keep depth to use z-buffer in offscreen fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw everything to offscreen buffer
    bunny.draw(*depthShader);
    // *** 

    // *** With setting greater on*
    // Depthbuffer settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);    // Make furthest fragments pass, (nearest)
    glClearDepth(0.0);          // Have to set depth test to zero else nothing will pass

    FBOZGreater.bindFBO();
    // Upload rot matrix
    depthShader->uploadMat4("model", modelDirZ);
    // Set backgroundcolor
    glClearColor(0.9f, 0.6f, 0.6f, 0.0f);
    // Clear buffer, keep depth to use z-buffer in offscreen fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw everything to offscreen buffer
    bunny.draw(*depthShader);

    FBOXGreater.bindFBO();
    // Upload rot matrix
    depthShader->uploadMat4("model", modelDirX);
    // Set backgroundcolor
    glClearColor(0.9f, 0.6f, 0.6f, 0.0f);
    // Clear buffer, keep depth to use z-buffer in offscreen fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw everything to offscreen buffer
    bunny.draw(*depthShader);

    FBOYGreater.bindFBO();
    // Upload rot matrix
    depthShader->uploadMat4("model", modelDirY);
    // Set backgroundcolor
    glClearColor(0.9f, 0.6f, 0.6f, 0.0f);
    // Clear buffer, keep depth to use z-buffer in offscreen fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw everything to offscreen buffer
    bunny.draw(*depthShader);
    // *** 

    // Binds default screen FB back, do not really like this as a memberfunction
    FBOZ.bindScreenFB(); 
    // ------------------------------------
}

void Sandbox::drawTriangle()
{
    // BINDS
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    // draw our first triangle
    glUseProgram(sandboxShader->shaderID);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Sandbox::drawQuad()
{
    // BINDS
    unsigned int VBO, VAO, EBO, quadTexCoordBuffer;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


     glGenBuffers(1, &quadTexCoordBuffer);
     glBindBuffer(GL_ARRAY_BUFFER, quadTexCoordBuffer);
     glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexCoords), quadTexCoords, GL_STATIC_DRAW);

     glEnableVertexAttribArray(2);
     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    // upload matrices to shader 
    cubeShader->useProgram();
    cubeShader->uploadMat4("projection", projection);
    cubeShader->uploadMat4("view", view);
    cubeShader->uploadMat4("model", quadModel);

    // draw our first quad
    glUseProgram(cubeShader->shaderID);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0); // no need to unbind it every time 
}

// Scroll input
void Sandbox::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Sandbox* me = static_cast<Sandbox*>(glfwGetWindowUserPointer(window));
    float speed = (float)(0.05 * yoffset);
    me->cameraPos += speed * me->cameraUp;
    me->view = glm::lookAt(me->cameraPos, me->cameraPos + me->cameraFront, me->cameraUp);
}