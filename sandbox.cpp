#include "sandbox.h"

#include <iostream>

Sandbox::Sandbox(GLFWwindow* window) :
    triangleVertices{-0.5f, -0.5f, 0.0f, // left  
                    0.5f, -0.5f, 0.0f, // right 
                    0.0f,  0.5f, 0.0f  // top
                    },
    quadVertices{
                0.5f,  0.5f, 0.0f,  // top right
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  // bottom left
                -0.5f,  0.5f, 0.0f   // top left 
                },
    quadIndices{  // note that we start from 0!
                0, 1, 3,   // first triangle
                1, 2, 3    // second triangle
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
    cubeShader(new Shader("cubeShader.vert", "cubeShader.frag"))
    

{
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Window width Height
    int wWidth, wHeight;
    glfwGetWindowSize(window, &wWidth, &wHeight);
    projection = glm::perspective(glm::radians(45.0f), (float)(wWidth / wHeight), 0.1f, 100.0f);

    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, scrollCallback);

    
}

Sandbox::~Sandbox()
{
    delete sandboxShader;
    delete bottomTiles;
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
}

// Main loop to display everything in scene.
void Sandbox::display()
{
    int count = 0;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE); 

    float prevTime = glfwGetTime();
    glm::vec3 moveDir(0, 0, 0);//(0, 0, -1);
    glm::vec3 transVec(0, 0, 0);

    int wWidth, wHeight;
    glfwGetWindowSize(window, &wWidth, &wHeight);
    Framebuffer depthFBO(wWidth, wHeight);
    /*

    std::string sourcePath = __FILE__;
    std::string dirPath = sourcePath.substr(0, sourcePath.rfind("/"));

    std::string modelPath = std::string(dirPath);
    modelPath.append("/").append("assets/models/stanford-bunny.obj");

    Model bunnyModel(modelPath);
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
        //drawQuad();
        //sandboxShader->useProgram(); // For now tileShader == sandboxShader
        //sandboxShader->uploadMat4("projection", projection);
        //sandboxShader->uploadMat4("view", view);


        //########### DEPTH TESTING STUFF
         float t = glfwGetTime() * 100;

        float currTime = glfwGetTime();
        float deltaT = (currTime - prevTime) / 2;
        prevTime = currTime;

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
        float near = 0.1f;
        float far = 10.0f;
        glm::mat4 depthProj = glm::ortho(left, right, bottom, top, near, far);
        depthShader->uploadMat4("dProj", depthProj);
        depthShader->uploadFloat("near", near);
        depthShader->uploadFloat("far", far);

        // Update translation vector dep on time
        transVec += deltaT * moveDir;
        
        //std::cout << transVec.z << std::endl;

        // Createe model = rot * trans 
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), transVec);
        glm::mat4 model = glm::rotate(trans, glm::radians(t), glm::vec3(0, 1, 0)); // Set t in radians to rotate object.
        depthShader->uploadMat4("model", model);
        
        depthFBO.bindFBO();
        // Set backgroundcolor
        glClearColor(0.9f, 0.6f, 0.6f, 1.0f);
        // Clear buffer, keep depth to use z-buffer in offscreen fbo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Draw everything to offscreen buffer
        bunny.draw(*depthShader);
        // Binds default screen FB, do not really like this as a memberfunction
        depthFBO.bindScreenFB(); 


        // ################# Things in scene
        //bottomTiles->drawTiles(sandboxShader, projection, view);
        sandboxShader->useProgram();
        sandboxShader->uploadMat4("projection", projection);
        sandboxShader->uploadMat4("view", view);
        
        bottomTiles->bindBuffersInstanced();
        bottomTiles->drawTilesInstanced();


        // Draw bunny
        bunnyShader->useProgram();
        bunnyShader->uploadMat4("projection", projection);
        bunnyShader->uploadMat4("view", view);

        bunny.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        bunny.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
        bunny.updateTransformation();
        bunnyShader->uploadMat4("model", bunny.getTransformation());
        bunny.draw(*bunnyShader);
        
        // Draw sphere
        bunnyShader->useProgram();
        bunnyShader->uploadMat4("projection", projection);
        bunnyShader->uploadMat4("view", view);
        sphere.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        sphere.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
        sphere.updateTransformation();
        bunnyShader->uploadMat4("model", sphere.getTransformation());
        sphere.draw(*bunnyShader);

/*  This is example to load texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // load and generate the texture
        int width, height, nrChannels;
        unsigned char *data = stbi_load("assets/textures/maskros512.tga", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
*/      
        // Draw cube
        cubeShader->useProgram();

        // set active texture to the one from fbo
        depthFBO.bindTex(cubeShader, "texUnit", 0);

        // Upload matrices and draw cube
        cubeShader->uploadMat4("projection", projection);
        cubeShader->uploadMat4("view", view);
        cube.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
        cube.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
        cube.updateTransformation();
        cubeShader->uploadMat4("model", cube.getTransformation());
        cube.draw(*cubeShader);


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
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    // upload matrices to shader 
    sandboxShader->uploadMat4("projection", projection);
    sandboxShader->uploadMat4("view", view);
    sandboxShader->uploadMat4("model", quadModel);

    // draw our first quad
    glUseProgram(sandboxShader->shaderID);
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