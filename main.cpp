#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
// Selfwritten files
#include "sandbox.h"
#include "shader.h"

// Create a sandbox/world class that can hold on all shaders and objects etc.

// Headers to functions below. Want to keep main at top. 
GLFWwindow* initGLFW_GLAD();
void display(GLFWwindow* window);
void initShaders();

int main(void)
{
    GLFWwindow* window = nullptr;
    window = initGLFW_GLAD();

    if (!window)
    {
        // Some error happened in init function
        return -1;
    }
    
    Sandbox sandbox(window);

    sandbox.display();

    //display(window); Now done in sandbox! 

    glfwTerminate();
    return 0;
}

GLFWwindow* initGLFW_GLAD()
{
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return nullptr;

    // Must have this to get right version, plus compiling frameworks for mac. 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1080, 800, "Real Time Particle Physics Simulation", NULL, NULL);
    if (!window)
    {
        
        glfwTerminate();
        return nullptr;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Think you have to init glad after OpenGL context which is the two closest above function.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    } 

    // To validate that linking of libraries are correct    
    std::cout << "----GL Version:---------" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    std::cout << "----GLSL Version:---------" << std::endl;
    std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "----------------------------- \n" << std::endl;


    return window;
}


/* Now done in sandbox
// Simple input handling function
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
*/ 
/* Now done in sandbox! 
void display(GLFWwindow* window)
{
    glEnable(GL_DEPTH_TEST);
    Sandbox sandbox = Sandbox();

    // Loop until the user closes the window 
    while (!glfwWindowShouldClose(window))
    {
        // Handle inputs
        processInput(window);
 
        // Set backgroundcolor
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);

        // Clear buffer 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render here
        sandbox.drawTriangle();

        sandbox.drawQuad();

        // Swap front and back buffers 
        glfwSwapBuffers(window);

        // Poll for and process events 
        glfwPollEvents();

    }
}
*/

