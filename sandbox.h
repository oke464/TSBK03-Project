#ifndef _SANDBOX_H_
#define _SANDBOX_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include "shader.h"

class Sandbox
{
public:
    Sandbox(GLFWwindow* window);
    ~Sandbox();

    void processInput();
    void display();

    void drawTriangle(); // Move these to separate handlers later
    void drawQuad();        // Move these to separate handlers later
    

    Shader* sandboxShader{};
    glm::mat4 projection{};
    glm::mat4 view{};
    glm::mat4 quadModel{};
private:
    void initShaders();

    float triangleVertices[9]; // Triangle has 3 vertices each 3 coordninates
    float quadVertices[12]; // Quad 4 vertices
    unsigned int quadIndices[6]; // Used for EBO

    GLFWwindow* window{};
};

#endif