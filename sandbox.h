#ifndef _SANDBOX_H_
#define _SANDBOX_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include "learnOpenGL/shader.h"
#include "tiles.h"
#include "learnOpenGL/model.h"
#include "object.h"
#include "framebuffer.h"
#include "voxelhandler.h"

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
    glm::mat4 view;
    glm::mat4 quadModel{};

    Shader* bunnyShader;
    Shader* depthShader;
    Shader* cubeShader;
private:
    void initShaders();
    void genereteFBODepthTextures(Framebuffer FBO1, Framebuffer FBO2, Framebuffer FBO3, Framebuffer FBO4, Framebuffer FBO5, Framebuffer FBO6);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    float triangleVertices[9]; // Triangle has 3 vertices each 3 coordninates
    float quadVertices[12]; // Quad 4 vertices
    unsigned int quadIndices[6]; // Used for EBO

    GLFWwindow* window{};
    Tiles* bottomTiles{};

    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float yaw;
    float pitch;

    VoxelHandler* voxHandler;

    bool bunnyToggle;
    // TEMPORARY 
    Sphere bunny;
    Sphere cube;
    Sphere sphere;



};

#endif