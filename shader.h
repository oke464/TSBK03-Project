
// This Shader class is based on the class from: 
// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h
// Simply creates a shader and abstracts opengl uniform functions uploading variables to shaders/GPU

#ifndef _SHADER_H_
#define _SHADER_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int shaderID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath);
    
    // activate the shader
    // ------------------------------------------------------------------------
    void useProgram();

    // utility uniform functions
    // ------------------------------------------------------------------------
    void uploadBool(const std::string &name, bool value) const;
    // ------------------------------------------------------------------------
    void uploadInt(const std::string &name, int value) const;
    // ------------------------------------------------------------------------
    void uploadFloat(const std::string &name, float value) const;

    void uploadMat4(const std::string &name, glm::mat4 matrix) const;

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);
};
#endif