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
    projection{glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f)},
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
    cube{Sphere(glm::vec3(0.0f, 0.0f, 0.0f), "assets/models/cubeplus.obj")}
    

{
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

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
        /*
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));	
        bunnyShader->uploadMat4("model", model);
        */
        //bunnyModel.Draw(*bunnyShader);
        
        bunny.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        bunny.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
        bunny.updateTransformation();
        bunnyShader->uploadMat4("model", bunny.getTransformation());
        bunny.draw(*bunnyShader);

        sphere.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        sphere.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
        sphere.updateTransformation();
        bunnyShader->uploadMat4("model", sphere.getTransformation());
        sphere.draw(*bunnyShader);
        
        cube.setPosition(glm::vec3(20.0f, 0.0f, 0.0f));
        cube.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
        cube.updateTransformation();
        bunnyShader->uploadMat4("model", cube.getTransformation());
        cube.draw(*bunnyShader);
        
        

        //std::cout << glGetError() << std::endl;

        // #######################

        // Swap front and back buffers 
        glfwSwapBuffers(window);
        // Poll for and process events 
        glfwPollEvents();
        /*
        if (count == 10)
        {
            break;

        }
        else 
        {
            count++;
        }
        */
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