#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Headers to functions below. Want to keep main at top. 
GLFWwindow* initGLFW_GLAD();
void display(GLFWwindow* window);

int main(void)
{
    GLFWwindow* window = nullptr;
    
    window = initGLFW_GLAD();
    
    if (!window)
    {
        // Some error happened in init function
        return -1;
    }

    display(window);

    glfwTerminate();
    return 0;
}

GLFWwindow* initGLFW_GLAD()
{
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return nullptr;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1080, 800, "Hello World", NULL, NULL);
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
    
    return window;
}

// Simple input handling function
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void display(GLFWwindow* window)
{
    // Set backgroundcolor
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Handle inputs
        processInput(window);

        // Render here 



        // Clear buffer 
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap front and back buffers 
        glfwSwapBuffers(window);

        // Poll for and process events 
        glfwPollEvents();

    }
}